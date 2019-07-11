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
const double cornerToLeft = 12; //12
const double cornerToRight = 2; //2
const double driveWidth = 6; //6

String choice = "none";
boolean obstacle = 1;
long waitTime = random(2000, 5000);


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
  FastLED.show();

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.print("LOADING...");

  M5.Lcd.setCursor(0, 15);
  M5.Lcd.print("Press a button to go left, straight or right");
  M5.Lcd.setCursor(0, 55);
  M5.Lcd.print("Button A -> Left");
  M5.Lcd.setCursor(0, 70);
  M5.Lcd.print("Button B -> Straight");
  M5.Lcd.setCursor(0, 85);
  M5.Lcd.print("Button C -> Right");

  while (choice == "none")
  {
    if (M5.BtnA.wasPressed())
    {
      choice = "left";
      M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(0,0);
    }
    else if (M5.BtnB.wasPressed())
    {
      choice = "straight";
      M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(0,0);
    }
    else if (M5.BtnC.wasPressed())
    {
      choice = "right";
      M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(0,0);
    }
    M5.update();
  }

  M5.Lcd.setTextSize(3);
  M5.Lcd.print(choice);
  delay(2000);

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
  delay(waitTime);
  drive(200, 200, straightSpeed);

    //wait for line sensor to be triggered by intersection
  do{
       qtr.read(sensorValues);
       delay(1);
   } while( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

    stop();
}

void loop() {
  
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
  
  if (choice == "straight") straight();
  else if (choice == "left") left();
  else right();
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

      qtr.read(sensorValues);
      delay(1);
      if ((sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 > 1000)
      {
        obstacle = 0;
        break;
      }
      
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
    
    if (obstacle) drive(350, 350, straightSpeed);

    //wait for line sensor to be triggered by intersection
    do {
      qtr.read(sensorValues);
      delay(1);
    } while( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

    stop();
    delay(5000);
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

      qtr.read(sensorValues);
      delay(1);
      if ((sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 > 1000) 
      {
        obstacle = 0;
        break;
      }
      
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
    
    if (obstacle) drive(.5 * PI * cornerToLeft * INCH_TO_UNIT, .5 * PI * (cornerToLeft + driveWidth) * INCH_TO_UNIT, turnSpeed);

    //wait for line sensor to be triggered by intersection
    do {
      qtr.read(sensorValues);
      delay(1);
    } while( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

    stop();
    delay(5000);
}

void right() {
  drive(25, 25, straightSpeed);
  drive(.5 * PI * (cornerToRight + driveWidth) * INCH_TO_UNIT, .5 * PI * cornerToRight * INCH_TO_UNIT, turnSpeed);
  delay(100);
  drive (200, 200, straightSpeed);

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

      qtr.read(sensorValues);
      delay(1);
      if ((sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 > 1000) 
      {
        obstacle = 0;
        break;
      }
      
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

    if (obstacle) 
    {
      drive(.5 * PI * (cornerToRight + driveWidth) * INCH_TO_UNIT, .5 * PI * cornerToRight * INCH_TO_UNIT, turnSpeed);
    }

    do {
      qtr.read(sensorValues);
      delay(1);
    } while( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

    stop();
    delay(5000);
}

void stop() {
  Serial2.print((char)133);
  Serial2.print("\r\n\r\n");
}
