//www.elegoo.com

#include <FastLED.h>
#include "Car.h"
#include "Sonar.h"
//#include "LineTracking.h"

#define LOOP_LIMIT 40

// Uncomment if white tape on black floor
#define WHITE_ON_BLACK

#ifndef WHITE_ON_BLACK

  #define LT_R !digitalRead(3)
  #define LT_M !digitalRead(4)
  #define LT_L !digitalRead(2)

  #define LT_INTERRUPT LOW

#else

  #define LT_R digitalRead(3)
  #define LT_M digitalRead(4)
  #define LT_L digitalRead(2)

  #define LT_INTERRUPT HIGH
#endif

#define STAGE_NONE 0
#define STAGE_INITIAL 1
#define STAGE_1 2
#define STAGE_2 3
#define STAGE_3 4

#define OBJECT_DISTANCE 20

#define RIGHT_EXTREME_ANGLE 180 //10
#define LEFT_EXTREME_ANGLE 0 //180
#define RIGHT_ANGLE 150 //10
#define LEFT_ANGLE 30 //180

struct DistanceAngle {
  int angle = 0;
  int distance = 0;

  DistanceAngle(int angle, int distance) {
    this->angle = angle;
    this->distance = distance;
  }
};

Direction lastDirection;
int avoidanceStage = STAGE_NONE;
int lastDistance = 0;
Direction avoidanceDirection = NOT_FOUND;
int scanAngle;
int tooFarCycles = 0;
bool shouldAttachInterrupts = false;

void callbackRightTrace();
void callbackLeftTrace();

void attachInterrupts() {
  switch (avoidanceDirection) {
    case LEFT:
      attachInterrupt(digitalPinToInterrupt(2), callbackLeftTrace, LT_INTERRUPT);
      break;
    case RIGHT:
      attachInterrupt(digitalPinToInterrupt(3), callbackRightTrace, LT_INTERRUPT);
      break;
  }
}

Servo servo;
SonarServo sonar(&servo);

void setup() {
  Serial.begin(9600);
  sonar.setup();
  setupCar();
  setSpeed(255);
}

void loop() {
  switch (avoidanceStage) {
    case STAGE_NONE:
      setSpeed(255);
      traceLine();
      if (sonar.pingDistance(MIDDLE_ANGLE) < OBJECT_DISTANCE) {
        avoidanceStage = STAGE_INITIAL;
        brake();
      }
      break;
    case STAGE_INITIAL:
      shouldAttachInterrupts = false;
      brake();
      Serial.print("STAGE_INITIAL");
      avoidanceDirection = getAvoidanceDirection();
      Serial.print("\tGoing ");
      Serial.println((avoidanceDirection == LEFT ? "LEFT" : "RIGHT"));
      avoidanceStage = STAGE_1;
      backward();
      delay(300);
      turnToward(avoidanceDirection, 2);
      delay(200);
      lastDistance = sonar.pingDistance(MIDDLE_ANGLE);
      break;
    case STAGE_2:
      Serial.println("STAGE_2");
      attachInterrupts();
      shouldAttachInterrupts = true;
      avoidanceStage = STAGE_1;
    case STAGE_1:
      Serial.println("STAGE_1");
        avoidanceStage += 1;
    case STAGE_3:
      Serial.println("STAGE_2");
      switch (avoidanceDirection) {
        case LEFT:
          if (LT_L) {
            callbackLeftTrace();
            return;
          }
          break;
        case RIGHT:
          if (LT_R) {
            callbackRightTrace();
            return;
          }
          break;
      }
      maintainDistanceV2(OBJECT_DISTANCE);
  }
}

Direction getOppositeDirection(Direction dir) {
  switch (dir) {
    case LEFT:
      return RIGHT;
    case RIGHT:
      return LEFT;
    case FORWARD:
      return BACKWARD;
    case BACKWARD:
      return FORWARD;
    default:
      return dir;
  }
}

Direction getAvoidanceDirection() {
  return LEFT;
//  DistanceAngle minDistanceAngle(0, 10000);
//  DistanceAngle maxDistanceAngle(0, -1);
//  for (int i = 20; i < (RIGHT_SCAN_ANGLE - LEFT_SCAN_ANGLE) / 2; i += 20) {
//    for (int j = 0; j < 2; j++) {
//      int angle = MIDDLE_ANGLE + pow(-1, j) * i;
//      int distance = sonar.pingDistance(angle);
//      if (distance < minDistanceAngle.distance) {
//        minDistanceAngle = DistanceAngle(angle, distance);
//      }
//      if (distance > maxDistanceAngle.distance) {
//        maxDistanceAngle = DistanceAngle(angle, distance);
//      }
//    }
//    if (i > 20 && abs(maxDistanceAngle.distance - minDistanceAngle.distance) > 5)  {
//      break;
//    }
//  }
//
//  if (maxDistanceAngle.angle < MIDDLE_ANGLE) {
//    Serial.println("Going around left");
//    return LEFT;
//  } else {
//    Serial.println("Going around right");
//    return RIGHT;
//  }
}

int directionToAngle(Direction dir) {
  switch (dir) {
    case LEFT:
      return LEFT_ANGLE;
    case RIGHT:
      return RIGHT_ANGLE;
    default:
      return MIDDLE_ANGLE;
  }
}

int directionToExtremeAngle(Direction dir) {
  switch (dir) {
    case LEFT:
      return LEFT_EXTREME_ANGLE;
    case RIGHT:
      return RIGHT_EXTREME_ANGLE;
    default:
      return MIDDLE_ANGLE;
  }
}

int directionToAcuteAngle(Direction dir) {
  switch (dir) {
    case LEFT:
      return (MIDDLE_ANGLE - LEFT_ANGLE) / 2 + LEFT_ANGLE;
    case RIGHT:
      return RIGHT_ANGLE - (RIGHT_ANGLE - MIDDLE_ANGLE) / 2;
    default:
      return MIDDLE_ANGLE;
  }
}

void rotateToward(Direction dir, int multiplier) {
  switch (dir) {
    case LEFT:
      rotateLeft(multiplier);
      break;
    case RIGHT:
      rotateRight(multiplier);
      break;
  }
}

void turnToward(Direction dir, int multiplier) {
  switch (dir) {
    case LEFT:
      turnLeft(multiplier);
      break;
    case RIGHT:
      turnRight(multiplier);
      break;
  }
}

void maintainDistanceV2(int distanceToMaintain) {
  int distance;
  int delta;
  int delayFor = 1;

  int angle = directionToAngle(avoidanceDirection);
  Direction oppositeDirection = getOppositeDirection(avoidanceDirection);
  int oppositeAngle = directionToAngle(oppositeDirection);
  int oppositeExtremeAngle = directionToExtremeAngle(oppositeDirection);
  int minimumSafeDistance = distanceToMaintain * .75;
  int maximumPreferredDistance = distanceToMaintain * 1.25;

  distance = min(sonar.pingDistance(scanAngle), sonar.pingDistance(oppositeExtremeAngle == scanAngle ? MIDDLE_ANGLE : oppositeExtremeAngle));

  bool isWayTooFar = distance > maximumPreferredDistance;
  bool isTooFar = distance > distanceToMaintain;
  bool isTooClose = distance < distanceToMaintain;
  bool isWayTooClose = distance < minimumSafeDistance;
  bool isReasonableDistance = !isWayTooFar && !isWayTooClose && abs(distance - distanceToMaintain) < 3;

  delta = lastDistance - distance;
  if (isWayTooClose) {
    Serial.println("WAY TOO CLOSE");
    detachInterrupt(digitalPinToInterrupt(3));
    detachInterrupt(digitalPinToInterrupt(2));
    brake();
    scanAngle = directionToAngle(oppositeDirection);
    tooFarCycles = 0;
    setSpeed(255);
    long now = millis();
    backward();
    while (millis() - now < 500) {
      if (sonar.pingDistance(scanAngle) > distanceToMaintain) {
        break;
      }
    }
    turnToward(avoidanceDirection, 4);
    if (attachInterrupts) {
      attachInterrupts();
    }
    delay(150);
    
  } else if (isWayTooFar) {
    Serial.println("WAY TOO FAR");
    tooFarCycles += 1;
    setSpeed(255);
    turnToward(oppositeDirection, 4);
    scanAngle = directionToAcuteAngle(oppositeDirection);
  } else if (isReasonableDistance) {
    tooFarCycles = 0;
    if (isTooClose) {
      Serial.println("TOO CLOSE");
      scanAngle = directionToExtremeAngle(oppositeDirection);
      if (getCurrentRotation() != avoidanceDirection) {
        rotateToward(avoidanceDirection, 3);
      }
    } else if (isTooFar) {
      Serial.println("TOO FAR");
      scanAngle = directionToAngle(oppositeDirection);
      if (getCurrentRotation() != oppositeDirection) {
        turnToward(oppositeDirection, 3);
      }
    }
  }
  lastDistance = distance;
}

void traceLine() {
  if (LT_M && LT_R) {
    setSpeed(225);
    turnRight(2);
    lastDirection = RIGHT;
  } else if (LT_M && LT_L) {
    setSpeed(225);
    turnLeft(2);
    lastDirection = LEFT;
  } else if (LT_M) {
    setSpeed(150);
    forward();
    lastDirection = FORWARD;
  }
  else if (LT_R) {
    setSpeed(225);
    turnRight(4);
    lastDirection = RIGHT;
  } else if (LT_L) {
    setSpeed(225);
    turnLeft(4);
    lastDirection = LEFT;
  }
}

void callbackLeftTrace() {
  Serial.println("Interrupt Left");
  avoidanceStage = STAGE_NONE;
  brake();
  detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
  shouldAttachInterrupts = false;
  rotateToward(avoidanceDirection, 1);
  while(!LT_M);
}

void callbackRightTrace() {
  Serial.println("Interrupt RIGHT");
  avoidanceStage = STAGE_NONE;
  brake();
  detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
  shouldAttachInterrupts = false;
  rotateToward(avoidanceDirection, 1);
  while(!LT_M);
}
