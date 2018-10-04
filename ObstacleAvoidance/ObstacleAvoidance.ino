//www.elegoo.com

#include <Servo.h>  //servo library
#include <NewPing.h>
#include <FastLED.h>
Servo myservo;      // create servo object to control servo

//Line Tracking IO define
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

int Echo = A4;  
int Trig = A5; 

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
#define LOOP_LIMIT 40
#define SCAN_INTERVAL 5
#define SCAN_INTERVAL_RADIANS SCAN_INTERVAL * 3.14159 / 180
#define SCAN_NUM (RIGHT_ANGLE - LEFT_ANGLE) / SCAN_INTERVAL + 1

int rightDistance = 0, leftDistance = 0, middleDistance = 0;

int distanceScan[SCAN_NUM];
int nextScanIndex = 0;

NewPing sonar(Trig, Echo, 5000);
CRGB leds[NUM_LEDS];

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

void back() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Back");
}

void left() {
  analogWrite(ENA, carSpeed/2);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH); 
  Serial.println("Left");
  setTurnSignal(CRGB(255,255,0), CRGB(0, 0, 0));
}

void turnLeft() {
  digitalWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed/4);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH); 
  Serial.println("Left");
  setTurnSignal(CRGB(255,255,0), CRGB(0, 0, 0));
}

void right() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed/2);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Right");
  setTurnSignal(CRGB(0,0,0), CRGB(255, 255, 0));
}

void turnRight() {
  digitalWrite(ENA, carSpeed/4);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Right");
  setTurnSignal(CRGB(0,0,0), CRGB(255, 255, 0));
}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  Serial.println("Stop!");
  setBrakeLights(CRGB(255,0,0));
} 

//Ultrasonic distance measurement Sub function
int Distance_test() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);   
  float Fdistance = pulseIn(Echo, HIGH);  
//  float Fdistance = sonar.ping_cm();
//  Serial.println("Distance: ");
//  Serial.println(Fdistance);
  Fdistance= Fdistance / 58;       
  return (int)Fdistance;
}

int scanIndexToAngle(int scanIndex) {
  return scanIndex * SCAN_INTERVAL + LEFT_ANGLE;
}

int scanIndexLength(int startScanIndex, int endScanIndex) {
  int degreeStart = scanIndexToAngle(startScanIndex);
  int degreeEnd = scanIndexToAngle(endScanIndex);
  float avgDistance = 0;
  for (int i = startScanIndex; i <= endScanIndex; i++) {
    avgDistance += distanceScan[i];
  }
  avgDistance /= (endScanIndex - (startScanIndex + 1));
  float angle = (degreeEnd - degreeStart) * 3.14159 /180;
  return tan(angle) * avgDistance;
}

void fullScan() {
  for(int i = 0; i < SCAN_NUM; i++) {
    int angle = scanIndexToAngle(i);
    myservo.write(angle);
    delay(200);
    distanceScan[i] = Distance_test();//sonar.ping_cm();
    Serial.print(angle);
    Serial.print(": ");
    Serial.println(distanceScan[i]);
    delay(400);
  }
  myservo.write(MIDDLE_ANGLE);
}

//void scanNext() {
//  int angle = scanIndexToAngle(nextScanIndex);
//  myservo.write(angle);
//  delay(200);
//  distanceScan[nextScanIndex++] = Distance_test();//sonar.ping_cm();
//}

void setup() { 
  myservo.attach(3);  // attach servo on pin 3 to servo object
  Serial.begin(9600);     
  pinMode(Echo, INPUT);    
  pinMode(Trig, OUTPUT);  
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
  fullScan();
} 

void loop() { 
  FastLED.clear();
    myservo.write(MIDDLE_ANGLE);  //setservo position according to scaled value
    delay(30); 
    Serial.println("Testing middle");
    middleDistance = Distance_test();

    if(middleDistance <= 20) {   
      Serial.println("Obstacle!");
      stop();
      fullScan();
      for (int i = 1; i < SCAN_NUM/2 - 1; i++) {
        int distanceLeft = distanceScan[90-i];
        int distanceRight = distanceScan[90+i];
        if (distanceLeft <= 25 || distanceRight <= 25) {
          continue;
        } else if (distanceLeft < distanceRight) {
          turnRight();
          delay(50);
          stop();
          break;
        } else if (distanceLeft > distanceRight) {
          turnLeft();
          delay(50);
          stop();
          break;
        }
      }
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
    } else {
      if(LT_M){
        forward();
      }
      else if(LT_R) { 
        rotateRight();                            
      }   
      else if(LT_L) {
        rotateLeft();  
      }
    }                     
}

void rotateRight() {
  right();
  int time = micros();
  while(LT_R && micros() - time < LOOP_LIMIT); 
}

void rotateLeft() {
  left();
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
