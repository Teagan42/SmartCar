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

#define BEAR_MILLISECONDS 50

void stop();
void setBrakeLights(CRGB lightColor);
void setTurnSignal(CRGB leftColor, CRGB rightColor);

CRGB leds[NUM_LEDS];

int carSpeed = 255;

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
  Serial.println("Forward");
  setBrakeLights(CRGB(0,255,0));
}

void backward() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Back");
}

void rotateLeft() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH); 
  Serial.println("Left");
  setTurnSignal(CRGB(255,255,0), CRGB(0, 0, 0));
}

void turnLeft() {
  analogWrite(ENA, carSpeed/8);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Left");
  setTurnSignal(CRGB(255,255,0), CRGB(0, 0, 0));
}

void rotateRight() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Right");
  setTurnSignal(CRGB(0,0,0), CRGB(255, 255, 0));
}

void turnRight() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed/8);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Right");
  setTurnSignal(CRGB(0,0,0), CRGB(255, 255, 0));
}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  Serial.println("Stop!");
  setBrakeLights(CRGB(255,0,0));
}

void setBrakeLights(CRGB color) {
  for(int i = BRAKE_LED_START_INDEX; i < BRAKE_LED_END_INDEX; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void setTurnSignal(CRGB left, CRGB right) {
  for(int i = LEFT_BRAKE_LED_START_INDEX; i < LEFT_BRAKE_LED_END_INDEX; i++) {
    leds[i] = left;
  }

  for(int i = RIGHT_BRAKE_LED_START_INDEX; i < RIGHT_BRAKE_LED_END_INDEX; i++) {
    leds[i] = right;
  }
  FastLED.show();
}

#endif
