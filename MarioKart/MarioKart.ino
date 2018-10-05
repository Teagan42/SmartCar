// MarioKart

#include "Car.h"

#define HUE_CHANGE 255/NUM_LEDS

// Speed Definitions
#define MAX_SPEED 255
#define MED_SPEED 150

int rainbowStart = 0;
bool state = LOW;
char getstr;

void starPower() {
  if (micros() % 200) {
    fill_rainbow(leds, NUM_LEDS, rainbowStart++, HUE_CHANGE);
  }
  FastLED.setBrightness(random(50, 200));
  FastLED.show();
}

void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(125);
  FastLED.show();
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  setupCar();
  stop();
}

void loop() {
  starPower();
  getstr = Serial.read();
  switch (getstr) {
    case 'i': setSpeed(MED_SPEED); forward();     break; // forward medium speed
    case 'x': setSpeed(MAX_SPEED); forward();     break; // forward max speed
    case 'k': setSpeed(MED_SPEED); backward();     break; // back: medium speed
    case 'z': setSpeed(MAX_SPEED); backward();     break; // back: max speed
    case 'j': setSpeed(MAX_SPEED); turnLeft();    break; 
    case 'l': setSpeed(MAX_SPEED); turnRight();   break;
    case 'p': stop();        break;
    case 'u': setSpeed(MAX_SPEED); rotateLeft();  break;
    case 'y': setSpeed(MAX_SPEED); rotateRight(); break;
    default:  break;
  }
}

// FastLED.clear()
