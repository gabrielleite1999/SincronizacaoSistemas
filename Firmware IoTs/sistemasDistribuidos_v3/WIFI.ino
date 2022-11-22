
String rota_send  = "http://192.168.137.1:8092/serverSend/";
String rota_Time  = "http://worldtimeapi.org/api/timezone/America/Sao_Paulo";
String rota_LocalTime  = "http://192.168.137.1:8092/getTime";

String macAdress  = "";

extern byte maxReadPerFile;


int setRTC (void)
{
  byte flag = 1;
  
  String payload = "";
  unsigned int httpResponseCode = 0;
  
  //Check WiFi connection status
  if(WiFi.status() == WL_CONNECTED)
  {  
    while (flag)
    {  
      Serial.println ("HTTP GET");    
      HTTPClient http;   
      http.begin(rota_Time); 
           
      // Send HTTP GET request
      httpResponseCode = http.GET();
        
      if (httpResponseCode == 200) 
      {
        Serial.print    ("HTTP Response code: ");
        Serial.println  (httpResponseCode);
          
        payload = http.getString();
        Serial.println (payload);      
  
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, payload);
     
      
        String dateTime = doc["datetime"];   
        Serial.println (dateTime);  

  
        byte delimiter[5];
    
        //"2020-12-16T21:30:10"
        //"2020-12-16 21:30:10"
        
        delimiter[0] = dateTime.indexOf("-");
        delimiter[1] = dateTime.indexOf("-", delimiter[0] + 1);
        delimiter[2] = dateTime.indexOf("T", delimiter[1] + 1);
        delimiter[3] = dateTime.indexOf(":", delimiter[2] + 1);
        delimiter[4] = dateTime.indexOf(":", delimiter[3] + 1);
      
        unsigned int ano     = (dateTime.substring(0, delimiter[0])).toInt();
        unsigned int mes     = (dateTime.substring(delimiter[0] + 1, delimiter[1])).toInt();
        unsigned int dia     = (dateTime.substring(delimiter[1] + 1, delimiter[2])).toInt();
        unsigned int hora    = (dateTime.substring(delimiter[2] + 1, delimiter[3])).toInt();
        unsigned int minuto  = (dateTime.substring(delimiter[3] + 1, delimiter[4])).toInt();
        unsigned int segundo = (dateTime.substring(delimiter[4] + 1)).toInt();
        
    
        Serial.print ("Ajuste RTC: ");
        Serial.print (hora);
        Serial.print (":");
        Serial.print (minuto);
        Serial.print (":");
        Serial.print (segundo);
        Serial.print (" ");
        Serial.print (dia);
        Serial.print ("/");
        Serial.print (mes);
        Serial.print ("/");
        Serial.print (ano);
  
        rtc.setTime(segundo, minuto, hora, dia, mes, ano); 
        delay (10);

        flag = 0;
      }
      else 
      {
        Serial.print    ("Error code: ");
        Serial.println  (httpResponseCode);
        delay (1000);
      }
      // Free resources
      http.end();
    }
  }
  Serial.println ();

  return (httpResponseCode);
}

int setRTC1 (void)
{
  byte flag = 1;
  
  String payload = "";
  unsigned int httpResponseCode = 0;
  
  //Check WiFi connection status
  if(WiFi.status() == WL_CONNECTED)
  {  
    while (flag)
    {  
      Serial.println ("HTTP GET");    
      HTTPClient http;   
      http.begin(rota_LocalTime); 
           
      // Send HTTP GET request
      httpResponseCode = http.GET();
        
      if (httpResponseCode == 200) 
      {
        Serial.print    ("HTTP Response code: ");
        Serial.println  (httpResponseCode);
          
        payload = http.getString();
        Serial.println (payload);      
  
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, payload);
    
        unsigned int ano     = doc["year"];
        unsigned int mes     = doc["month"];
        unsigned int dia     = doc["day"];
        unsigned int hora    = doc["hour"];
        unsigned int minuto  = doc["minute"];
        unsigned int segundo = doc["second"];
            
        Serial.print ("Ajuste RTC Localmente: ");
        Serial.print (hora);
        Serial.print (":");
        Serial.print (minuto);
        Serial.print (":");
        Serial.print (segundo);
        Serial.print (" ");
        Serial.print (dia);
        Serial.print ("/");
        Serial.print (mes);
        Serial.print ("/");
        Serial.print (ano);
  
        rtc.setTime(segundo, minuto, hora, dia, mes, ano); 
        delay (10);

        flag = 0;
      }
      else 
      {
        Serial.print    ("Error code: ");
        Serial.println  (httpResponseCode);
        delay (1000);
      }
      // Free resources
      http.end();
    }
  }
  Serial.println ();

  return (httpResponseCode);
}




unsigned int httpPost (String dataToSend)
{
  int httpResponseCode = 0; 

  float temp = hdc1080.readTemperature();
  
  dataToSend = "{\"temperature\":" + String (temp) + ",\"timestamp\": " + String (rtc.getEpoch()+10800) + "000}";

  macAdress = WiFi.macAddress();
  
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println ();      
    Serial.println ("HTTP POST");   

    Serial.println (dataToSend);

    WiFiClient Client;    
    HTTPClient http;
      
    http.begin(Client, rota_send + macAdress);
    http.setTimeout(2000);
    http.addHeader("Content-Type", "application/json");
//    http.addHeader("auth_token", "DWKpRyxZh9C79VSj");
    
    httpResponseCode = http.POST(dataToSend);
    String response = http.getString();
    
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    Serial.println("Response: " + response);
        
    // Free resources
    http.end();
  }  

  if (httpResponseCode != 200)
  {
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    delay (500);
  }
 
  return (httpResponseCode);
}
