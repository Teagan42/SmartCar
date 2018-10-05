#ifndef __SONAR_H__
#define __SONAR_H__

#define ECHO A4  
#define TRIG A5
#define SERVO_PIN 3

int pingDistance();
int pingDistance(int angle);
void setupSonar();
void setAngle(int angle);

Servo sonarServo;
int lastAngle = 0;

int pingDistance(int angle) {
  setAngle(angle);
  return pingDistance();
}

int pingDistance() {
  digitalWrite(TRIG, LOW);   
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);  
  delayMicroseconds(20);
  digitalWrite(TRIG, LOW);   
  return (int)(pulseIn(ECHO, HIGH) / 58);
}

void setupSonar() {
  pinMode(ECHO, INPUT);    
  pinMode(TRIG, OUTPUT); 

  sonarServo.attach(SERVO_PIN);
  setAngle(90);
}

void setAngle(int angle) {
  if (angle != lastAngle) {
    sonarServo.write(angle);
    lastAngle = angle;
    delay(300);
  }
}

#endif
