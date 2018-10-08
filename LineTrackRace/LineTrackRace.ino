//www.elegoo.com

#include <FastLED.h>
#include "Car.h"

// Uncomment if white tape on black floor
//#define WHITE_ON_BLACK

#ifndef WHITE_ON_BLACK

  #define LT_R !digitalRead(3)
  #define LT_M !digitalRead(4)
  #define LT_L !digitalRead(2)

  #define LT_INTERRUPT LOW

#else

  #define LT_R digitalRead(3)
  #define LT_M digitalRead(4)
  #define LT_L digitalRead(2)

  #define LT_INTERRUPT HIGH
#endif

void setup() {
  Serial.begin(9600);
  sonar.setup();
  setupCar();
  setSpeed(255);
}

void loop() {
  traceLine();
}

void traceLine() {
  if (LT_M && LT_R) {
    rotateRight(1);
  } else if (LT_M && LT_L) {
    rotateLeft(1);
  } else if (LT_M) {
    forward();
  }
  else if (LT_R) {
    turnRight(2);
  } else if (LT_L) {
    turnLeft(2);
  }
}
