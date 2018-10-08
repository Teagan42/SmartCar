//www.elegoo.com

#include <FastLED.h>
#include "Car.h"
#include "Sonar.h"

#define LOOP_LIMIT 40

// Uncomment if white tape on black floor
//#define WHITE_ON_BLACK

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
int forwardAvoidanceCount = 0;
int scanAngle;
int tooFarCycles = 0;

void callbackRightTrace();
void callbackLeftTrace();

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
      forward();
      if (sonar.pingDistance(MIDDLE_ANGLE) < OBJECT_DISTANCE) {
        avoidanceStage = STAGE_INITIAL;
        stop();
      }
      break;
    case STAGE_INITIAL:
      stop();
      avoidanceDirection = getAvoidanceDirection();
      Serial.println("Found obstacle, rotating");
      avoidanceStage += 1;
      backward();
      delay(200);
      lastDistance = sonar.pingDistance(MIDDLE_ANGLE);
      rotateToward(avoidanceDirection, 8);
      delay(150);
      forwardAvoidanceCount = 0;
      break;
    case STAGE_2:
      switch (avoidanceDirection) {
        case LEFT:
          attachInterrupt(digitalPinToInterrupt(2), callbackLeftTrace, LT_INTERRUPT);
          break;
        case RIGHT:
          attachInterrupt(digitalPinToInterrupt(3), callbackRightTrace, LT_INTERRUPT);
          break;
      }
      avoidanceStage += 1;
    case STAGE_1:
      if (forwardAvoidanceCount > -1) {
        avoidanceStage += 1;
        forwardAvoidanceCount = 0;
      }
    case STAGE_3:
      switch (avoidanceDirection) {
        case LEFT:
          if (LT_L) {
            callbackLeftTrace();
            return;
          }
          break;
          break;
        case RIGHT:
          if (LT_R) {
            callbackRightTrace();
            return;
          }
          break;
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
  return Right;
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

  distance = min(sonar.pingDistance(scanAngle), sonar.pingDistance(directionToExtremeAngle(oppositeDirection)));

  bool isWayTooFar = distance > maximumPreferredDistance;
  bool isTooFar = distance > distanceToMaintain;
  bool isTooClose = distance < distanceToMaintain;
  bool isWayTooClose = distance < minimumSafeDistance;
  bool isReasonableDistance = !isWayTooFar && !isWayTooClose && abs(distance - distanceToMaintain) < 3;

  delta = lastDistance - distance;
  if (isWayTooClose) {
    scanAngle = directionToAngle(oppositeDirection);
    tooFarCycles = 0;
    setSpeed(255);
    int now = millis();
    backward();
    while (millis() - now < 500) {
      if (sonar.pingDistance(scanAngle) > distanceToMaintain) {
        break;
      }
    }
    turnToward(avoidanceDirection, 4);
    delay(150);
    setTurnSignal(CRGB(255,0,0), CRGB(0,0,0));
  } else if (isWayTooFar) {
    tooFarCycles += 1;
    setSpeed(255);
    turnToward(oppositeDirection, 8);
    scanAngle = directionToAcuteAngle(oppositeDirection);
    forwardAvoidanceCount += 1;
  } else if (isReasonableDistance) {
    tooFarCycles = 0;
    if (isTooClose) {
      scanAngle = directionToExtremeAngle(oppositeDirection);
      if (getCurrentRotation() != avoidanceDirection) {
        rotateToward(avoidanceDirection, 3);
      }
    } else if (isTooFar) {
      scanAngle = directionToAngle(oppositeDirection);
      if (getCurrentRotation() != oppositeDirection) {
        turnToward(oppositeDirection, 3);
      }
    } 
    forwardAvoidanceCount += 1;
  }
  lastDistance = distance;
}

void traceLine() {
  if (!LT_M && !LT_R && !LT_L) {
    setSpeed(175);
    switch (lastDirection) {
      //      case FORWARD:
      //        backward();
      //        break;
      case LEFT:
        rotateLeft();
        //        lastDirection = LEFT;
        break;
      case RIGHT:
        rotateRight();
        //        lastDirection = RIGHT;
        break;
    }
  } else if (LT_M && LT_R && LT_L) { // Perpendicular
    setSpeed(225);
    rotateRight(1);
    return; // TODO:
  } else if (LT_M && LT_R) {
    setSpeed(225);
    rotateRight(1);
    lastDirection = RIGHT;
  } else if (LT_M && LT_L) {
    setSpeed(225);
    rotateLeft(1);
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
  avoidanceStage = STAGE_NONE;
  stop();
  Serial.println("Interrupt Left");
  detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
  rotateToward(avoidanceDirection, 1);
  while(!LT_M);
}

void callbackRightTrace() {
  avoidanceStage = STAGE_NONE;
  stop();
  detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
  rotateToward(avoidanceDirection, 1);
  while(!LT_M);
}
