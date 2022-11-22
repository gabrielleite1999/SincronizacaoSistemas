
String readRTC (void)
{
  //"2020-12-16T21:30:10"
  //"2020-12-16 21:30:10"
  String i = "";

  unsigned int ano      = rtc.getYear();
  unsigned int mes      = rtc.getMonth()+1;
  unsigned int dia      = rtc.getDay();
  unsigned int hora     = rtc.getHour(true);
  unsigned int minuto   = rtc.getMinute();
  unsigned int segundo  = rtc.getSecond();

  if (ano < 10) i += "0";
  i += String (ano);
  
  i += "-";

  if (mes < 10) i += "0";
  i += String (mes);

  i += "-";

  if (dia < 10) i += "0";
  i += String (dia);

  i += "T";

  if (hora < 10) i += "0";
  i += String (hora);
  
  i += ":";

  if (minuto < 10) i += "0";
  i += String (minuto);
  
  i += ":";

  if (segundo < 10) i += "0";
  i += String (segundo);
  
  return (i);
}


void hurryRTC (void)
{
  Serial.println (rtc.getEpoch());
  
  String i = "";

//  unsigned int ano      = rtc.getYear();
//  unsigned int mes      = rtc.getMonth()+1;
//  unsigned int dia      = rtc.getDay();
//  unsigned int hora     = rtc.getHour(true);
//  unsigned int minuto   = rtc.getMinute();
//  unsigned int segundo  = rtc.getSecond();

  unsigned long currentTime = rtc.getEpoch();

//  for (byte i = 0; i < 2; ++i)
//  {
//    ++segundo;
//    if (segundo > 59) segundo = 0;
//  }
  
  int randNumber = random(4);  
//  Serial.println ("RAND - " + String (randNumber));
  switch (randNumber)
  {
    case (0): 
                Serial.println ("Adianto 2s");
                currentTime += 2;                 
    break;

    case (1): 
                Serial.println ("Adianto 1s");
                currentTime += 1;                  
    break;

    case (2): 
                Serial.println ("Adianto 1s");
                currentTime += 1;                
    break;

    case (3): 
                Serial.println ("Adianto 2s");
                currentTime += 2;                
    break;
  }

  rtc.setTime(currentTime, 0); 
//  rtc.setTime(segundo, minuto, hora, dia, mes, ano); 
  delay (10);

  Serial.println (rtc.getEpoch());
}
