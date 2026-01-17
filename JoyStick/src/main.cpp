#include <Arduino.h>

// === JOYSTICK PINS ===
// Use ADC1 pins (ADC2 conflicts with WiFi)
#define JOY_VRX 34  // X-axis analog
#define JOY_VRY 35  // Y-axis analog
#define JOY_SW  32  // Button (active LOW)

// === STEPPER MOTOR PINS ===
// 28BYJ-48 with ULN2003 driver
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 4

// Joystick center values (calibrated for this joystick)
const int JOY_CENTER = 2945;  // Measured center value
const int DEADZONE = 200;     // Ignore small movements

// Stepper speed limits (microseconds between steps)
const int MIN_DELAY = 800;    // Fastest
const int MAX_DELAY = 3000;   // Slowest

// Half-step sequence for smoother movement
// Corrected sequence for 28BYJ-48
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

// Full-step sequence - corrected pin order for 28BYJ-48
const int fullStepSequence[4][4] = {
  {1, 0, 0, 1},
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1}
};

int currentStep = 0;
bool motorEnabled = true;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setStep(int in1, int in2, int in3, int in4) {
  digitalWrite(IN1, in1);
  digitalWrite(IN2, in2);
  digitalWrite(IN3, in3);
  digitalWrite(IN4, in4);
}

// Set to true for half-step (smoother), false for full-step (more torque)
const bool USE_HALF_STEP = true;
const int SEQ_LENGTH = USE_HALF_STEP ? 8 : 4;

void stepMotor(bool clockwise) {
  if (clockwise) {
    currentStep = (currentStep + 1) % SEQ_LENGTH;
  } else {
    currentStep = (currentStep + SEQ_LENGTH - 1) % SEQ_LENGTH;
  }

  if (USE_HALF_STEP) {
    setStep(
      halfStepSequence[currentStep][0],
      halfStepSequence[currentStep][1],
      halfStepSequence[currentStep][2],
      halfStepSequence[currentStep][3]
    );
  } else {
    setStep(
      fullStepSequence[currentStep][0],
      fullStepSequence[currentStep][1],
      fullStepSequence[currentStep][2],
      fullStepSequence[currentStep][3]
    );
  }
}

void stopMotor() {
  setStep(0, 0, 0, 0);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== Joystick Stepper Control ===");
  Serial.println("X-axis: Controls direction & speed");
  Serial.println("Button: Toggle motor on/off");
  Serial.println();

  // Joystick pins
  pinMode(JOY_VRX, INPUT);
  pinMode(JOY_VRY, INPUT);
  pinMode(JOY_SW, INPUT_PULLUP);

  // Stepper pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotor();
  Serial.println("Ready! Move joystick left/right to control motor.");
}

void loop() {
  // Read joystick
  int xValue = analogRead(JOY_VRX);
  int yValue = analogRead(JOY_VRY);
  bool buttonPressed = digitalRead(JOY_SW) == LOW;

  // Button debounce and toggle
  if (buttonPressed != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonPressed && lastButtonState == HIGH) {
      motorEnabled = !motorEnabled;
      Serial.printf("Motor %s\n", motorEnabled ? "ENABLED" : "DISABLED");
      if (!motorEnabled) {
        stopMotor();
      }
    }
    lastButtonState = buttonPressed ? LOW : HIGH;
  }

  // Calculate deviation from center
  int deviation = xValue - JOY_CENTER;

  // Check if joystick is outside deadzone
  if (abs(deviation) > DEADZONE && motorEnabled) {
    bool clockwise = deviation < 0;

    // Map speed - use 1200 min delay for stability
    int absDeviation = abs(deviation);
    int stepDelay = map(absDeviation, DEADZONE, 2048, 4000, 1200);

    stepMotor(clockwise);
    delayMicroseconds(stepDelay);
  } else {
    // Only call stopMotor once to prevent coil whining
    stopMotor();
  }

  // Print debug info periodically
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    Serial.printf("X: %4d  Y: %4d  Btn: %d  Motor: %s\n",
                  xValue, yValue, !buttonPressed,
                  motorEnabled ? "ON" : "OFF");
    lastPrint = millis();
  }
}
