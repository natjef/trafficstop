#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <M5Stack.h> 
#include <queue>

const char *ssid = "trafficbot";
const char *password = "Tiuk34562";
const String space = " ";
WiFiServer server(80);
WiFiClient client;
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

  M5.Lcd.println("Server started! Check serial monitor for debug.");


  //get everyone connected and assign ids
//  WiFiClient client;
//  WiFiClient client2;
//  WiFiClient client3;
//  
//  do {
//    Serial.println("waiting for all clients to connect");
//    client = server.available();
//    client2 = server.available();
//    client3 = server.available();
//    delay(20);
//  } while(!(client && client2 && client3));
//
//  client.println("1");
//  client2.println("2");
//  client3.println("3");


  //TEMP TEST FOR ONE BOT
  while(!client) {
    Serial.println("waiting for client");
    client = server.available();
    delay(100);
  }
  client.println("1");

}

void loop() {
  // get everyone connected - #TODO: MAKE THIS A LOOP + ARRAY
  client = server.available();
//  WiFiClient client2 = server.available();
//  WiFiClient client3 = server.available();
  

  if (client) {
    Serial.println("Handling Client 1");
    String l = client.readStringUntil('\r');
    //parse message in the form "ID MESSAGE"
    int id = l.substring(0,1).toInt();
    String message = l.substring(2);
    Serial.println(id);
    Serial.println(message);
    handleMessage(id, message);
  }
//  if(client2) {
//    Serial.println("Handling Client 2");
//    String l = client2.readStringUntil('\r');
//    Serial.println(l);
//    handleMessage(2, l);
//  }
//  if(client3) {
//    Serial.println("Handling Client 3");
//    String l = client3.readStringUntil('\r');
//    Serial.println(l);
//    handleMessage(3, l);
//  }

  delay(10);
}

void handleMessage(int id, String message) {
  Serial.println(id + space + message);

  if(message == "ARRIVED") {
    queue.push(id);
  }
}

void broadcastMessage(String message) {
  client = server.available();
//  client2 = server.available();
//  client3 = server.available();

  int id1 = 1, id2 = 2, id3 = 3;
  String message1 = id1 + space + message;
  String message2 = id2 + space + message;
  String message3 = id3 + space + message;

  client.println(message1);
//  client2.println(message2);
//  client3.println(message3);
}

void executeIntersection() {
  const String go = "GO";
  while(!queue.empty()) {
    int id = queue.front();
    String mes = id + space + go;
    broadcastMessage(mes);

    queue.pop();
    delay(1000);
  }
}
