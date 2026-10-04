const int IR = 19; 

unsigned long readPulse(int level, unsigned long timeout = 20000) {
  unsigned long start = micros();
  
  while(digitalRead(IR) == level)
  {
    if(micros() - start > timeout) return 0;
  }
  return micros() - start;
}

void setup() {
  Serial.begin(9600);
  pinMode(IR, INPUT);
  Serial.println("NEC decoder started...");
}

void loop() {
  if(digitalRead(IR) == LOW) {

    unsigned long lowTime = readPulse(LOW);
    // 8ms ~ 10ms, target: 9ms
    if (lowTime < 8000 || lowTime > 10000) return;

    unsigned long highTime = readPulse(HIGH);
    // 4ms ~ 5ms, target: 4.5ms
    if (highTime < 4000 || highTime > 5000) return;

    // start reading 32-bit data
    unsigned long code = 0;

    for (int i = 0; i < 32; i++) {
      // Each bit starts with 560us LOW
      unsigned long bitLow = readPulse(LOW);
      // 400us ~ 700us, target: 560us
      if (bitLow < 400 || bitLow > 700) return;

      // 560us = 0, 1680us = 1
      unsigned long bitHigh = readPulse(HIGH);
      if (bitHigh == 0) return; 

      code <<= 1;  // shift left to make room for new bit
      if (bitHigh > 1000) code |= 1;  // long HIGH (1680us) = bit 1
      else code |= 0;                 // short HIGH (560us) = bit 0
    }

    Serial.print("Received code: 0x");
    Serial.println(code, HEX);

  switch (code) {
      case 0xFFA25D: Serial.println("POWER"); break;
      case 0xFFE21D: Serial.println("FUNC/STOP"); break;
      case 0xFF629D: Serial.println("VOL+"); break;
      case 0xFF22DD: Serial.println("FAST BACK"); break;
      case 0xFF02FD: Serial.println("PAUSE"); break;
      case 0xFFC23D: Serial.println("FAST FORWARD"); break;
      case 0xFFE01F: Serial.println("DOWN"); break;
      case 0xFFA857: Serial.println("VOL-"); break;
      case 0xFF906F: Serial.println("UP"); break;
      case 0xFF9867: Serial.println("EQ"); break;
      case 0xFFB04F: Serial.println("ST/REPT"); break;
      case 0xFF6897: Serial.println("0"); break;
      case 0xFF30CF: Serial.println("1"); break;
      case 0xFF18E7: Serial.println("2"); break;
      case 0xFF7A85: Serial.println("3"); break;
      case 0xFF10EF: Serial.println("4"); break;
      case 0xFF38C7: Serial.println("5"); break;
      case 0xFF5AA5: Serial.println("6"); break;
      case 0xFF42BD: Serial.println("7"); break;
      case 0xFF4AB5: Serial.println("8"); break;
      case 0xFF52AD: Serial.println("9"); break;
      case 0xFFFFFFFF: Serial.println("REPEAT"); break;
      default: Serial.println("OTHER BUTTON"); break;
    }

    delay(150);  // Debounce delay to avoid duplicate readings
  }
}
