// DC motor control with L293D (one motor, left half of the chip)
//
// L293D truth table for one motor (EN = pin 1, IN1 = pin 2, IN2 = pin 7):
//   EN   IN1  IN2   Result
//   0    x    x     Coast: outputs off, motor spins down freely
//   1    1    0     Forward
//   1    0    1     Reverse
//   1    0    0     Brake: both motor terminals tied to GND, motor stops fast
//   1    1    1     Brake: both motor terminals tied to VCC2
// PWM on EN switches the motor on/off rapidly; the duty cycle sets the
// average voltage across the motor, and therefore its speed.

#define ENABLE 25   // PWM speed control
#define DIRA   26   // direction input A
#define DIRB   27   // direction input B

int i;

void setup() {
  pinMode(ENABLE, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // ---- Section 1: direction reversal ----
  // ENABLE stays on; only IN1/IN2 swap, so the motor reverses every 0.5 s.
  Serial.println("One way, then reverse");
  analogWrite(ENABLE, 255);          // EN = 100% duty: driver outputs active
  for (i = 0; i < 5; i++) {
    digitalWrite(DIRA, HIGH);        // IN1=1, IN2=0 -> forward
    digitalWrite(DIRB, LOW);
    delay(500);
    digitalWrite(DIRA, LOW);         // IN1=0, IN2=1 -> reverse
    digitalWrite(DIRB, HIGH);
    delay(500);
  }
  analogWrite(ENABLE, 0);            // EN = 0: outputs off, motor coasts to a stop
  delay(2000);

  // ---- Section 2: coast stop vs brake stop ----
  Serial.println("Fast/slow stop example");
  analogWrite(ENABLE, 255);          // EN on
  digitalWrite(DIRA, HIGH);          // forward
  digitalWrite(DIRB, LOW);
  delay(3000);
  analogWrite(ENABLE, 0);            // Coast: outputs disconnected, motor slows
                                     // down only by friction
  delay(1000);
  analogWrite(ENABLE, 255);          // EN on
  digitalWrite(DIRA, LOW);           // reverse
  digitalWrite(DIRB, HIGH);
  delay(3000);
  digitalWrite(DIRB, LOW);           // Brake: IN1=IN2=0 with EN on, both motor
                                     // terminals shorted to GND; the motor's own
                                     // back-EMF opposes rotation -> fast stop
  delay(2000);

  // ---- Section 3: speed control with PWM ----
  // analogWrite value 0-255 = duty cycle 0-100% on EN.
  // Direction is set before enabling so the motor starts in the right direction.
  Serial.println("PWM full then slow");
  digitalWrite(DIRA, HIGH);          // forward
  digitalWrite(DIRB, LOW);
  analogWrite(ENABLE, 255);          // 100% duty: full speed
  delay(2000);
  analogWrite(ENABLE, 180);          // ~71% duty
  delay(2000);
  analogWrite(ENABLE, 128);          // ~50% duty
  delay(2000);
  analogWrite(ENABLE, 50);           // ~20% duty: average voltage likely below the
                                     // motor's starting threshold, may only hum
  delay(2000);
  analogWrite(ENABLE, 128);          // ~50% duty
  delay(2000);
  analogWrite(ENABLE, 180);          // ~71% duty
  delay(2000);
  analogWrite(ENABLE, 255);          // 100% duty
  delay(2000);
  analogWrite(ENABLE, 0);            // EN = 0: coast to a stop, end of cycle
  delay(10000);
}