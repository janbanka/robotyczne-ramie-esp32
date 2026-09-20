#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "index.h"

//KONFIGURACJA SIECI
const char* ssid = "Robot_ESP32";
const char* password = "password123";

//PINY
const int servo1Pin = 13; 
const int servo2Pin = 14; 
const int servo3Pin = 27; 
const int servo4Pin = 26; 
const int servo5Pin = 25; 

//OBIEKTY
WebServer server(80);
Servo servo1, servo2, servo3, servo4, servo5;

//POZYCJE DOCELOWE
int targetS1 = 90;
int targetS2 = 90;
int targetS3 = 90;
int targetS4 = 90;
int targetS5 = 100;

//POZYCJE AKTUALNE
float currentS1 = 90.0;
float currentS2 = 90.0;
float currentS3 = 90.0;
float currentS4 = 90.0;
float currentS5 = 100.0;

//USTAWIENIA PRĘDKOŚCI
// 1 = Wolno (2s), 2 = Średnio (1s), 3 = Szybko (0.5s)
int speedMode = 2; // Domyślnie średnio

//TIMER PĘTLI
unsigned long lastUpdate = 0;
const int UPDATE_INTERVAL = 20;

float getStepSize() {
  // Pętla wykonuje się 50 razy na sekundę, 1000ms / 20ms = 50Hz
  if (speedMode == 1) { 
    // WOLNO: Pełny ruch w 2 sekundy.
    // 2 sekundy = 100 cykli. 
    // 180 / 100 = 1.8 stopnia na cykl.
    return 1.8; 
  } 
  else if (speedMode == 2) { 
    // ŚREDNIO: Pełny ruch w 1 sekundę.
    // 1 sekunda = 50 cykli.
    // 180 / 50 = 3.6 stopnia na cykl.
    return 3.6; 
  } 
  else { 
    // SZYBKO: Pełny ruch w 0.5 sekundy.
    // 0.5 sekundy = 25 cykli.
    // 180 / 25 = 7.2 stopnia na cykl.
    return 7.2; 
  }
}

//OBSŁUGA STRONY
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleServo() {
  if (server.hasArg("nr") && server.hasArg("angle")) {
    int nr = server.arg("nr").toInt();
    int angle = server.arg("angle").toInt();
    switch(nr) {
      case 1: targetS1 = angle; break;
      case 2: targetS2 = angle; break;
      case 3: targetS3 = angle; break;
      case 4: targetS4 = angle; break;
      case 5: targetS5 = angle; break;
    }
    server.send(200, "text/plain", "OK");
  } else { server.send(400, "text/plain", "Bad Request"); }
}

void handleMoveAll() {
  if(server.hasArg("s1") && server.hasArg("s5")) {
    targetS1 = server.arg("s1").toInt();
    targetS2 = server.arg("s2").toInt();
    targetS3 = server.arg("s3").toInt();
    targetS4 = server.arg("s4").toInt();
    targetS5 = server.arg("s5").toInt();
    server.send(200, "text/plain", "OK");
  } else server.send(400, "text/plain", "Data missing");
}

void handleRead() {
  String data = String(targetS1) + ";" + String(targetS2) + ";" + String(targetS3) + ";" + String(targetS4) + ";" + String(targetS5);
  server.send(200, "text/plain", data);
}

//OBSŁUGA ZMIANY PRĘDKOŚCI
void handleSpeed() {
  if (server.hasArg("val")) {
    speedMode = server.arg("val").toInt();
    if (speedMode < 1) speedMode = 1;
    if (speedMode > 3) speedMode = 3;
    Serial.printf("Zmieniono tryb predkosci na: %d\n", speedMode);
  }
  server.send(200, "text/plain", "OK");
}

void handleSave() {
  server.send(200, "text/plain", "OK");
}

//PŁYNNY RUCH
void smoothMove() {
  float step = getStepSize();
  auto moveTowards = [&](float current, int target, float stepVal) -> float {
    if (abs(target - current) < stepVal) {
      return (float)target;
    }
    if (current < target) return current + stepVal;
    else return current - stepVal;
  };

  currentS1 = moveTowards(currentS1, targetS1, step);
  currentS2 = moveTowards(currentS2, targetS2, step);
  currentS3 = moveTowards(currentS3, targetS3, step);
  currentS4 = moveTowards(currentS4, targetS4, step);
  currentS5 = moveTowards(currentS5, targetS5, step * 2.0); 

  //ZAPIS DO SERW
  servo1.write((int)currentS1);
  servo2.write(constrain((int)currentS2, 90, 180));
  servo3.write((int)currentS3);
  servo4.write((int)currentS4);
  int valS5 = (int)currentS5;
  int angle5 = map(valS5, 0, 100, 100, 180);
  servo5.write(angle5);
}

void setup() {
  Serial.begin(115200);

  servo1.setPeriodHertz(50); 
  servo1.attach(servo1Pin, 500, 2400);
  servo2.setPeriodHertz(50); 
  servo2.attach(servo2Pin, 500, 2400);
  servo3.setPeriodHertz(50); 
  servo3.attach(servo3Pin, 500, 2400);
  servo4.setPeriodHertz(50); 
  servo4.attach(servo4Pin, 500, 2400);
  servo5.setPeriodHertz(50); 
  servo5.attach(servo5Pin, 500, 2400);

  servo1.write(targetS1);
  servo2.write(targetS2);
  servo3.write(targetS3);
  servo4.write(targetS4);
  servo5.write(map(targetS5, 0, 100, 100, 180));

  Serial.println("Start AP");
  WiFi.softAP(ssid, password);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/servo", handleServo);
  server.on("/moveAll", handleMoveAll);
  server.on("/read", handleRead);
  server.on("/speed", handleSpeed);
  server.on("/save", handleSave);

  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = currentMillis;
    smoothMove();
  }
}