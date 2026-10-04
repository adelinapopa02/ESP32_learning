const int SW_pin = 18;
const int X_pin = 34;
const int Y_pin = 35;

void setup() {
  pinMode(SW_pin, INPUT_PULLUP);
  pinMode(X_pin, INPUT);
  pinMode(Y_pin, INPUT);

  Serial.begin(9600);
}

void loop() {

  Serial.print("Switch: ");
  Serial.println(digitalRead(SW_pin));

  Serial.print("X-axis: ");
  Serial.println(analogRead(X_pin));

  Serial.print("Y-axis: ");
  Serial.println(analogRead(Y_pin));

  Serial.print("\n");
  delay(1000); 
}
