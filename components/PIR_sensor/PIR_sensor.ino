const int ledPin = 2;
const int pirPin = 18;

const unsigned long WARMUP_MS = 60000;
const unsigned long SAMPLE_MS = 50;

int lastState = LOW;
bool pirReady = false;
unsigned long bootTime = 0;
unsigned long lastSample = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  bootTime = millis();
  Serial.println("Warming up PIR (60 s)...");
}

void loop() {
  unsigned long now = millis();

  // Warm-up phase: ignore the PIR, but the loop keeps running
  if (!pirReady) {
    if (now - bootTime >= WARMUP_MS) {
      pirReady = true;
      Serial.println("Ready");
    }
  }

  // PIR handling, only after warm-up, once every SAMPLE_MS
  if (pirReady && now - lastSample >= SAMPLE_MS) {
    lastSample = now;

    int state = digitalRead(pirPin);

    if (state == HIGH && lastState == LOW) {
      digitalWrite(ledPin, HIGH);
      Serial.println("Motion detected");
    } else if (state == LOW && lastState == HIGH) {
      digitalWrite(ledPin, LOW);
      Serial.println("Motion ended");
    }

    lastState = state;
  }
  // Other tasks can go here: they run immediately, even during warm-up
}