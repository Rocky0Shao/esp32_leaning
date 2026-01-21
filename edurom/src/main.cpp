#include <Arduino.h>
#include <WiFi.h>
#include "esp_wpa2.h"
#include "secrets.h" // Import your credentials

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.print("Connecting to network: ");
  Serial.println(EAP_SSID);

  WiFi.disconnect(true); 
  WiFi.mode(WIFI_STA);
  
  // 1. Load the Certificate from secrets.h
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
    
  } else {
    Serial.println("\nFailed to connect.");
  }
}

void loop() {
  // Your code here
}