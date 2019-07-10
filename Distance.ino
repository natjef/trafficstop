#include <M5Stack.h>
#include "FastLED.h"
#include "BluetoothSerial.h"
#include <NewPing.h>
#include <QTRSensors.h>

#define INCH_TO_UNIT 100/6.68

// LED definitions
#define Neopixel_PIN 1
#define NUM_LEDS 7
CRGB leds[NUM_LEDS];

//line sensor declarations
QTRSensors qtr;
const uint8_t SensorCount = 3;
uint16_t sensorValues[SensorCount];

// Motor definitions
const int straightSpeed = 10000;
const int turnSpeed = 8000;
const double cornerToLeft = 15; //12
const double cornerToRight = 2; //2
const double driveWidth = 6; //6


// bluetooth definitions
BluetoothSerial SerialBT;

// ultrasonic senor defs
int trigPin = 26;   // Trigger
int echoPin = 13;    // Echo
int maxDistance = 200;
long duration, cm;

NewPing sonar(trigPin, echoPin, maxDistance);


int choice=0;

void setup() {
  M5.begin();

  FastLED.addLeds<WS2811, Neopixel_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear();
  FastLED.show();

  M5.Lcd.print("LOADING...");

  //Line Sensor init
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]) {2,5,15}, SensorCount);
  
  // M5 LCD Setup
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setCursor(10,10);
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(10,10);
  

  // setup Nano communcation
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.begin(115200);

  //Distance Sensor init
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
    
  randomSeed(analogRead(0));

  // give grbl time to set up
  delay(1000);
}

void loop() {
  choice = random(3);
  
  //clears trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  cm = duration*0.034/2;

  // Prints the distance on the Serial Monitor
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setCursor(10,10);
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(10,10);
  M5.Lcd.print("Distance: ");
  M5.Lcd.print(cm);

  //wait for its turn to go
  delay(500);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  cm = duration*0.034/2;
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setCursor(10,10);
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(10,10);
  M5.Lcd.print("Distance: ");
  M5.Lcd.print(cm);
  
  left();
}

void drive(double left, double right, double speed) {
  Serial2.printf("$J = G21 G91 X%f Y%f F%f", left, right, speed);
  Serial2.print("\r\n\r\n");
}

void straight() {
    drive(350, 350, straightSpeed);
    do {
      
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      cm = duration*0.034/2;

      M5.Lcd.setTextSize(3);
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.setCursor(10,10);
      M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(10,10);
      M5.Lcd.print("Distance: ");
      M5.Lcd.print(cm);
    } while (cm > 7);
        
    stop();
    
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setCursor(10,10);
    M5.Lcd.clear(BLACK);
    M5.Lcd.setCursor(10,10);
    
    do{
      
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      cm = duration*0.034/2;
      
      M5.Lcd.setTextSize(3);
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.setCursor(10,10);
      M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(10,10);
      M5.Lcd.print("Distance Straight: ");
      M5.Lcd.print(cm);
      delay(1);
    } while (cm <= 7);
    
    drive(350, 350, straightSpeed);

    //wait for line sensor to be triggered by intersection
    do {
      qtr.read(sensorValues);
      delay(1);
    } while( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

    stop();
}

void left() {
    drive(.75 * PI * cornerToLeft * INCH_TO_UNIT, .75 * PI * (cornerToLeft + driveWidth) * INCH_TO_UNIT, turnSpeed);
    do {
      
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      cm = duration*0.034/2;

      M5.Lcd.setTextSize(3);
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.setCursor(10,10);
      M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(10,10);
      M5.Lcd.print("Distance: ");
      M5.Lcd.print(cm);
    } while (cm > 7);
        
    stop();
    
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setCursor(10,10);
    M5.Lcd.clear(BLACK);
    M5.Lcd.setCursor(10,10);
    
    do{
      
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      cm = duration*0.034/2;
      
      M5.Lcd.setTextSize(3);
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.setCursor(10,10);
      M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(10,10);
      M5.Lcd.print("Distance Left: ");
      M5.Lcd.print(cm);
      delay(1);
    } while (cm <= 7);
    
    drive(.5 * PI * cornerToLeft * INCH_TO_UNIT, .5 * PI * (cornerToLeft + driveWidth) * INCH_TO_UNIT, turnSpeed);

    //wait for line sensor to be triggered by intersection
    do {
      qtr.read(sensorValues);
      delay(1);
    } while( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

    stop();
}

void right() {
  drive(.5 * PI * (cornerToRight + driveWidth) * INCH_TO_UNIT, .5 * PI * cornerToRight * INCH_TO_UNIT, turnSpeed);
}

void stop() {
  Serial2.print((char)133);
  Serial2.print("\r\n\r\n");
}
