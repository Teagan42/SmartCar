#include <FastLED.h>

#define LED_PIN     7
#define ENA 5
#define ENB 6
#define IN1 13
#define IN2 8
#define IN3 9
#define IN4 11
#define carSpeed 255
#define RIGHT_ANGLE 135 //10
#define MIDDLE_ANGLE 90
#define LEFT_ANGLE 45 //180
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    33
#define HUE_CHANGE 255/NUM_LEDS

int rainbowStart = 0;

CRGB leds[NUM_LEDS];

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
}

void loop() {
  starPower();
}
