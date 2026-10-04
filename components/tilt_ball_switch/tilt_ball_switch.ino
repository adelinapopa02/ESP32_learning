const int tiltPin = 4;
int state;
int lastReading;
unsigned long lastChangeTime = 0;
unsigned long stateStartTime = 0;
const unsigned long debounceDelay = 100; // ms

void setup() {
  Serial.begin(115200);
  pinMode(tiltPin, INPUT_PULLUP);

  state = digitalRead(tiltPin);
  lastReading = state;
  Serial.print(state == LOW ? "UPWARDS" : "DOWNWARDS");
}

void loop() {
  int reading = digitalRead(tiltPin);

  if (reading != lastReading) {
    lastChangeTime = millis();
  }

  if ((millis()-lastChangeTime) > debounceDelay) {
    if(reading != state) {

      unsigned long duration = millis() - stateStartTime;
      Serial.print(" for ");
      Serial.print(duration);
      Serial.println(" ms");

      state = reading;
      stateStartTime = millis();
      Serial.print(state == LOW ? "UPWARDS" : "DOWNWARDS"); 
    }
  }
  lastReading = reading;
}
