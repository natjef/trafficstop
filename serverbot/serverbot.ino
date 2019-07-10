#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <M5Stack.h>

const char *ssid = "trafficbot";
const char *password = "Tiuk34562";
WiFiServer server(80);
int numBots = 0;

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Configuring access point...");

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  M5.Lcd.print("AP IP address: ");
  M5.Lcd.println(myIP);
  server.begin();

  M5.Lcd.println("Server started! Check serial monitor for debug.");

}

void loop() {
  // get everyone connected - #TODO: MAKE THIS A LOOP + ARRAY

  WiFiClient client = server.available();
  WiFiClient client2 = server.available();

  if (client) {
    Serial.println("New Client.");
    String l = client.readStringUntil('\r');
    Serial.println(l);
  }

  if(client2) {
    Serial.println("client 2");
    String l = client2.readStringUntil('\r');
    Serial.println(l); 
  }
}
