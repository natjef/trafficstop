/*
    This sketch sends a message to a TCP server

*/

#include <WiFi.h>
#include <WiFiMulti.h>
#include <M5Stack.h>
#include <NewPing.h>
#include <QTRSensors.h>

#define INCH_TO_UNIT 100/6.68

//line sensor declarations
QTRSensors qtr;
const uint8_t SensorCount = 3;
uint16_t sensorValues[SensorCount];

// Motor definitions
const int straightSpeed = 10000;
const int turnSpeed = 8000;
const double cornerToLeft = 10; //12
const double cornerToLeftArya = 15;
const double cornerToRight = 2; //2
const double driveWidth = 6; //6

String choice = "none";
boolean obstacle = 1;
long waitTime = random(2000, 5000);

// ultrasonic senor defs
int trigPin = 26;   // Trigger
int echoPin = 13;    // Echo
int maxDistance = 200;
long duration, cm;

NewPing sonar(trigPin, echoPin, maxDistance);


WiFiMulti WiFiMulti;
WiFiClient client;
const uint16_t port = 80;
const char * host = "192.168.4.1"; // ip or dns
const String space = " ";

int id=0;

void setup()
{
  // We start by connecting to a WiFi network
  WiFiMulti.addAP("trafficbot", "Tiuk34562");
  M5.begin();

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.print("Waiting for WiFi");

    //Line Sensor init
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]) {2,5,15}, SensorCount);

  // setup Nano communcation
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.begin(115200);

  //Distance Sensor init
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
    
  randomSeed(analogRead(0));

  int i = 0;

  while (WiFiMulti.run() != WL_CONNECTED) {
    M5.Lcd.print(".");
    i++;
    if (i == 3) {
      M5.Lcd.clear();
      M5.Lcd.print("Waiting for Wifi");
    }
    delay(500);
  }

  M5.Lcd.clear();
  M5.Lcd.println("WiFi connected! IP address: ");
  M5.Lcd.println(WiFi.localIP());

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections

  delay(500);
}

bool once = true;

void loop()
{
  while(!id) {
    if (!client.connect(host, port)) {
      Serial.println("Connection failed.");
      Serial.println("Waiting 5 seconds before retrying..."); 
      delay(5000);
      return;
    }
    delay(500);
    //read back one line from the server
    if(once) {
      sendMessage("HI");
      once = false;
    }
    id = waitForMessage().toInt();
    Serial.println(id);
    delay(50);
  }

  M5.Lcd.printf("My id is %d\n", id); 

  
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
  
  //stop at interseciton
  // give grbl time to set up
  delay(waitTime);
  drive(200, 200, straightSpeed);

  //wait for line sensor to be triggered by intersection
  do{
       qtr.read(sensorValues);
       delay(1);
   } while( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

  stop();

  M5.Lcd.println("Stopping at intersection");
  //after arriving at intersection
  sendMessage("ARRIVED");

  int receivedId = 0;
  do {
    receivedId = waitForMessage().substring(0,1).toInt();
    M5.Lcd.printf("ID received is %d\n", receivedId);
  } while(receivedId != id);

  //go
  M5.Lcd.println("going!");
  
  if (choice == "straight") straight();
  else if (choice == "left") left();
  else right();

  M5.Lcd.println("completed intersection!");
  const String done = "DONE";
  client.println(id + space + done);
  delay(100000);
}

void sendMessage(String message) {
  while (!client.connect(host, port)) {
      Serial.println("Connection failed.");
      Serial.println("Waiting 5 seconds before retrying..."); 
      delay(5000);
  }
  String send = id + space + message;
  client.println(send);
}

String waitForMessage() {

  String message;
  do {
    message = client.readStringUntil('\r');
    Serial.println("waiting");
  } while(message == "");

  return message;
}

void drive(double left, double right, double speed) {
  Serial2.printf("$J = G21 G91 X%f Y%f F%f", left, right, speed);
  Serial2.print("\r\n\r\n");
}

void straight() {
    drive(450, 450, straightSpeed);
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
    
    if (obstacle) drive(450, 450, straightSpeed);

    //wait for line sensor to be triggered by intersection
    do {
      qtr.read(sensorValues);
      delay(1);
    } while( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

    stop();
    delay(50);
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

    drive(100, 100, straightSpeed);

    //wait for line sensor to be triggered by intersection
    do {
      qtr.read(sensorValues);
      delay(1);
    } while( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

    stop();
      
    delay(50);
}

void leftArya() {
    
    drive(.75 * PI * cornerToLeftArya * INCH_TO_UNIT, .75 * PI * (cornerToLeftArya + driveWidth) * INCH_TO_UNIT, turnSpeed);
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
    
    if (obstacle) drive(.5 * PI * cornerToLeftArya * INCH_TO_UNIT, .5 * PI * (cornerToLeftArya + driveWidth) * INCH_TO_UNIT, turnSpeed);

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
    drive(100, 100, straightSpeed);

    do {
      qtr.read(sensorValues);
      delay(1);
    } while( (sensorValues[0] + sensorValues[1] + sensorValues[2]) / 3 < 1000);

    stop();
    delay(50);
}

void stop() {
  Serial2.print((char)133);
  Serial2.print("\r\n\r\n");
}
