#include "ESP32Servo.h"

Servo srv;

void setup() {
  srv.attach(13);
  srv.write(90); 
}

void loop() {
  srv.write(90);
  delay(1000);-=
  srv.write(60);
  delay(1000);
  srv.write(150);
  delay(1000);
}
