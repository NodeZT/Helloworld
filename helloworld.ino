#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const int RELAY_PIN = 13; //RELAY
 
const char* ssid = "owo";
const char* password = "helloworld";
MDNSResponder mdns;
ESP8266WebServer server(80);

void index() {
  server.send(200, "text/plain", "Hello World");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0;i<server.args();i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/html", message);
}
 
void setup(void){
  pinMode(RELAY_PIN, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print(".");
    digitalWrite(RELAY_PIN, 1);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(RELAY_PIN, 0);
  
  if(mdns.begin("esp8266", WiFi.localIP())){
    Serial.println("MDNS responder started");
  }

  server.on("/", index);
  
  server.on("/led", [](){
    String state=server.arg("state");
    if (state == "on")digitalWrite(RELAY_PIN, 1);
    else if (state == "off")digitalWrite(RELAY_PIN, 0);
    else if(state == "flash"){      
      for(int i=0;i<10;i++){
        digitalWrite(RELAY_PIN, 1);
        delay(100);
        digitalWrite(RELAY_PIN, 0);
        delay(100);
      }
    }
    server.send(200, "text/plain", "LED is now " + state);
  });

  server.on("/pinState", [](){
    String pin=server.arg("pin");
    //if (pin != "")int RELAY_PIN = pin;
    server.send(200, "text/plain", "pin = " + pin);
  });
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
