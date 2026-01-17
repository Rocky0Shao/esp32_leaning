#include <Arduino.h>

// 28BYJ-48 stepper motor with ULN2003 driver
// Wiring: IN1->GPIO19, IN2->GPIO18, IN3->GPIO5, IN4->GPIO4

#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 4

// 28BYJ-48 has 2048 steps per revolution (in half-step mode)
// In full-step mode: 2048 steps = 360 degrees
const int STEPS_PER_REV = 2048;

// Delay between steps in microseconds (adjust for speed)
// Lower = faster, but too low and motor will skip steps
// 28BYJ-48 works well around 1000-2000us
int stepDelay = 1500;

// Full-step sequence (4 steps)
// More torque, but less smooth
const int fullStepSequence[4][4] = {
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};

// Half-step sequence (8 steps)
// Smoother movement, better precision
const int halfStepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

int currentStep = 0;

void setStep(int in1, int in2, int in3, int in4) {
  digitalWrite(IN1, in1);
  digitalWrite(IN2, in2);
  digitalWrite(IN3, in3);
  digitalWrite(IN4, in4);
}

void stepMotor(int steps, bool clockwise) {
  int sequenceLength = 8;  // Using half-step

  for (int i = 0; i < abs(steps); i++) {
    if (clockwise) {
      currentStep++;
      if (currentStep >= sequenceLength) currentStep = 0;
    } else {
      currentStep--;
      if (currentStep < 0) currentStep = sequenceLength - 1;
    }

    setStep(
      halfStepSequence[currentStep][0],
      halfStepSequence[currentStep][1],
      halfStepSequence[currentStep][2],
      halfStepSequence[currentStep][3]
    );

    delayMicroseconds(stepDelay);
  }

  // Turn off coils when done (saves power, reduces heat)
  setStep(0, 0, 0, 0);
}

void rotateAngle(float angle, bool clockwise) {
  // For half-step mode: 4096 steps = 360 degrees
  int steps = (int)(angle / 360.0 * 4096);
  Serial.printf("Rotating %.1f degrees (%d steps) %s\n",
                angle, steps, clockwise ? "CW" : "CCW");
  stepMotor(steps, clockwise);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== 28BYJ-48 Stepper Motor Test ===");
  Serial.println("Pins: IN1=19, IN2=18, IN3=5, IN4=4");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Make sure motor starts in off state
  setStep(0, 0, 0, 0);

  Serial.println("Motor initialized. Starting demo...");
  delay(1000);
}

void loop() {
  // Demo: rotate 360 degrees clockwise
  Serial.println("\n--- Rotating 360° clockwise ---");
  rotateAngle(360, true);
  delay(1000);

  // Demo: rotate 360 degrees counter-clockwise
  Serial.println("\n--- Rotating 360° counter-clockwise ---");
  rotateAngle(360, false);
  delay(1000);

  // Demo: small movements
  Serial.println("\n--- Small movements: 90° steps ---");
  for (int i = 0; i < 4; i++) {
    rotateAngle(90, true);
    delay(500);
  }
  delay(1000);
}