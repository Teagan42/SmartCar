//www.elegoo.com

#include <FastLED.h>
#include "Car.h"
#include "Sonar.h"
//#include "LineTracking.h"

#define LOOP_LIMIT 40

#define LT_R !digitalRead(3)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

void callbackRightTrace();
void callbackLeftTrace();

SonarServo sonar(&Servo());

bool objectAvoidanceMode = false;

void setup() { 
  Serial.begin(9600); 
  sonar.setup();
  setupCar();  
  setSpeed(200);
} 

void loop() { 
  FastLED.clear();
//  if(LT_M || LT_R || LT_L) {
//        objectAvoidanceMode = false;
//      }
//    int leftDistance = sonar.pingDistance(LEFT_ANGLE);
//    if (leftDistance <= 20) {
//      Serial.println("Obstacle!");
//      stop();
//    }
//    int middleDistance = sonar.pingDistance(MIDDLE_ANGLE);
//    if (middleDistance <= 20) {
//      Serial.println("Obstacle!");
//      stop();
//    }
//    int rightDistance = sonar.pingDistance(RIGHT_ANGLE);
//    if (rightDistance <= 20) {
//      Serial.println("Obstacle!");
//      stop();
//    }
//    sonar.setAngle(MIDDLE_ANGLE);
//
//    if(middleDistance <= 20 || leftDistance <= 20 || rightDistance <= 20) {
//      stop();
//      setSpeed(100);
////      Serial.println("Testing right");
////      int rightDistance = pingDistance(RIGHT_ANGLE);
////      Serial.println("Testing left");
////      int leftDistance = pingDistance(LEFT_ANGLE);
//      
//      if(rightDistance > leftDistance) {
//        setSpeed(255);
//        rotateRight();
//        delay(60);
//      }
//      else if(rightDistance < leftDistance) {
//        setSpeed(255);
//        rotateLeft(); 
//        delay(60);
//      }
//      else if((rightDistance <= 20) || (leftDistance <= 20)) {
//        setSpeed(200);
//        backward();
//        delay(180);
//      }
//      else {
//        setSpeed(150);
//        forward();
//      }
//    } else 
    if (objectAvoidanceMode) {
      // TODO : Track movement
      scanDistance();
      attachInterrupt(digitalPinToInterrupt(3), callbackRightTrace, FALLING);
  attachInterrupt(digitalPinToInterrupt(2), callbackLeftTrace, FALLING);
//      setSpeed(150);
//      forward();
    } else {
      detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
      traceLine();
      if (sonar.pingDistance() < 20) {
        objectAvoidanceMode = true;
        stop();
      }
    }
}

void scanDistance() {
  sonar.pingDistance(LEFT_ANGLE);
  delay(1000);
  sonar.pingDistance(MIDDLE_ANGLE);
  delay(1000);
  sonar.pingDistance(RIGHT_ANGLE);
  delay(1000);
}

void callbackLeftTrace() {
  Serial.println("Interrupt Left");
  objectAvoidanceMode = false;
  turnLeft();
}

void callbackRightTrace() {
  Serial.println("Interrupt Right");
  objectAvoidanceMode = false;
  turnRight();
}

void traceLine() {
  if (!LT_M && !LT_R && !LT_L) {
    return; // TODO:
  } else if (LT_M && LT_R && LT_L) {
    return; // TODO:
  } else if (LT_M) {
    objectAvoidanceMode = false;
    forward();
  }
  else if (LT_R) {
    callbackRightTrace();
  } else if (LT_L) {
    callbackLeftTrace();
  }
}
