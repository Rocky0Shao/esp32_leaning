#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void scanI2C() {
  Serial.println("Scanning I2C bus...");
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("Scan complete.");
}

void setup() {
  Serial.begin(115200);
  delay(3000);  // Wait 3 seconds for serial monitor to connect
  Serial.println();
  Serial.println("--- Starting LCD Setup ---");

  Wire.begin(21, 22);
  Wire.setClock(100000);

  scanI2C();  // Show all I2C devices

  lcd.init();
  delay(100);
  lcd.backlight();
  delay(100);

  lcd.setCursor(0, 0);
  lcd.print("Hello World");
  lcd.setCursor(0, 1);
  lcd.print("Line 2 Test");
  Serial.println("LCD Init Complete!");
}

void loop() {
}
