#include <WiFi.h>
#include "esp_wpa2.h"
#include "secrets.h" // Import the separate file

// Use credentials from secrets.h
const char* ssid = EAP_SSID;
const char* eap_identity = EAP_IDENTITY;
const char* eap_password = EAP_PASSWORD;

// Counter for the success message
int successCount = 0; 

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.print("Connecting to network: ");
  Serial.println(ssid);

  WiFi.disconnect(true); 
  WiFi.mode(WIFI_STA); 

  // Configure WPA2 Enterprise
  // We cast to (uint8_t *) because the library expects byte arrays
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)eap_identity, strlen(eap_identity));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)eap_identity, strlen(eap_identity));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)eap_password, strlen(eap_password));
  
  // Enable WPA2 Enterprise
  esp_wifi_sta_wpa2_ent_enable();

  WiFi.begin(ssid);

  // Wait for connection
  int setupCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    setupCounter++;
    if(setupCounter >= 60){ // Timeout after 30s (60 * 500ms)
       Serial.println("\nConnection Failed! Rebooting...");
       ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if we are still connected
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("success -");
    Serial.println(successCount);
    
    successCount++; // Increment the counter
  } else {
    Serial.println("Connection lost...");
    // If you want it to try reconnecting, you usually need to restart the cycle
    // or just reboot: ESP.restart();
  }
  
  delay(1000); // Wait for 1 second
}