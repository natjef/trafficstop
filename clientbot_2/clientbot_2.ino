/*
    This sketch sends a message to a TCP server

*/

#include <WiFi.h>
#include <WiFiMulti.h>
#include <M5Stack.h>

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
    
    M5.Lcd.print("Loop Entered.");
  
    //read back one line from the server
    String line = client.readStringUntil('\r');
    id = line.toInt();    
    sendMessage("HI");
    delay(500);
  }

  sendMessage("ARRIVED");

  int receivedId = 0;
  do {
    receivedId = waitForMessage.substring(0,1).toInt();
  } while(receivedId != id);

  //go
  ifc
  delay(100);
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
  while (!client.connect(host, port)) {
      Serial.println("Connection failed.");
      Serial.println("Waiting 5 seconds before retrying..."); 
      delay(5000);
  }

  String message;
  do {
    message = client.readStringUntil('\r');
    Serial.println("waiting for message");
    delay(10);
  } while(message == "");

  return message;
}
