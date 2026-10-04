#include "pitches.h"

int buzzer = 4;
int melody[] = {NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};
int duration = 500;

void setup() {
  pinMode(buzzer, OUTPUT);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    tone(buzzer, melody[i], duration);
    delay(1000);
  }
  delay(2000);
}
