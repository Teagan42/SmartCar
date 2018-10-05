//www.elegoo.com

#include <Servo.h>  //servo library
#include <FastLED.h>
#include "Car.h"
#include "Sonar.h"
#include "LineTracking.h"

// Sonar Definition
#define RIGHT_ANGLE 135 //10
#define MIDDLE_ANGLE 90
#define LEFT_ANGLE 45 //180

#define LOOP_LIMIT 40

void callbackLineTracking(Direction dir);
void timerCallback();

LineTrackingThread lineTrackingThread(callbackLineTracking);

ThreadController controller = ThreadController();

bool objectAvoidanceMode = false;

void timerCallback(){
  controller.run();
}

void setup() { 
  Serial.begin(9600);  
  setupCar();  
  setupSonar();
  setSpeed(200);

  lineTrackingThread.setInterval(50);
  controller.add(&lineTrackingThread);

   Timer1.initialize(20000);
   Timer1.attachInterrupt(timerCallback);
   Timer1.start();
} 

void loop() { 
  FastLED.clear();
//  if(LT_M || LT_R || LT_L) {
//        objectAvoidanceMode = false;
//      }
    int leftDistance = pingDistance(LEFT_ANGLE);
    if (leftDistance <= 20) {
      Serial.println("Obstacle!");
      stop();
    }
    int middleDistance = pingDistance(MIDDLE_ANGLE);
    if (middleDistance <= 20) {
      Serial.println("Obstacle!");
      stop();
    }
    int rightDistance = pingDistance(RIGHT_ANGLE);
    if (rightDistance <= 20) {
      Serial.println("Obstacle!");
      stop();
    }
    setAngle(MIDDLE_ANGLE);

    if(middleDistance <= 20 || leftDistance <= 20 || rightDistance <= 20) {
      stop();
      setSpeed(100);
//      Serial.println("Testing right");
//      int rightDistance = pingDistance(RIGHT_ANGLE);
//      Serial.println("Testing left");
//      int leftDistance = pingDistance(LEFT_ANGLE);
      
      if(rightDistance > leftDistance) {
        setSpeed(255);
        rotateRight();
        delay(60);
      }
      else if(rightDistance < leftDistance) {
        setSpeed(255);
        rotateLeft(); 
        delay(60);
      }
      else if((rightDistance <= 20) || (leftDistance <= 20)) {
        setSpeed(200);
        backward();
        delay(180);
      }
      else {
        setSpeed(150);
        forward();
      }
    } else if (objectAvoidanceMode) {
      // TODO : Track movement
      setSpeed(150);
      forward();
    }
}

void rotateRight_LineTrace() {
  turnRight();
  int time = micros();
  while(LT_R && micros() - time < LOOP_LIMIT); 
}

void rotateLeft_LineTrace() {
  turnLeft();
  int time = micros();
  while(LT_L && micros() - time < LOOP_LIMIT); 
}

void callbackLineTracking(Direction dir) {
  Serial.print("Line Callback: ");
  switch(dir) {
    case NOT_FOUND: 
    Serial.println("NOT FOUND");
    break;
    case PERPENDICULAR:
      Serial.println("PERP");
      objectAvoidanceMode = false;
    // TODO: Rotate, which way?
      break;
    case FORWARD:
    Serial.println("Forward");
      objectAvoidanceMode = false;
      forward();
      break;
    case LEFT:
    Serial.println("LEFT");
      objectAvoidanceMode = false;
      turnLeft();  
      break;
    case RIGHT:
    Serial.println("RIGHT");
      objectAvoidanceMode = false;
      turnRight();
      break;
  }
}
