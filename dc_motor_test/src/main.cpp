#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

int pin_dcmotorspeed=6;

void setup() {
    pinMode(pin_dcmotorspeed,OUTPUT);
}

void loop() {
    analogWrite(pin_dcmotorspeed,0);
    delay(2000);
    // digitalWrite(pin_dcmotordirc,HIGH);
    analogWrite(pin_dcmotorspeed,100);
    delay(2000);
    // digitalWrite(pin_dcmotordirc,HIGH);
    analogWrite(pin_dcmotorspeed,200);
    delay(2000);
    analogWrite(pin_dcmotorspeed,255);
    delay(2000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}