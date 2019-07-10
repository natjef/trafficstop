/*
    This sketch sends a message to a TCP server

*/

#include <WiFi.h>
#include <WiFiMulti.h>
#include <M5Stack.h>

WiFiMulti WiFiMulti;
WiFiClient client;


void setup()
{
  // We start by connecting to a WiFi network
  WiFiMulti.addAP("trafficbot", "trafficbot");
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

  const uint16_t port = 80;
  const char * host = "192.168.4.2"; // ip or dns

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections

  delay(500);

  while (!client.connect(host, port)) {
    Serial.println("Connection failed.");
    Serial.println("Waiting 5 seconds before retrying...");
    delay(5000);
  }
}

void loop()
{
  M5.Lcd.print("Loop Entered.");
  // This will send a request to the server
  client.print("Hello! it is I, a robot!");

  //read back one line from the server
  String line = client.readStringUntil('\r');
  Serial.println(line);
  delay(5000);
}
