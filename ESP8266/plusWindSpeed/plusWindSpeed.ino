#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h> //biblioteka od czujnika ciśnienia
#include <LM75A.h>            //biblioteka od czujnika temp
#include <APDS9930.h>        //biblioteka od czujnika światła
#include <Adafruit_VEML6075.h>  //biblioteka od czujnika światła UV
#include <Adafruit_HTU21DF.h>   //biblioteka od czujnika wilgotności

//Ticker ticker;
WiFiManager wifiManager;



int timeout = 10;     //czas oczekiwania na ponowne połączenie z WiFi liczony w sekundach
String serverIP = "192.168.1.111";    //adres serwera na którym znajduje się api
String serwerPort = "8088";          // port na którym znajduje się api


// zmienne gdzie przetrzymywana jest wartość czy czujnik jest podłączony
bool compassSensor, windSpeedSensor, windDirectionSensor, humiditySensor,
     lightSensor, temperatureSensor, UVlightSensor, pressureSensor;
// zmienne z wartościami pobranymi z czujników
float pressure, temperature, light, humidity, UVlight, windDirection;
int windSpeed;
//  zmienna określająca czas głębokiego snu
int deepSleepTime = 5000000;


void setup()
{
  Wire.begin();
  Serial.begin(9600);

  updateData();
  wifiConnection();
  sendJson();
  deepSleepTime = checkDeepSleepTime(deepSleepTime);
  ESP.deepSleep(deepSleepTime);

}

void loop()
{
}

void sendJson()
{
  String macAddresse = "ESP8266-";
  unsigned char mac[6];
  WiFi.macAddress(mac);
  macAddresse += macToStr(mac);
  if (windSpeedSensor)
  {
    StaticJsonBuffer<300> JSONbuffer;   //Deklaracja statycznego JSON buffer dla prędkości wiatru
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["PhysicalShieldId"] = macAddresse;
    JSONencoder["PhysicalSensorId"] = "windSpeedSensor";
    JSONencoder["Value"] = windSpeed;

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    httpRequest(JSONmessageBuffer);
  }
  if (windDirectionSensor)
  {
    StaticJsonBuffer<300> JSONbuffer;   //Deklaracja statycznego JSON buffer dla kierunku wiatru
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["PhysicalShieldId"] = macAddresse;
    JSONencoder["PhysicalSensorId"] = "windDirectionSensor";
    JSONencoder["Value"] = windDirection;
    //...
    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    httpRequest(JSONmessageBuffer);
  }
  if (humiditySensor)
  {
    StaticJsonBuffer<300> JSONbuffer;   //Deklaracja statycznego JSON buffer dla czujnika wilgotności
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["PhysicalShieldId"] = macAddresse;
    JSONencoder["PhysicalSensorId"] = "humiditySensor";
    JSONencoder["Value"] = humidity;

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    httpRequest(JSONmessageBuffer);
  }
  if (lightSensor)
  {
    StaticJsonBuffer<300> JSONbuffer;   //Deklaracja statycznego JSON buffer dla czunika swiatła
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["PhysicalShieldId"] = macAddresse;
    JSONencoder["PhysicalSensorId"] = "lightSensor";
    JSONencoder["Value"] = light;

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    httpRequest(JSONmessageBuffer);
  }
  if (temperatureSensor)
  {
    StaticJsonBuffer<300> JSONbuffer;   //Deklaracja statycznego JSON buffer dla czujnika temperatury
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["PhysicalShieldId"] = macAddresse;
    JSONencoder["PhysicalSensorId"] = "temperatureSensor";
    JSONencoder["Value"] = temperature;

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    httpRequest(JSONmessageBuffer);
  }
  if (UVlightSensor)
  {
    StaticJsonBuffer<300> JSONbuffer;   //Deklaracja statycznego JSON buffer dla
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["PhysicalShieldId"] = macAddresse;
    JSONencoder["PhysicalSensorId"] = "UVlightSensor";
    JSONencoder["Value"] = UVlight;

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    httpRequest(JSONmessageBuffer);
  }
  if (pressureSensor)
  {
    StaticJsonBuffer<300> JSONbuffer;   //Deklaracja statycznego JSON buffer dla
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["PhysicalShieldId"] = macAddresse;
    JSONencoder["PhysicalSensorId"] = "pressureSensor";
    JSONencoder["Value"] = pressure;

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    httpRequest(JSONmessageBuffer);
  }
}

void httpRequest(char JSONmessageBuffer[])
{
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;    //deklaracja obiektu klasy HTTPClient
    http.begin("http://" + serverIP + ":" + serwerPort + "/api/measure/single"); //request na adres
    http.addHeader("Content-Type", "application/json");  //Specyfika content-type header

    int httpCode = http.POST(JSONmessageBuffer);   //Wysłanie requesta
    String payload = http.getString();             //zapisanie odpowiedzi
    if (httpCode == 200) {
      Serial.println("Sukces- wysłano prawidłowo requesta");
      Serial.println(httpCode);   //drukowanie kodu zwrotnego
      StaticJsonBuffer<80> JSONbuf;
      JsonObject& root = JSONbuf.parseObject(payload);
      deepSleepTime = root["deepSleep"];
      Serial.println(deepSleepTime);
    } else {
      Serial.println("Porażka- coś poszło nie tak");
      if (httpCode == -1) {
        Serial.println("Brak API pod wskazanym adresem lub brak dostępnego adresu w sieci");
      }
    }
    Serial.println(httpCode);
    http.end();  //Zamknięcie połączenia
  }
}


void wifiConnection()
{

  wifiManager.setAPCallback(configModeCallback);                //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  Serial.println("Próba nawiązania połączenia z siecią.");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    timeout--;
    delay(1000);
    if (timeout == 0) {
      Serial.println("Rozłączono-ustawiam AccessPoint");
      wifiManager.setAPCallback(configModeCallback);
      wifiManager.autoConnect();
    }
  }
  Serial.println("Połączono");
}

void updateData()
{
  isDeviceConnected();          //metoda sprawdzająca dostępne czujniki
  setWindDirection();           //metoda pobierająca dane z czujnika kierunku wiatru
  setWindSpeed();               //metoda pobierająca dane z czujnika prędkości wiatru
  setHumidity();                //metoda pobierająca dane z czujnika wilgotności
  setLight();                   //metoda pobierająca dane z czujnika swiatła
  setTemperature();             //metoda pobierająca dane z czujnika temperatury
  setUVlight();                 //metoda pobierająca dane z czujnika swiatla uv
  setPressure();                //metoda pobierająca dane z czujnika cisnienia
}

void isDeviceConnected()
{
  byte error, address;
  //ustawienie zmiennych bool na 0
  compassSensor = 0, windSpeedSensor = 0, windDirectionSensor = 0, humiditySensor = 0, lightSensor = 0, temperatureSensor = 0, UVlightSensor = 0, pressureSensor = 0;
  int devices = 0; //ilosc podpiętych urządzeń
  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      switch (address) {
        case 1:
          compassSensor = 1;
          Serial.println("znaleziono:kompas");
          break;
        case 38:
          windSpeedSensor = 1;
            Serial.println("znaleziono:windSS");
          break;
        case 100:
          windDirectionSensor = 1;
            Serial.println("znaleziono:windDS");
          break;
        case 57:
          lightSensor = 1;
            Serial.println("znaleziono:light");
          break;
        case 72:
          temperatureSensor = 1;
            Serial.println("znaleziono:temp");
          break;
        case 16:
          UVlightSensor = 1;
            Serial.println("znaleziono:UV");
          break;
        case 119:
          pressureSensor = 1;
            Serial.println("znaleziono:pressure");
          break;

      }
      devices++;

    }
    else if (error == 4) //kiedy nie rozpoznaje czujnika lub są problemy z komunikacją a adresie
    {
      Serial.print("Nieznany błąd na adresie: 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  Adafruit_HTU21DF htu = Adafruit_HTU21DF();
  if (htu.begin()) {
    humiditySensor = 1;
      Serial.println("znaleziono:hum");
    devices++;
  }
  if (devices == 0)
  {
    Serial.print("Brak podłączonych czujników na magistrali I2C");
  }else{
    Serial.print("znaleziono:");
    Serial.println(devices);
  }
  String  sensors = compassSensor ? "1" : "0";              //potrzebne?
  sensors += windSpeedSensor ? "1" : "0";
  sensors += windDirectionSensor ? "1" : "0";
  sensors += humiditySensor ? "1" : "0";
  sensors += lightSensor ? "1" : "0";
  sensors += temperatureSensor ? "1" : "0";
  sensors += UVlightSensor ? "1" : "0";
  sensors += pressureSensor ? "1" : "0" ;


  Serial.println(sensors);

}

void setWindDirection()
{

  if (windDirectionSensor) {

    int bigNum;

    byte a, b, c, d;

    Wire.requestFrom(100, 4);

    a = Wire.read();
    b = Wire.read();
    c = Wire.read();
    d = Wire.read();

    bigNum = a;
    bigNum = (bigNum << 8) | b;
    bigNum = (bigNum << 8) | c;
    bigNum = (bigNum << 8) | d;
    Serial.print("kier wiatru:");
    windDirection = bigNum * 0.01;
    Serial.print(windDirection);
    Serial.println(" stop");
    Serial.print(bigNum);

  }
}

void setWindSpeed()
{
  if (windSpeedSensor) {

    int bigNum;

    byte a, b, c, d;

    Wire.requestFrom(38, 4);

    a = Wire.read();
    b = Wire.read();
    c = Wire.read();
    d = Wire.read();

    bigNum = a;
    bigNum = (bigNum << 8) | b;
    bigNum = (bigNum << 8) | c;
    bigNum = (bigNum << 8) | d;
    Serial.print("prędkość wiatru:");
    windSpeed = bigNum;
    Serial.print(windSpeed);
    Serial.println(" RPM");

  }
}
void setHumidity()
{

  if (humiditySensor) {
    Adafruit_HTU21DF htu = Adafruit_HTU21DF();
    htu.begin();
    humidity = htu.readHumidity();
  }


}
void setLight()
{
  if (lightSensor) {
    APDS9930 apds = APDS9930();
    float ambient_light = 0;
    uint16_t ch0 = 0;
    uint16_t ch1 = 1;
    if ( !apds.init() ) {
      Serial.println(F("problem z inicjalizacją"));
    }
    if ( !apds.enableLightSensor(false) ) {
      Serial.println(F("problem właczeniem sensora"));
    }
#ifdef DUMP_REGS
    /* Register dump */
    uint8_t reg;
    uint8_t val;

    for (reg = 0x00; reg <= 0x19; reg++) {
      if ( (reg != 0x10) && \
           (reg != 0x11) )
      {
        apds.wireReadDataByte(reg, val);
      }
    }
    apds.wireReadDataByte(0x1E, val);
#endif

    // czekanie na koniec ustawień
    delay(500);
    if (  !apds.readAmbientLightLux(ambient_light) ||
          !apds.readCh0Light(ch0) ||
          !apds.readCh1Light(ch1) ) {
      Serial.println(F("błąd przy czytaniu wartości"));
    } else {
      Serial.print(F("Światło: "));
      Serial.println(ambient_light);
      light = ambient_light;  //przypisanie wartości do zmiennej globalnej
    }

  }

}


void setTemperature()
{
  if (temperatureSensor) {
    LM75A lm75a_sensor(false, false, false);

    temperature = lm75a_sensor.getTemperatureInDegrees();
    while (temperature == INVALID_LM75A_TEMPERATURE)
    {
      temperature = lm75a_sensor.getTemperatureInDegrees();   //przypisanie do zmienne globalnej
    }
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" Stopni Celcjusza");
  }
}
void setUVlight()
{
  if (UVlightSensor) {
    Adafruit_VEML6075 uv = Adafruit_VEML6075();
    uv.begin();
    Serial.print("Indeks UV: ");
    Serial.print(float(uv.readUVI()));
    Serial.print("   ");
    Serial.println(float(UVlight));
    UVlight = uv.readUVI();
  }
}
void setPressure()
{
  if (pressureSensor) {
    Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
    bmp.begin();
    sensors_event_t event;
    bmp.getEvent(&event);

    /* wyswietlanie wyniku cisnienia w hPa) */
    if (event.pressure)
    {
      /* wyswietlanie cisnienia atmosferycznego w hPa */
      Serial.print("Ciśnienie:    ");
      Serial.print(event.pressure); // zwraca ciśnienie
      pressure = event.pressure;
      Serial.println(" hPa");
    }
    else
    {
      Serial.println("Błąd sensora");
    }
  }
}
//przejscie urządzenia w AP
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Wejście w stan konfiguracji");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}
String macToStr(const uint8_t* mac) // przestawienie adresu mac w postaci szesnastkowej w formacie 11:22:33:44:55:66
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}
int checkDeepSleepTime(int dsTime) {
  if (dsTime > 429496729)
  {
    return 429496729;
  } //4 29496729us =~71 min
  if (dsTime < 0)
  {
    return 0;
  }
  return dsTime;
}


