#include <Arduino.h>

// put function declarations here:

int pin_dcmotorspeed=3;
int pin_dcmotor_dirc=2;

int dc_motor_speed=0;


void setup() {
    pinMode(pin_dcmotorspeed,OUTPUT);
    pinMode(pin_dcmotor_dirc,OUTPUT);
}

void loop() {
  for(int i=100;i<220;i++){
    analogWrite(pin_dcmotorspeed,i);
    delay(5);
  }
  analogWrite(pin_dcmotorspeed,0);
  delay(3000);
 
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}