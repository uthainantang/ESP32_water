#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <Preferences.h>

Preferences preferences;
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* serverName = "http://49.0.124.54:1234/water_monitors";
unsigned long lastTime = 0;
unsigned long timerDelay = 60000;

#define SMART_CONFIG 13
#define LED_CONNECT 2

static int wifiReconnect = 0;

void connectWiFi(){
  wifiReconnect = 0;
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
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

      // Specify content-type header
      //http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      //String httpRequestData = "mac_address=00:11:22:33:44:55&electric_current=3.14159265&water_temperature=25.12&water_flow=true";
      // Send HTTP POST request
      //int httpResponseCode = http.POST(httpRequestData);

      JSONVar waterMonitor;
      waterMonitor["mac_address"] = "00:11:22:33:44:55";
      waterMonitor["electric_current"] = 3.14159265;
      waterMonitor["water_temperature"] = 24.25;
      waterMonitor["water_flow"] = waterFlow;

      String jsonString = JSON.stringify(waterMonitor);

      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // int httpResponseCode = http.POST("{\"mac_address\":\"00:11:22:33:44:55\",\"electric_current\":\"3.14159265\",\"water_temperature\":\"24.25\",\"water_flow\":\"true\"}");
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
