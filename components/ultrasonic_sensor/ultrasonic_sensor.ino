#include "SR04.h"

#define TRIG_PIN 18
#define ECHO_PIN 19

SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long distance; 

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  
  // from SR04 library                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
  // pulseIn measures how long ECHO stays high = ToF
  // then converts the time to cm
  distance = sr04.Distance();
  Serial.print(distance);
  Serial.println("cm");

  delay(1000);
}
