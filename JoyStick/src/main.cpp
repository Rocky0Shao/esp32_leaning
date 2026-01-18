#include <Arduino.h>
#include "joystick.h"
#include "motor.h"

const int JOY_CENTER = 2945;
const int DEADZONE = 200;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== Joystick Stepper Control ===");

  joystickInit();
  motorInit();

  Serial.println("Ready! Move joystick left/right to control motor.");
}

void loop() {
  int xValue = joystickReadX();
  int deviation = xValue - JOY_CENTER;

  if (abs(deviation) > DEADZONE) {
    bool clockwise = deviation < 0;
    motorStep(clockwise, 1);
  } else {
    motorStop();
  }
}
