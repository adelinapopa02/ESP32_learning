#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  21
#define RST_PIN 5
#define LED_PIN 2

MFRC522 rfid(SS_PIN, RST_PIN);

// Known UIDs: run once, read your card's UID, paste it here
const byte KNOWN_UIDS[][4] = {
  {0x2E, 0x0C, 0x16, 0x06},
  {0xBB, 0xC2, 0xFE, 0x06},
};
const int NUM_KNOWN = sizeof(KNOWN_UIDS) / sizeof(KNOWN_UIDS[0]);

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  SPI.begin();
  rfid.PCD_Init();

  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print("RC522 version: 0x");
  Serial.println(version, HEX);
  if (version == 0x00 || version == 0xFF) {
    Serial.println("No communication with RC522: check wiring and 3.3V");
    while (true) delay(1000);
  }

  Serial.println("Place a card on the reader");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial())   return;

  printUid();

  MFRC522::PICC_Type type = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print("Type: ");
  Serial.println(rfid.PICC_GetTypeName(type));

  if (isKnown()) {
    Serial.println("Known card");
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
  } else {
    Serial.println("Unknown card");
  }
  Serial.println();

  rfid.PICC_HaltA();
}

void printUid() {
  Serial.print("UID:");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();
}

bool isKnown() {
  if (rfid.uid.size != 4) return false;
  for (int k = 0; k < NUM_KNOWN; k++) {
    if (memcmp(rfid.uid.uidByte, KNOWN_UIDS[k], 4) == 0) return true;
  }
  return false;
}