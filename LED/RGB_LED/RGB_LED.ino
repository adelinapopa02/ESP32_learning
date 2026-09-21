#define RED   2
#define GREEN 4
#define BLUE  5

void setup() 
{
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
}

void setColor(int red, int green, int blue) {
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}

void loop() {
  for (int i = 0; i < 255; i++) { setColor(255 - i, i, 0); delay(10); } // red   -> green
  for (int i = 0; i < 255; i++) { setColor(0, 255 - i, i); delay(10); } // green -> blue
  for (int i = 0; i < 255; i++) { setColor(i, 0, 255 - i); delay(10); } // blue  -> red
}

