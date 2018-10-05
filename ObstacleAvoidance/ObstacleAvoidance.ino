//www.elegoo.com

#include <FastLED.h>
#include "Car.h"
#include "Sonar.h"
//#include "LineTracking.h"

#define LOOP_LIMIT 40

#define LT_R !digitalRead(3)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

enum Direction {
  LEFT,
  RIGHT,
  FORWARD
};

Direction lastDirection;

void callbackRightTrace();
void callbackLeftTrace();

Servo servo;
SonarServo sonar(&servo);

bool objectAvoidanceMode = false;

void setup() {
  Serial.begin(9600);
  sonar.setup();
  setupCar();
  setSpeed(255);
}

void loop() {
  FastLED.clear();

  if (objectAvoidanceMode) {
    // TODO : Track movement
    stop();
    scanDistance();
  } else {
    traceLine();
    if (sonar.pingDistance() < 20) {
      attachInterrupt(digitalPinToInterrupt(3), callbackRightTrace, LOW);
      attachInterrupt(digitalPinToInterrupt(2), callbackLeftTrace, LOW);
      objectAvoidanceMode = true;
      stop();
    }
  }
}

void scanDistance() {
  Serial.print("Left: ");
  Serial.println(sonar.pingDistance(LEFT_ANGLE));
  delay(1000);
  Serial.print("Middle: ");
  Serial.println(sonar.pingDistance(MIDDLE_ANGLE));
  delay(1000);
  Serial.print("Right: ");
  Serial.println(sonar.pingDistance(RIGHT_ANGLE));
  delay(1000);
}

void callbackLeftTrace() {
  objectAvoidanceMode = false;
  Serial.println("Interrupt Left");
  detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
  turnLeft();
}

void callbackRightTrace() {
  objectAvoidanceMode = false;
  Serial.println("Interrupt Right");
  detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
  turnRight();
}

void traceLine() {
  if (!LT_M && !LT_R && !LT_L) {
    setSpeed(175);  
    switch (lastDirection) {
      case FORWARD:
        backward();
        break;
      case LEFT:
        rotateLeft();
//        lastDirection = LEFT;
        break;
      case RIGHT:
        rotateRight();
//        lastDirection = RIGHT;
        break;
    }
  } else if (LT_M && LT_R && LT_L) {
    setSpeed(225);
    rotateRight();
    return; // TODO:
  } else if (LT_M && LT_R) {
    setSpeed(225);
    rotateRight();
    lastDirection = RIGHT;
  } else if (LT_M && LT_L) {
    setSpeed(225);
    rotateLeft();
    lastDirection = LEFT;
  } else if (LT_M) {
    setSpeed(150);
    forward();
    lastDirection = FORWARD;
  }
  else if (LT_R) {
    setSpeed(225);
    turnRight();
    lastDirection = RIGHT;
  } else if (LT_L) {
    setSpeed(225);
    turnLeft();
    lastDirection = LEFT;
  }
}
