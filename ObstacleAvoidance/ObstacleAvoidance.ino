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

void setup() { 
  myservo.attach(3);  // attach servo on pin 3 to servo object
  Serial.begin(9600);  
  setupCar();  
  setupSonar();
} 

void loop() { 
  FastLED.clear();
    middleDistance = pingDistance(MIDDLE_ANGLE);

    if(middleDistance <= 20) {
      stop();
      Serial.println("Obstacle!");
      Serial.println("Testing right");
      rightDistance = pingDistance(RIGHT_ANGLE);
      Serial.println("Testing left");
      leftDistance = pingDistance(LEFT_ANGLE);
      setAngle(MIDDLE_ANGLE);
      
      if(rightDistance > leftDistance) {
        rotateRight(); 
      }
      else if(rightDistance < leftDistance) {
        rotateLeft(); 
      }
      else if((rightDistance <= 20) || (leftDistance <= 20)) {
        backward();
        delay(180);
      }
      else {
        forward();
      }
    } else {
      if(LT_M){
        forward();
      }
      else if(LT_R) { 
        rotateRight_LineTrace();                            
      }   
      else if(LT_L) {
        rotateLeft_LineTrace();  
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
