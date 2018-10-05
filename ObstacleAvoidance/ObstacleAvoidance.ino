//www.elegoo.com

#include <FastLED.h>
#include "Car.h"
#include "SonarThread.h"
#include "LineTracking.h"

#define LOOP_LIMIT 40

void callbackLineTracking(Direction dir);
void callbackSonarObstacle();
void timerCallback();

Sonar sonar(&Servo());

LineTrackingThread lineTrackingThread(callbackLineTracking);
SonarThread sonarThread(&sonar, callbackSonarObstacle);

ThreadController controller = ThreadController();

bool objectAvoidanceMode = false;

void timerCallback(){
  controller.run();
}

void setup() { 
  Serial.begin(9600);  
  setupCar();  
  sonar.setup();
  setSpeed(200);

  lineTrackingThread.setInterval(50);
  sonarThread.setInterval(40);
  controller.add(&lineTrackingThread);
  controller.add(&sonarThread);

   Timer1.initialize(20000);
   Timer1.attachInterrupt(timerCallback);
   Timer1.start();
} 

void loop() { 
  FastLED.clear();
//  if(LT_M || LT_R || LT_L) {
//        objectAvoidanceMode = false;
//      }
    int leftDistance = sonar.pingDistance(LEFT_ANGLE);
    if (leftDistance <= 20) {
      Serial.println("Obstacle!");
      stop();
    }
    int middleDistance = sonar.pingDistance(MIDDLE_ANGLE);
    if (middleDistance <= 20) {
      Serial.println("Obstacle!");
      stop();
    }
    int rightDistance = sonar.pingDistance(RIGHT_ANGLE);
    if (rightDistance <= 20) {
      Serial.println("Obstacle!");
      stop();
    }
    sonar.setAngle(MIDDLE_ANGLE);

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

void callbackSonarObstacle() {
  stop();
  objectAvoidanceMode = true;
  lineTrackingThread.enabled = false;
  // Perform Obstacle Avoidance
  // TODO: Re-enable linetracking thread once avoidance started
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
