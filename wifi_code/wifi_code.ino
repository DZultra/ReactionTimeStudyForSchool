#include <SPI.h>
#include <WiFiNINA.h>
#include <Servo.h>

const int RGB_RED_PIN = 13;
const int RGB_BLUE_PIN = 12;
const int RGB_GREEN_PIN = 11;
const int PIEZO_PIN = 10;
const int SERVO_PIN = 9;
const int TASTER_PIN = 8;

char ssid[] = "MyArduinoAP";
char pass[] = "12345678";

int current_angle;
int initial_pos = 90;

int status = WL_IDLE_STATUS;
WiFiServer server(80);
String displayText = "";

Servo Servo;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  servoSetup();
  piezoSetup();
  wifiSetup();

  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  pinMode(TASTER_PIN, INPUT_PULLUP);
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        if (request.endsWith("\r\n\r\n")) {
          break;
        }
      }
    }

    handleRequest(request);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    createWebsite(client);

    delay(10);
    client.stop();
  }
}

void createWebsite(WiFiClient client ) {
  client.println("<!DOCTYPE html>");
  client.println("<html><head><title>Studie</title>");
  client.println("<meta charset='UTF-8'>");
  client.println("<style>");
  client.println("body { text-align: center; font-family: Arial; margin-top: 50px; }");
  client.println(".btn { padding: 15px 30px; margin: 10px; font-size: 18px; }");
  client.println("</style></head><body>");

  client.print("<input type='text' readonly value='");
  client.print(displayText);
  client.println("' style='font-size:20px; text-align:center; margin-bottom:30px; width:300px;'><br>");

  client.println("<a href='/action?cmd=btn1'><button class='btn'>Sehsinn</button></a>");
  client.println("<a href='/action?cmd=btn2'><button class='btn'>Gehörsinn</button></a>");
  client.println("<a href='/action?cmd=btn3'><button class='btn'>Tastsinn</button></a>");
  client.println("<br>");
  client.println("<a href='/action?cmd=btn4'><button class='btn'>Motor +</button></a>");
  client.println("<a href='/action?cmd=btn5'><button class='btn'>Motor -</button></a>");

  client.println("</body></html>");
}

void handleRequest(String request) {
  if (request.indexOf("GET /action?cmd=btn1") >= 0) {
    onSehsinn();
  } else if (request.indexOf("GET /action?cmd=btn2") >= 0) {
    onGehoersinn();
  } else if (request.indexOf("GET /action?cmd=btn3") >= 0) {
    onTastsinn();
  } else if (request.indexOf("GET /action?cmd=btn4") >= 0) {
    onAddMotor();
  } else if (request.indexOf("GET /action?cmd=btn5") >= 0) {
    onLowerMotor();
  }
}

// === Helper Functions ===
void updateDisplayText(String newText) {
  displayText = newText;
}

// === Button Functions ===
void onSehsinn() {
  Serial.println("[Sehsinn] Startet Reaktionstest...");

  // Rot an
  digitalWrite(RGB_RED_PIN, HIGH);
  digitalWrite(RGB_GREEN_PIN, LOW);
  digitalWrite(RGB_BLUE_PIN, LOW);

  // Warte 5–10 Sekunden (5000–10000 ms)
  int waitTime = random(5000, 10001);
  delay(waitTime);

  // Grün an
  digitalWrite(RGB_RED_PIN, LOW);
  digitalWrite(RGB_GREEN_PIN, HIGH);
  digitalWrite(RGB_BLUE_PIN, LOW);

  Serial.println("[Sehsinn] Grün aktiviert. Warte auf Tastendruck...");

  unsigned long greenTime = millis();

  // Warte auf Button-Presse
  while (digitalRead(TASTER_PIN) == HIGH) {
    // tue nichts, bis gedrückt
  }

  unsigned long reactionTime = millis() - greenTime;
  Serial.print("[Sehsinn] Reaktionszeit: ");
  Serial.print(reactionTime);
  Serial.println(" ms");
  updateDisplayText(String(reactionTime) + "ms");
  // LED aus
  digitalWrite(RGB_RED_PIN, LOW);
  digitalWrite(RGB_GREEN_PIN, LOW);
  digitalWrite(RGB_BLUE_PIN, LOW);
}

void onGehoersinn() {
  Serial.println("[Gehörsinn] Startet Reaktionstest...");

  // Warte 5–10 Sekunden
  int waitTime = random(5000, 10001);
  delay(waitTime);

  // Ton abspielen
  Serial.println("[Gehörsinn] Ton aktiviert. Warte auf Tastendruck...");
  tone(PIEZO_PIN, 750); // 1000 Hz-Ton
  unsigned long toneStartTime = millis();

  // Auf Taster-Eingabe warten
  while (digitalRead(TASTER_PIN) == HIGH) {
    // Warten bis gedrückt
  }

  unsigned long reactionTime = millis() - toneStartTime;
  Serial.print("[Gehörsinn] Reaktionszeit: ");
  Serial.print(reactionTime);
  Serial.println(" ms");
  noTone(PIEZO_PIN);

  updateDisplayText(String(reactionTime) + "ms");
}


void onTastsinn() {
  Serial.println("[Tastsinn] Startet Reaktionstest...");

  // Warte 5–10 Sekunden
  int waitTime = random(5000, 10001);
  delay(waitTime);

  // Servo bewegt sich um 10 Grad
  int moveBy = 20;
  current_angle += moveBy;
  Servo.write(current_angle);
  Serial.println("[Tastsinn] Servo bewegt. Warte auf Tastendruck...");

  // Zeit merken
  unsigned long movementTime = millis();

  // Warte auf Taster-Eingabe
  while (digitalRead(TASTER_PIN) == HIGH) {
    // tue nichts, bis gedrückt
  }

  unsigned long reactionTime = millis() - movementTime;
  Serial.print("[Tastsinn] Reaktionszeit: ");
  Serial.print(reactionTime);
  Serial.println(" ms");

  updateDisplayText(String(reactionTime) + "ms");

  current_angle -= moveBy;
  Servo.write(current_angle);
}

void onAddMotor() {
  Serial.println("[Motor] +2 ");
  current_angle += 4;
  Servo.write(current_angle);
}

void onLowerMotor() {
  Serial.println("[Motor] -2 ");
  current_angle -= 4;
  Servo.write(current_angle);
}

// === Setup Functions ===

void servoSetup() {
  Servo.attach(SERVO_PIN);
  Servo.write(initial_pos);
  current_angle = Servo.read();
}

void wifiSetup() {
  while (!Serial);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found!");
    while (true);
  }

  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Failed to start Access Point.");
    while (true);
  }

  server.begin();
  Serial.println("Access Point started!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void piezoSetup() {
  pinMode(PIEZO_PIN, OUTPUT);
  noTone(PIEZO_PIN);
}
