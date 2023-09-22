#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
<<<<<<< HEAD
#include <PZEM004Tv30.h>
#include "DHT.h"

#define DHTPIN 13 
#define water_level_pin 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#if defined(ESP32)
PZEM004Tv30 pzem(Serial2, 16, 17);
#else
PZEM004Tv30 pzem(Serial2);
#endif 

const char* ssid = "home_nantang";
const char* password = "0622780667";
=======
#include <Preferences.h>

Preferences preferences;
const char* ssid = "SSID";
const char* password = "PASSWORD";
>>>>>>> f32ffdf1ff391a4dd231c29ce1b1475be4df6944
const char* serverName = "http://49.0.124.54:1234/water_monitors";
unsigned long lastTime = 0;
unsigned long timerDelay = 60000;

<<<<<<< HEAD
float current_pzem;
float temp;
int water_level =0;
void setup() {
  Serial.begin(115200);
  dht.begin();
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
=======
#define SMART_CONFIG 13
#define LED_CONNECT 2

static int wifiReconnect = 0;

void connectWiFi(){
  wifiReconnect = 0;
>>>>>>> f32ffdf1ff391a4dd231c29ce1b1475be4df6944
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_CONNECT, 0);
    delay(500);
    Serial.print(".");
    wifiReconnect++;
    if (wifiReconnect >= 30) {
      ESP.restart();
    }
  }
  digitalWrite(LED_CONNECT, 1);
}

bool waterFlow = false;

void setup() {
  preferences.begin("Setting", false);
  String storedSSID = preferences.getString("ssid", "");
  String storedPassword = preferences.getString("password", "");

  delay(500);
  Serial.begin(115200);
  pinMode(LED_CONNECT, OUTPUT);
  pinMode(SMART_CONFIG, INPUT_PULLUP);

  digitalWrite(LED_CONNECT, 0);

  if (!digitalRead(SMART_CONFIG)) {
    Serial.println("Entering for SmartConfig.");
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();

    //Wait for SmartConfig packet from mobile
    Serial.println("Waiting for SmartConfig.");
    while (!WiFi.smartConfigDone()) {
      delay(500);
      Serial.print(".");
    }

    digitalWrite(LED_CONNECT, 1);

    Serial.println("");
    Serial.println("SmartConfig received.");

    // WiFi.begin(WiFi.SSID().c_str(), WiFi.psk().c_str());

    preferences.putString("ssid", WiFi.SSID().c_str());
    preferences.putString("password", WiFi.psk().c_str());
    Serial.println("Stored new WiFi credentials in preferences");
  } else {
    if (storedSSID.length() > 0 && storedPassword.length() > 0) {
      WiFi.begin(storedSSID.c_str(), storedPassword.c_str());
    }
  }

  connectWiFi();

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  String mac = WiFi.macAddress();
  Serial.print("MAC Address: ");
  Serial.println(mac);
}

void loop() {

  current_pzem = pzem.current();

  temp = dht.readTemperature();
  water_level = analogRead(water_level_pin);
  Serial.print("current_pzem-004T =");
  Serial.println(current_pzem);
  Serial.print("temp =");
  Serial.println(temp);
  Serial.print("water_level =");
  Serial.println(water_level);
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      JSONVar waterMonitor;
      waterMonitor["mac_address"] = "00:11:22:33:44:55";
<<<<<<< HEAD
      waterMonitor["electric_current"] = current_pzem;
      waterMonitor["water_temperature"] = temp;
      waterMonitor["water_flow"] = true;
=======
      waterMonitor["electric_current"] = 3.14159265;
      waterMonitor["water_temperature"] = 24.25;
      waterMonitor["water_flow"] = waterFlow;
>>>>>>> f32ffdf1ff391a4dd231c29ce1b1475be4df6944

      String jsonString = JSON.stringify(waterMonitor);

      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsonString);
      
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      http.end();
      
      waterFlow = !waterFlow;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
