#ifndef __SONAR_H__
#define __SONAR_H__

#include <Servo.h>
#include <Arduino.h>

#define ECHO A4  
#define TRIG A5
#define SERVO_PIN 3

// Sonar Definition
#define RIGHT_ANGLE 135 //10
#define MIDDLE_ANGLE 90
#define LEFT_ANGLE 45 //180

class Sonar {
  public:
  Sonar(Servo* servo);
  int pingDistance();
  int pingDistance(int angle);
  void setup();
  void setAngle(int angle);

  private:
  Servo* servo;
  int lastAngle = -100;
};

#endif
