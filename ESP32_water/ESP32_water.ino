#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "home_nantang";
const char* password = "0622780667";
const char* serverName = "http://49.0.124.54:1234/water_monitors";
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
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

      JSONVar waterMonitor;
      waterMonitor["mac_address"] = "00:11:22:33:44:55";
      waterMonitor["electric_current"] = 3.14159265;
      waterMonitor["water_temperature"] = 24.25;
      waterMonitor["water_flow"] = true;

      String jsonString = JSON.stringify(waterMonitor);

      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsonString);
      
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      //http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
