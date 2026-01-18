#include <Arduino.h>
#include "joystick.h"

// Use ADC1 pins (ADC2 conflicts with WiFi)
#define JOY_VRX 34  // X-axis analog
#define JOY_SW  32  // Button (active LOW)

void joystickInit() {
  pinMode(JOY_VRX, INPUT);
  pinMode(JOY_SW, INPUT_PULLUP);
}

int joystickReadX() {
  int xValue = analogRead(JOY_VRX);
  Serial.printf("Joystick X: %d\n", xValue);
  return xValue;
}
