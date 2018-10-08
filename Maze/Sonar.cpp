#include "Sonar.h"

SonarServo::SonarServo(Servo* servo) {
  this->servo = servo;
}

int SonarServo::pingDistance(int angle) {
  setAngle(angle);
  return pingDistance();
}

int SonarServo::pingDistance() {
  digitalWrite(TRIG, LOW);   
  int now = micros();
  while (micros() - now < 2);
  digitalWrite(TRIG, HIGH);  
  now = micros();
  while (micros() - now < 20);
  digitalWrite(TRIG, LOW);   
  return (int)(pulseIn(ECHO, HIGH) / 58);

//  return sonar->ping_cm();
}

void SonarServo::setup() {
  pinMode(ECHO, INPUT);    
  pinMode(TRIG, OUTPUT); 

  this->servo->attach(SERVO_PIN);
  setAngle(MIDDLE_ANGLE);
  delay(50);

//  sonar = &NewPing(TRIG, ECHO, 3000);
}

void SonarServo::setAngle(int angle) {
  if (angle != lastAngle) {
    this->servo->write(angle);
    delay(2*abs(angle - lastAngle));
    lastAngle = angle;
  }
}
