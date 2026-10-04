#include <Stepper.h>

// ---------------- Wiring ----------------
//   IR receiver: S -> GPIO25, + -> ESP32 3V3, - -> GND
//   ULN2003:     IN1 -> GPIO16, IN2 -> GPIO17, IN3 -> GPIO18, IN4 -> GPIO19
//                + / - -> power supply module 5 V rail / GND rail
//   ESP32 GND -> GND rail (common ground)

const int IR_PIN = 25;

const int IN1 = 16;
const int IN2 = 17;
const int IN3 = 18;
const int IN4 = 19;

// ---------------- Stepper parameters ----------------
// Steps for one turn of the output shaft in full-step mode:
// 32 steps per motor revolution x ~64:1 gearbox = 2048
// (the real ratio is 63.68:1, so 2048 is ~0.5% more than one exact turn)
const int stepsPerRevolution = 2048;

// Output shaft speed in rpm. Above ~15 rpm the motor tends to skip steps
// or just vibrate, because the coils can't build up current fast enough.
const int rpm = 10;

// Time between two steps at this speed:
// 60 s / (2048 steps * 10 rpm) = ~2929 us
const unsigned long STEP_INTERVAL_US =
    60UL * 1000000UL / ((unsigned long)stepsPerRevolution * rpm);

// The Stepper library energizes pins in the order: pin1, pin2, pin3, pin4.
// The 28BYJ-48 coil order requires IN1, IN3, IN2, IN4 to get the correct
// sequence; with IN1-IN2-IN3-IN4 the motor vibrates instead of turning.
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

// ---------------- Remote codes ----------------
const unsigned long CODE_VOL_UP   = 0xFF629D;
const unsigned long CODE_VOL_DOWN = 0xFFA857;

// While a button is held, the remote sends a repeat frame every ~108 ms.
// If no frame arrives for this long, the button is considered released.
const unsigned long RELEASE_TIMEOUT_MS = 200;

// ---------------- State ----------------
bool isRunning = false;         // true while a direction button is held
int direction = 1;              // +1 = clockwise, -1 = counterclockwise
unsigned long lastCmdTime = 0;  // millis() of the last code or repeat frame
unsigned long lastStepTime = 0; // micros() of the last motor step

// Result of one attempt to decode an NEC frame
enum IrResult { IR_NONE, IR_CODE, IR_REPEAT };

// Measures how long the IR pin stays at 'level'.
// Returns the duration in us, or 0 if it exceeds 'timeout'.
unsigned long readPulse(int level, unsigned long timeout = 20000) {
  unsigned long start = micros();
  while (digitalRead(IR_PIN) == level) {
    if (micros() - start > timeout) return 0;
  }
  return micros() - start;
}

// Decodes one NEC frame. Call it when the IR pin has just gone LOW.
// The receiver output is inverted: LOW = IR burst received, HIGH = silence.
//
// Full frame:   9 ms LOW, 4.5 ms HIGH, then 32 bits
//               (each bit: 560 us LOW + 560 us HIGH = 0, or 1680 us HIGH = 1)
// Repeat frame: 9 ms LOW, 2.25 ms HIGH, 560 us LOW, no data
//               (sent every ~108 ms while the button is held)
IrResult readNEC(unsigned long &code) {
  // Leader: 9 ms LOW (accepted 8-10 ms)
  unsigned long lowTime = readPulse(LOW);
  if (lowTime < 8000 || lowTime > 10000) return IR_NONE;

  // The HIGH after the leader tells repeat frames from full frames
  unsigned long highTime = readPulse(HIGH);
  if (highTime >= 1800 && highTime <= 2700) return IR_REPEAT;  // ~2.25 ms
  if (highTime < 4000 || highTime > 5000) return IR_NONE;      // ~4.5 ms

  // 32 data bits, shifted in MSB-first
  // (this gives the same codes as the remote test sketch, e.g. 0xFF629D)
  code = 0;
  for (int i = 0; i < 32; i++) {
    unsigned long bitLow = readPulse(LOW);       // ~560 us
    if (bitLow < 400 || bitLow > 700) return IR_NONE;

    unsigned long bitHigh = readPulse(HIGH);     // ~560 us = 0, ~1680 us = 1
    if (bitHigh == 0) return IR_NONE;

    code <<= 1;
    if (bitHigh > 1000) code |= 1;
  }
  return IR_CODE;
}

// Switches all coils off: no current, no heat, no holding torque.
void releaseCoils() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void setup() {
  pinMode(IR_PIN, INPUT);  // the receiver module drives the line, no pull-up needed
  // The Stepper constructor already set IN1-IN4 as outputs
  releaseCoils();
  Serial.begin(9600);
  Serial.println("Hold VOL+ = clockwise, hold VOL- = counterclockwise");
}

void loop() {
  // ---- 1. IR input ----
  // The pin is polled every loop iteration. Since stepping never blocks
  // (see section 2), the start of the 9 ms leader is caught in time.
  if (digitalRead(IR_PIN) == LOW) {
    unsigned long code;
    IrResult result = readNEC(code);

    if (result == IR_CODE) {
      Serial.print("Code: 0x");
      Serial.println(code, HEX);

      if (code == CODE_VOL_UP) {
        direction = 1;
        isRunning = true;
        lastCmdTime = millis();
        Serial.println("Clockwise");
      } else if (code == CODE_VOL_DOWN) {
        direction = -1;
        isRunning = true;
        lastCmdTime = millis();
        Serial.println("Counterclockwise");
      }
    } else if (result == IR_REPEAT && isRunning) {
      // Button still held: keep running in the same direction
      lastCmdTime = millis();
    }
  }

  // ---- 2. Motor ----
  if (isRunning) {
    if (millis() - lastCmdTime > RELEASE_TIMEOUT_MS) {
      // No frame for 200 ms: button released
      isRunning = false;
      releaseCoils();
      Serial.println("Stopped");
    } else if (micros() - lastStepTime >= STEP_INTERVAL_US) {
      // One step only when it's due. step(1) then returns immediately,
      // instead of busy-waiting ~2.9 ms inside the library, so the loop
      // keeps polling the IR pin between steps.
      lastStepTime = micros();
      myStepper.step(direction);
    }
  }
}