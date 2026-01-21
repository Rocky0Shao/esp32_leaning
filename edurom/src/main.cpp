#include <Arduino.h>
#include <WiFi.h>
#include "esp_wpa2.h"
#include "secrets.h" 
#include <HTTPClient.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.print("Connecting to network: ");
  Serial.println(EAP_SSID);

  WiFi.disconnect(true); 
  WiFi.mode(WIFI_STA);
  
  // 1. Load the Certificate from secrets.h
  // keep this commented out as it broke the connection for me
  // esp_wifi_sta_wpa2_ent_set_ca_cert((uint8_t *)EDUROAM_CA_CERT, strlen(EDUROAM_CA_CERT));

  // 2. Load Credentials from secrets.h
  // OSU uses full email for both identity and username
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));

  // 3. Enable WPA2 Enterprise
  esp_wifi_sta_wpa2_ent_enable();

  // 4. Start Connection
  WiFi.begin(EAP_SSID);

  // 5. Wait for connection
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if(counter > 60){ // Timeout after 30 seconds
        Serial.println("\nConnection timed out! Checking error status...");
        break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nSuccessfully connected to eduroam!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength (RSSI): ");
    Serial.println(WiFi.RSSI());
    {
      Serial.println("\nTesting Internet Access...");
    HTTPClient http;
    
    // We use http (not https) to keep the test simple and avoid SSL errors for now
    http.begin("http://www.google.com"); 
    
    int httpCode = http.GET();
    
    if (httpCode > 0) {
        Serial.printf("Server Responded! Status Code: %d\n", httpCode);
        // Code 200 = OK. Code 301/302 = Redirect (also means internet works).
        if (httpCode == 200 || httpCode == 301 || httpCode == 302) {
            Serial.println(">>> SUCCESS: Your ESP32 has access to the outside internet! <<<");
        }
    } else {
        Serial.printf("Request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    // -----------------------------------
    }
    
  } else {
    Serial.println("\nFailed to connect.");
  }
}

void loop() {
  // Your code here
}