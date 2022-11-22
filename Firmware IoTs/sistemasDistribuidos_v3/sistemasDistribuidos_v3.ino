
#include <Wire.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ESP32Time.h>
#include "ClosedCube_HDC1080.h"

ESP32Time rtc;

ClosedCube_HDC1080 hdc1080;

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "GabDEV1";
const char* password = "123456789";

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
const char* PARAM_INPUT_3 = "input3";



unsigned int httpPost (String dataToSend);

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    input1: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    input2: <input type="text" name="input2">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    input3: <input type="text" name="input3">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}



void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  rtc.setTime(0); 
  Serial.println (readRTC());

  Wire.begin();
  delay (10);

  hdc1080.begin(0x40); 
//  hdc1080.readTemperature();
//  hdc1080.readHumidity();
  delay (10);

  //dataToSend = "{\"temperature\":123,\"timestamp\": " + String (rtc.getEpoch()+10800) + "000}";

  server.on("/horarioLocal", HTTP_GET, [](AsyncWebServerRequest *request){
 
    int paramsNr = request->params();
    Serial.print ("TIME REQUEST - ");
    Serial.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      Serial.print("Param name: ");
      Serial.println(p->name());
      Serial.print("Param value: ");
      Serial.println(p->value());
      Serial.println("------");
    }
     
    request->send(200, "application/json", "{\"timestamp\": " + String (rtc.getEpoch()+10800) + "000}");
  });
  

  server.on(
    "/atualizarHorario",
    HTTP_POST,
    [](AsyncWebServerRequest * request){},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

      Serial.println ("SINCRONIZANDO HORARIO");
      Serial.println (len);
      Serial.println (index);
      Serial.println (total);

      String dataToSend = "";
        
      for (size_t i = 0; i < len; i++) 
      {
        dataToSend += char (data[i]); 
      }

//      dataToSend.replace(" ", ""); // depois comentar
      dataToSend.replace("\t", "");
      dataToSend.replace("\n", "");
      dataToSend.replace("\r", "");

      Serial.println (dataToSend);
      Serial.println();

      DynamicJsonDocument doc(2048);
      deserializeJson(doc, dataToSend);
     
      unsigned int hora     = doc["hora"];   
      unsigned int minuto   = doc["minuto"];   
      unsigned int segundo  = doc["segundo"];   
      unsigned int dia      = doc["dia"];   
      unsigned int mes      = doc["mes"];   
      unsigned int ano      = doc["ano"];   
      
      Serial.println (hora);
      Serial.println (minuto);
      Serial.println (segundo);
      Serial.println (dia);
      Serial.println (mes);
      Serial.println (ano);

      rtc.setTime(segundo, minuto, hora, dia, mes, ano); 
      delay (10);

      Serial.println (readRTC());

      request->send (200, "text/plain", "OK");
  });

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() 
{
  while (httpPost ("a") != 200);
  hurryRTC();
  delay (10000);
}
