//www.elegoo.com

#include <Servo.h>  //servo library
#include <FastLED.h>
#include "Car.h"
#include "Sonar.h"

//Line Tracking IO define
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

// Sonar Definition
#define RIGHT_ANGLE 135 //10
#define MIDDLE_ANGLE 90
#define LEFT_ANGLE 45 //180

#define LOOP_LIMIT 40

bool objectAvoidanceMode = false;

void setup() { 
  Serial.begin(9600);  
  setupCar();  
  setupSonar();
  setSpeed(100);
} 

void loop() { 
  FastLED.clear();
  if(LT_M || LT_R || LT_L) {
        objectAvoidanceMode = false;
      }
    int leftDistance = pingDistance(LEFT_ANGLE);
    if (leftDistance <= 20) {
      Serial.println("Obstacle!");
      stop();
    }
    if(LT_M || LT_R || LT_L) {
        objectAvoidanceMode = false;
      }
    int middleDistance = pingDistance(MIDDLE_ANGLE);
    if (middleDistance <= 20) {
      Serial.println("Obstacle!");
      stop();
    }
    if(LT_M || LT_R || LT_L) {
        objectAvoidanceMode = false;
      }
    int rightDistance = pingDistance(RIGHT_ANGLE);
    if (rightDistance <= 20) {
      Serial.println("Obstacle!");
      stop();
    }
    if(LT_M || LT_R || LT_L) {
        objectAvoidanceMode = false;
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
      if(!LT_M && !LT_R && !LT_L) {
        objectAvoidanceMode= true;
      }
    } else if (!objectAvoidanceMode) {
      setSpeed(255);
      if(LT_M){
        forward();
      }
      else if(LT_R) { 
        rotateRight_LineTrace();                            
      }   
      else if(LT_L) {
        rotateLeft_LineTrace();  
      }
    } else {
      // TODO : Track movement
      setSpeed(150);
      forward();  
      if(LT_M || LT_R || LT_L) {
        objectAvoidanceMode = false;
      }
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
