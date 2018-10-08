#ifndef __CAR_H__
#define __CAR_H__

#include <Arduino.h>
#include <FastLED.h>

// Motor Pins
#define ENA 5
#define ENB 6
#define IN1 13
#define IN2 8
#define IN3 9
#define IN4 11

// LED Definitions
#define LED_PIN     7
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    33

#define BRAKE_LED_START_INDEX 6
#define BRAKE_LED_END_INDEX 13

#define LEFT_BRAKE_LED_START_INDEX BRAKE_LED_START_INDEX
#define LEFT_BRAKE_LED_END_INDEX 8

#define RIGHT_BRAKE_LED_START_INDEX 10
#define RIGHT_BRAKE_LED_END_INDEX BRAKE_LED_END_INDEX

#define LEFT_HEAD_LED_START_INDEX 29
#define LEFT_HEAD_LED_END_INDEX 31

#define RIGHT_HEAD_LED_START_INDEX 25
#define RIGHT_HEAD_LED_END_INDEX 27

#define BEAR_MILLISECONDS 50

enum Direction {
  LEFT,
  RIGHT,
  FORWARD,
  BACKWARD,
  NOT_FOUND
};

void stop();
void setBrakeLights(CRGB lightColor);
void setTurnSignal(CRGB leftColor, CRGB rightColor);

CRGB leds[NUM_LEDS];

int carSpeed = 255;
Direction currentDirection = NOT_FOUND;

Direction getCurrentRotation() {
  return currentDirection;
}

void setSpeed(int speed) {
  carSpeed = speed;
}

void setupCar() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  stop();
  
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( 125 );
  FastLED.show();
}

void forward(){ 
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  currentDirection = FORWARD;
  setBrakeLights(CRGB(0,255,0));
}

void backward() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  currentDirection = BACKWARD;
}

void rotateLeft(int multiplier = 1) {
  analogWrite(ENA, 255/multiplier);
  analogWrite(ENB, 255);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH); 
  currentDirection = LEFT;
  setTurnSignal(CRGB(255,255,0), CRGB(0, 0, 0));
}

void turnLeft(int multiplier = 4) {
  analogWrite(ENA, carSpeed/multiplier);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  currentDirection = LEFT;
  setTurnSignal(CRGB(255,255,0), CRGB(0, 0, 0));
}

void rotateRight(int multiplier = 1) {
  analogWrite(ENA, 255);
  analogWrite(ENB, 255/multiplier);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  currentDirection = RIGHT;
  setTurnSignal(CRGB(0,0,0), CRGB(255, 255, 0));
}

void turnRight(int multiplier = 4) {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed/multiplier);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  currentDirection = RIGHT;
  setTurnSignal(CRGB(0,0,0), CRGB(255, 255, 0));
}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  currentDirection = NOT_FOUND;
  setBrakeLights(CRGB(255,0,0));
}

void setBrakeLights(CRGB color) {
  FastLED.clear();
  for(int i = BRAKE_LED_START_INDEX; i < BRAKE_LED_END_INDEX; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void setTurnSignal(CRGB left, CRGB right) {
  FastLED.clear();
  for(int i = LEFT_BRAKE_LED_START_INDEX; i < LEFT_BRAKE_LED_END_INDEX; i++) {
    leds[i] = left;
  }

  for(int i = LEFT_HEAD_LED_START_INDEX; i < LEFT_HEAD_LED_END_INDEX; i++) {
    leds[i] = left;
  }

  for(int i = RIGHT_BRAKE_LED_START_INDEX; i < RIGHT_BRAKE_LED_END_INDEX; i++) {
    leds[i] = right;
  }

  for(int i = RIGHT_HEAD_LED_START_INDEX; i < RIGHT_HEAD_LED_END_INDEX; i++) {
    leds[i] = right;
  }
  FastLED.show();
}

#endif
