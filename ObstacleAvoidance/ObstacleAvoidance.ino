//www.elegoo.com

#include <Servo.h>  //servo library
#include <FastLED.h>
#include "Car.h"
Servo myservo;      // create servo object to control servo

//Line Tracking IO define
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

// Sonar Definition
#define RIGHT_ANGLE 135 //10
#define MIDDLE_ANGLE 90
#define LEFT_ANGLE 45 //180
#define ECHO A4  
#define TRIG A5 

// LED Definitions
#define LED_PIN     7
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    33

#define LOOP_LIMIT 40

int rightDistance = 0, leftDistance = 0, middleDistance = 0;

CRGB leds[NUM_LEDS];

//Ultrasonic distance measurement Sub function
int Distance_test() {
  digitalWrite(TRIG, LOW);   
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);  
  delayMicroseconds(20);
  digitalWrite(TRIG, LOW);   
  float Fdistance = pulseIn(ECHO, HIGH);  
  Fdistance= Fdistance / 58;       
  return (int)Fdistance;
}

void setup() { 
  myservo.attach(3);  // attach servo on pin 3 to servo object
  Serial.begin(9600);    
  pinMode(ECHO, INPUT);    
  pinMode(TRIG, OUTPUT);  
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
  //fullScan();
} 

void loop() { 
  FastLED.clear();
//    myservo.write(MIDDLE_ANGLE);  //setservo position according to scaled value
//    delay(30); 
//    Serial.println("Testing middle");
//    middleDistance = Distance_test();
//
//    if(middleDistance <= 20) {   
//      Serial.println("Obstacle!");
//      stop();
//      fullScan();
//      for (int i = 1; i < SCAN_NUM/2 - 1; i++) {
//        int distanceLeft = distanceScan[90-i];
//        int distanceRight = distanceScan[90+i];
//        if (distanceLeft <= 25 || distanceRight <= 25) {
//          continue;
//        } else if (distanceLeft < distanceRight) {
//          turnRight();
//          delay(50);
//          stop();
//          break;
//        } else if (distanceLeft > distanceRight) {
//          turnLeft();
//          delay(50);
//          stop();
//          break;
//        }
//      }
//      stop();
//      delay(500);     
//      myservo.write(RIGHT_ANGLE);          
//      delay(1000);      
//      Serial.println("Testing right");
//      rightDistance = Distance_test();
//      
//      delay(500);
//      myservo.write(MIDDLE_ANGLE);              
//      delay(1000);                                                  
//      myservo.write(LEFT_ANGLE);              
//      delay(1000); 
//      Serial.println("Testing left");
//      leftDistance = Distance_test();
//      
//      delay(500);
//      myservo.write(90);              
//      delay(1000);
//      if(rightDistance > leftDistance) {
//        rotateRight(); 
//      }
//      else if(rightDistance < leftDistance) {
//        rotateLeft(); 
//      }
//      else if((rightDistance <= 20) || (leftDistance <= 20)) {
//        back();
//        delay(180);
//      }
//      else {
//        forward();
//      }
//    } else {
      if(LT_M){
        forward();
      }
      else if(LT_R) { 
        rotateRight_LineTrace();                            
      }   
      else if(LT_L) {
        rotateLeft_LineTrace();  
      }
//    }                     
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

void setBrakeLights(CRGB color) {
  for(int i = 6; i < 13; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void setTurnSignal(CRGB left, CRGB right) {
  for(int i = 6; i< 8; i++) {
    leds[i] = left;
  }

  for(int i = 10; i < 13; i++) {
    leds[i] = right;
  }
  FastLED.show();
}
