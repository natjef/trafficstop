#include <queue>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <M5Stack.h> 

const char *ssid = "trafficbot";
const char *password = "Tiuk34562";
const String space = " ";
WiFiServer server(80);
WiFiClient client, client2, client3;
int numBots = 0;
std::queue<int> queue;

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

  M5.Lcd.println("Server started!");

}

bool once = true;

void loop() {

  if(once) {    
    while(numBots < 3) {
      client = server.available();

      if(client) {
        if(client.readStringUntil('\r').substring(2) == "HI") {
          delay(3000);
          numBots++;
          client.println(numBots);
          Serial.println(numBots);
        }
        delay(20);
      }
    }
    delay(500);
    once = false;
    M5.Lcd.println("All clients connected");
  }

  if(!client) client = server.available();
  if(!client2) client2 = server.available();
  if(!client3) client3 = server.available();  

  if (client) {
    Serial.println("Handling Client 1");
    String l = client.readStringUntil('\r');
    //parse message in the form "ID MESSAGE"
    int id = l.substring(0,1).toInt();
    String message = l.substring(2);
    handleMessage(id, message);
  }
  if(client2) {
    Serial.println("Handling Client 2");
    String l = client2.readStringUntil('\r');
    int id = l.substring(0,1).toInt();
    String message = l.substring(2);
    handleMessage(id, message);
  }
  if(client3) {
    Serial.println("Handling Client 3");
    String l = client3.readStringUntil('\r');
    int id = l.substring(0,1).toInt();
    String message = l.substring(2);
    handleMessage(id, message);
  }

  //CHANGE TO 3 LATER
  if(queue.size() >= 3) executeIntersection();

  delay(10);
}

void handleMessage(int id, String message) {
  Serial.println(id + space + message);

  if(message == "ARRIVED") {
    queue.push(id);
  }
}

void broadcastMessage(int id, String message) {
  delay(1000);
  Serial.printf("%d %d %d\n", client ? 1 : 0, client2 ? 1 : 0, client3 ? 1 : 0);
  Serial.println(id + space + message);
  
  client.println(id + space + message);
  client2.println(id + space + message);
  client3.println(id + space + message);
}

void executeIntersection() {
  M5.Lcd.println("executing intersection...");
  while(!queue.empty()) {
    int id = queue.front();
    broadcastMessage(id, "GO");

    queue.pop();
    M5.Lcd.printf("Sending ID %d...\n", id);
    int receivedId = 0;
    do {
      if(client) {
        String l = client.readStringUntil('\r');
        int num = l.substring(0,1).toInt();
        if(num) receivedId = num;
      } if(client2) {
        String l = client2.readStringUntil('\r');
        int num = l.substring(0,1).toInt();
        if(num) receivedId = num;
      }
      if(client3) {
        String l = client3.readStringUntil('\r');
        int num = l.substring(0,1).toInt();
        if(num) receivedId = num;
      }
      delay(50);
    } while(receivedId != id);

    M5.Lcd.printf("Id %d completed intersection...sending next bot\n", id);
    delay(1000);
  }
}
