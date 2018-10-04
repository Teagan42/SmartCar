#ifndef __CAR_H__
#define __CAR_H__

#include <Arduino.h>

#define ENA 5
#define ENB 6
#define IN1 13
#define IN2 8
#define IN3 9
#define IN4 11
#define CAR_SPEED 255



void forward(){ 
  analogWrite(ENA, CAR_SPEED);
  analogWrite(ENB, CAR_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Forward");
//  setBrakeLights(CRGB(0,255,0));
}

void backward() {
  analogWrite(ENA, CAR_SPEED);
  analogWrite(ENB, CAR_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Back");
}

void rotateLeft() {
  analogWrite(ENA, CAR_SPEED);
  analogWrite(ENB, CAR_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH); 
  Serial.println("Left");
//  setTurnSignal(CRGB(255,255,0), CRGB(0, 0, 0));
}

void turnLeft() {
  digitalWrite(ENA, CAR_SPEED);
  analogWrite(ENB, CAR_SPEED/2);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH); 
  Serial.println("Left");
//  setTurnSignal(CRGB(255,255,0), CRGB(0, 0, 0));
}

void rotateRight() {
  analogWrite(ENA, CAR_SPEED);
  analogWrite(ENB, CAR_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Right");
//  setTurnSignal(CRGB(0,0,0), CRGB(255, 255, 0));
}

void turnRight() {
  digitalWrite(ENA, CAR_SPEED/2);
  analogWrite(ENB, CAR_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Right");
//  setTurnSignal(CRGB(0,0,0), CRGB(255, 255, 0));
}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  Serial.println("Stop!");
//  setBrakeLights(CRGB(255,0,0));
}

#endif
