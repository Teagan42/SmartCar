#ifndef __SONAR_H__
#define __SONAR_H__

#include <NewPing.h>
#include <Servo.h>
#include <Arduino.h>
#include <TimerOne.h>
#include <Thread.h>
#include <ThreadController.h>

#define ECHO A4  
#define TRIG A5
#define SERVO_PIN 10

// Sonar Definition
#define RIGHT_ANGLE 180 //10
#define MIDDLE_ANGLE 90
#define LEFT_ANGLE 0 //180

class SonarServo {
  public:
  SonarServo(Servo* servo);
  int pingDistance();
  int pingDistance(int angle);
  void setup();
  void setAngle(int angle);

  private:
  Servo* servo;
//  NewPing* sonar;
  int lastAngle = -100;
};

#endif
