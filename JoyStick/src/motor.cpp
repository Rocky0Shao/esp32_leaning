#include <Arduino.h>
#include "motor.h"

// 28BYJ-48 with ULN2003 driver
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 4

static int stepDelay = 1500;
static int currentStep = 0;

// Half-step sequence for smoother movement
static const int halfStepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

static void setStep(int in1, int in2, int in3, int in4) {
  digitalWrite(IN1, in1);
  digitalWrite(IN2, in2);
  digitalWrite(IN3, in3);
  digitalWrite(IN4, in4);
}

void motorInit() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  motorStop();
}

void motorStep(bool clockwise, int steps) {
  for (int i = 0; i < steps; i++) {
    if (clockwise) {
      currentStep++;
      if (currentStep >= 8) currentStep = 0;
    } else {
      currentStep--;
      if (currentStep < 0) currentStep = 7;
    }

    setStep(
      halfStepSequence[currentStep][0],
      halfStepSequence[currentStep][1],
      halfStepSequence[currentStep][2],
      halfStepSequence[currentStep][3]
    );

    delayMicroseconds(stepDelay);
  }

  setStep(0, 0, 0, 0);
}

void motorStop() {
  setStep(0, 0, 0, 0);
}
