#include <M5Stack.h>
#include "FastLED.h"
#include "BluetoothSerial.h"
#include <NewPing.h>

// LED definitions
#define Neopixel_PIN 1
#define NUM_LEDS 7
CRGB leds[NUM_LEDS];

// Motor definitions
int forwardSpeed = 20000;
int reverseSpeed = 10000;
int turnSpeed = 5000;
int driveDistance = 200;
int turnDistance = 33;
int pauseTime = 3000;
boolean headlights = false;

// bluetooth definitions
BluetoothSerial SerialBT;

// ultrasonic senor defs
int trigPin = 26;   // Trigger
int echoPin = 13;    // Echo
int maxDistance = 200;
long duration, cm;

NewPing sonar(trigPin, echoPin, maxDistance);

void setup() {
  M5.begin();

  FastLED.addLeds<WS2811, Neopixel_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear();

  M5.Lcd.print("LOADING...");

  // setup Nano communcation
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.begin(115200);
  SerialBT.begin("RobotDowneyJr");

  // Distance communication


  // give grbl time to set up
  delay(1000);

  // display info about the program
  M5.Lcd.clear();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.setCursor(40, 0);
  M5.Lcd.println("Bluetooth Test");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 25);
  M5.Lcd.println("\n- Send F: Drive Forward");
  M5.Lcd.println("\n- Send B: Drive Backward");
  M5.Lcd.println("\n- Send L: Turn Left");
  M5.Lcd.println("\n- Send R: Turn Right"); // gambol
}

void loop() {
  if (headlights) {
    fill_solid(leds, NUM_LEDS, CRGB::White);
  }
  else {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
  FastLED.show();

  char in = SerialBT.read();

  if (in == 'F') {
    driveForward();
  }
  else if (in == 'B') {
    driveBackward();
  }
  else if (in == 'R') {
    turnRight();
  }
  else if (in == 'L') {
    turnLeft();
  }
  else if (in == 'H') {
    headlights = !headlights;
  }
  else if (in == 'D') {
    SerialBT.print(sonar.ping_cm());
  }
  else if (in == 'F') {
    follow();
  }
}

void driveForward() {
  Serial2.printf("$J = G21 G91 X%d Y%d F%d", driveDistance, driveDistance, forwardSpeed);
  Serial2.print("\r\n\r\n");
}

void driveBackward() {
  Serial2.printf("$J = G21 G91 X%d Y%d F%d", -driveDistance, -driveDistance, reverseSpeed);
  Serial2.print("\r\n\r\n");
}

void turnLeft() {
  Serial2.printf("$J = G21 G91 X%d Y%d F%d", -turnDistance, turnDistance, turnSpeed);
  Serial2.print("\r\n\r\n");
}

void turnRight() {
  Serial2.printf("$J = G21 G91 X%d Y%d F%d", turnDistance, -turnDistance, turnSpeed);
  Serial2.print("\r\n\r\n");
}

void follow() {
  
}
