#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <WiFiClientSecure.h>
#include <WiFiMulti.h>

 const uint8_t ledPin =2;

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  WiFi.softAP("JadsaAP","");
  Serial.println("\nsofAP");
  Serial.println(WiFi.softAPIP());
  
  if(!SPIFFS.begin(true))

  {
    Serial.println("error Al montar SPIFFS");
    return;
  }
  
  server.on ("/", HTTP_GET,[](AsyncWebServerRequest *request)
             {request ->send(SPIFFS, "/index.html", "text/html" ); });

  server.onNotFound(notFound);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

}
 
void loop() {
  webSocket.loop();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length){
  switch (type)
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Desconectado!\n", num);
      break;

    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Conectado en %d.%d.%d.%d url: %s\n",num, ip[0], ip[1], ip[2], ip[3], payload);
        
        webSocket.sendTXT(num, "conectado en servidor:");
      }
      break;

    case WStype_TEXT:
       Serial.printf("[%u] Mensaje Recibido: %s\n", num, payload);
       String msg = String((char*)(payload));

       if (msg.equalsIgnoreCase("LedOn"))
           digitalWrite(ledPin, HIGH);

       if(msg.equalsIgnoreCase("LedOff"))    
       digitalWrite(ledPin, LOW);
  }
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404,"text/plain","¡Pagina no encontrada!");
}