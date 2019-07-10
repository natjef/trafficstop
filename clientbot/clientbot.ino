#include <M5Stack.h>
#include "FastLED.h"
#include "BluetoothSerial.h"
#include <NewPing.h>
#include <QTRSensors.h>
#include <WiFi.h>
#include <WiFiMulti.h>

// LED definitions
#define Neopixel_PIN 1
#define NUM_LEDS 7
CRGB leds[NUM_LEDS];

//line sensor declarations
QTRSensors qtr;
const uint8_t SensorCount = 3;
uint16_t sensorValues[SensorCount];

// Motor definitions
#define INCH_TO_UNIT 100/6.68
const int straightSpeed = 20000;
const int turnSpeed = 15000;
const double cornerToLeft = 12; //TODO MEASURE
const double cornerToRight = 2; //TODO MEASURE
const double driveWidth = 6; //TODO MEASURE

// networking definitions
WiFiMulti WiFiMulti;
const uint16_t port = 80;
const char * host = "192.168.1.1"; // ip or dns

// ultrasonic senor defs
int trigPin = 26;   // Trigger
int echoPin = 13;    // Echo
int maxDistance = 200;
long duration, cm;

NewPing sonar(trigPin, echoPin, maxDistance);

int choice = 0;

void setup() {
  M5.begin();

  FastLED.addLeds<WS2811, Neopixel_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear();

  M5.Lcd.println("LOADING...");

  // setup Nano communcation
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.begin(115200);

  //Line Sensor init
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]) {
    2, 5, 15
  }, SensorCount);

  // Networking init - heavy
  WiFiMulti.addAP("trafficbot", "trafficbot"); // user, pass

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.print("Waiting for WiFi");

  int i = 0;

  while (WiFiMulti.run() != WL_CONNECTED) {
    M5.Lcd.print(".");
    i++
    if (i == 3) {
      M5.Lcd.clear();
      M5.Lcd.println("LOADING...\nWaiting for Wifi");
    }
    delay(500);
  }

  M5.Lcd.print("WiFi connected! IP address: ")
  M5.Lcd.println(WiFi.localIP());

  WiFiClient client;
  client.connect(host, port);  

  randomSeed(analogRead(0));

  // give grbl time to set up - MIGHT BE UNNEEDED SINCE WIFI TAKES TIME ANYWAY
  delay(1000);

  // display info about the program
  M5.Lcd.clear();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.setCursor(40, 0);
  M5.Lcd.println("Traffic Stop");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 25);
}

void loop() {
  choice = random(3);

  M5.Lcd.clear();
  M5.Lcd.setTextSize(4);
  if (choice == 0) M5.Lcd.print("LEFT");
  else if (choice == 1) M5.Lcd.print("STRAIGHT");
  else if (choice == 2) M5.Lcd.print("RIGHT");

  //move indefinitely forward to the intersection
  drive(10000, 10000, 10000);

  //wait for line sensor to be triggered by intersection
  do {
    qtr.read(sensorValues);
    delay(1);
  } while ( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

  stop();

  //wait for its turn to go
  delay(3000);

  //go
  if (choice == 0) left();
  else if (choice == 1) straight();
  else if (choice == 2) right();

  delay(3000);
  drive(100, 100, 10000);

  delay(10000);
}

void drive(double left, double right, double speed) {
  Serial2.printf("$J = G21 G91 X%f Y%f F%f", left, right, speed);
  Serial2.print("\r\n\r\n");
}

void straight() {
  drive(350, 350, straightSpeed);
}

void left() {
  drive(.5 * PI * cornerToLeft * INCH_TO_UNIT, .5 * PI * (cornerToLeft + driveWidth) * INCH_TO_UNIT, turnSpeed);
}

void right() {
  drive(.5 * PI * (cornerToRight + driveWidth) * INCH_TO_UNIT, .5 * PI * cornerToRight * INCH_TO_UNIT, turnSpeed);
}

void stop() {
  Serial2.print((char)133);
  Serial2.print("\r\n\r\n");
}
