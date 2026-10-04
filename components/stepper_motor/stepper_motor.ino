#include <Stepper.h>

// Steps for one turn of the output shaft in full-step mode:
// 32 steps per motor revolution x ~64:1 gearbox = 2048
// (the real ratio is 63.68:1, so 2048 is ~0.5% more than one exact turn)
const int stepsPerRevolution = 2048;

// Output shaft speed in rpm. Above ~15 rpm the motor tends to skip steps
// or just vibrate, because the coils can't build up current fast enough.
const int rpm = 10;

// The Stepper library energizes pins in the order: pin1, pin2, pin3, pin4.
// The 28BYJ-48 coil order requires IN1, IN3, IN2, IN4 to get the correct
// sequence; with IN1-IN2-IN3-IN4 the motor vibrates instead of turning.
Stepper myStepper(stepsPerRevolution, 16, 18, 17, 19); // IN1, IN3, IN2, IN4

void setup() {
  myStepper.setSpeed(rpm);   // sets the delay between steps
  Serial.begin(9600);
}

void loop() {
  // Positive step count: one full turn in one direction.
  // step() is blocking: the sketch waits here until all steps are done
  // (2048 steps at 10 rpm = about 6 s).
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);  // the last coils stay energized during the pause (holding torque)

  // Negative step count: same sequence played backwards, opposite direction.
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
}