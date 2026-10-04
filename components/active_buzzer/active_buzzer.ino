int buzzer = 4;

void setup() {
  pinMode(buzzer, OUTPUT);
}

void loop() {
  unsigned char i;
  for (i = 0; i < 3; i++) {
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
    delay(5000);
  }
  delay(10000);
}
