#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <M5Stack.h>

const char *ssid = "trafficbot";
const char *password = "trafficbot";
WiFiServer server(80);
int numBots = 0;

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Configuring access point...");

  WiFi.softAP(ssid, password);
  IPAddress myIP = IPAddress(192, 168, 1, 1);
  M5.Lcd.print("AP IP address: ");
  M5.Lcd.println(myIP);
  server.begin();

  M5.Lcd.println("Server started! Check serial monitor for debug.");

}

void loop() {
  // get everyone connected - #TODO: MAKE THIS A LOOP + ARRAY

  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    client.printf("You are Bot #%d", numBots++);
    String curline = "";
    while (client.connected()) {
      char c = client.read();
      Serial.write(c);
    }

    client.stop();
    Serial.println("Client Disconnected.");
  }
}
