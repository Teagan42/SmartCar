//www.elegoo.com

#include <FastLED.h>
#include "Car.h"
#include "Sonar.h"
//#include "LineTracking.h"

#define LOOP_LIMIT 40

#define LT_R !digitalRead(3)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

#define STAGE_NONE 0
#define STAGE_INITIAL 1
#define STAGE_1 2
#define STAGE_2 3
#define STAGE_3 4

struct DistanceAngle {
  int angle = 0;
  int distance = 0;

  DistanceAngle(int angle, int distance) {
    this->angle = angle;
    this->distance = distance;
  }
};

enum Direction {
  LEFT,
  RIGHT,
  FORWARD,
  BACKWARD,
  NOT_FOUND
};

Direction lastDirection;
int avoidanceStage = STAGE_NONE;
int lastDistance = 0;
Direction avoidanceDirection = NOT_FOUND;

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
  // Go forward, scan opposite direction (Left->Right angle)
  // When opposite direction > 30: rotate opposite direction (Left->Right)
  // Turn on line tracking
  // Go forward, scan opposite direction (Left->Right Angle)
  // When opposite direction > 30: rotate opposite direction (Left->Right)
  // Go until line?
  Serial.print("STAGE ");
  Serial.println(avoidanceStage);
  switch (avoidanceStage) {
    case STAGE_NONE:
      traceLine();
      if (sonar.pingDistance(MIDDLE_ANGLE) < 12) {
        avoidanceStage = STAGE_INITIAL;
        stop();
      }
      break;
    case STAGE_INITIAL:
      stop();
      avoidanceDirection = getAvoidanceDirection();
      switch (avoidanceDirection) {
        case LEFT:
        case RIGHT:
          Serial.println("Found avoidance, rotating");
          setSpeed(255);
          rotate90Degrees(getOppositeDirection(avoidanceDirection));
          avoidanceStage = STAGE_1;
          break;
        case NOT_FOUND:
          Serial.println("Avoidance not found, taking drastic measures");
          // Rotate left & right to figure it out
          rotate90Degrees(LEFT);
          if (sonar.pingDistance(RIGHT_ANGLE) > 20) {
            avoidanceStage = STAGE_1;
            avoidanceDirection = LEFT;
            setSpeed(175);
            backward();
            delay(1000);
          } else {
            rotate90Degrees(RIGHT);
            rotate90Degrees(RIGHT);
            if (sonar.pingDistance(LEFT_ANGLE) > 20) {
              avoidanceStage = STAGE_1;
              avoidanceDirection = RIGHT;
            } else {
              Serial.println("CONFUSED!");
              backward();
              delay(100);
            }
          }
          break;
      }
      break;
    case STAGE_1:
    case STAGE_2:
      setSpeed(175);
      if (sonar.pingDistance(MIDDLE_ANGLE) < 20) {
        Serial.println("WALL!");
        stop(); // TODO:
      } else {
        Serial.println("Creeping forward");
        backward();
      }
      int distance = 0;
      Direction rotateDirection = NOT_FOUND;
      switch (avoidanceDirection) {
        case LEFT:
          distance = sonar.pingDistance(RIGHT_ANGLE);
          rotateDirection = RIGHT;
          break;
        case RIGHT:
          distance = sonar.pingDistance(LEFT_ANGLE);
          rotateDirection = LEFT;
          break;
        default:
          Serial.println("UNKNOWN ANGLE");
          break;
      }

      Serial.print("Distance: ");
      Serial.println(distance);

      if (distance > 60) {
        delay(200);
        setSpeed(255);
        avoidanceStage += 1;
        if (avoidanceStage == STAGE_3) {
          setSpeed(255);
          rotate90Degrees(rotateDirection);
          attachInterrupt(digitalPinToInterrupt(2), callbackLeftTrace, LOW);
          attachInterrupt(digitalPinToInterrupt(3), callbackRightTrace, LOW);
        } else {
          rotate90Degrees(getOppositeDirection(rotateDirection));
        }
        backward();
        delay(1000);
      }
      break;
    case STAGE_3:
      setSpeed(125);
      if (sonar.pingDistance(MIDDLE_ANGLE) < 20) {
        Serial.println("WALL!");
        stop();
      } else {
        Serial.println("Creeping forward");
        backward();
      }
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

void rotate90Degrees(Direction dir) {
  stop();
  setSpeed(255);
  int sonarAngle = MIDDLE_ANGLE;
  while (true) {
    switch (dir) {
      case LEFT:
        sonarAngle = RIGHT_ANGLE;
        rotateLeft();
        break;
      case RIGHT:
        sonarAngle = LEFT_ANGLE;
        rotateRight();
        break;
      default:
        return;
    }

    int distance = sonar.pingDistance(RIGHT_ANGLE);
    if (lastDistance - distance > 1) {
      break;
    }

    lastDistance = distance;
  }
  delay(575);
  stop();
}

Direction getAvoidanceDirection() {
  DistanceAngle minDistanceAngle(0, 10000);
  DistanceAngle maxDistanceAngle(0, -1);
  for (int i = LEFT_ANGLE; i < RIGHT_ANGLE; i += 20) {
    int distance = sonar.pingDistance(i);
    if (distance < minDistanceAngle.distance) {
      minDistanceAngle = DistanceAngle(i, distance);
    } else if (distance > maxDistanceAngle.distance) {
      maxDistanceAngle = DistanceAngle(i, distance);
    }
    delay(200);
  }

  if (abs(maxDistanceAngle.distance - minDistanceAngle.distance) > 5) {
    return NOT_FOUND;
  }

  if (maxDistanceAngle.angle < 90) {
    return LEFT;
  } else {
    return RIGHT;
  }
}

void callbackLeftTrace() {
  avoidanceStage = STAGE_NONE;
  Serial.println("Interrupt Left");
  detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
  rotate90Degrees(getOppositeDirection(avoidanceDirection));
  avoidanceStage = STAGE_INITIAL;
  turnLeft();
}

void callbackRightTrace() {
  avoidanceStage = STAGE_NONE;
  Serial.println("Interrupt Right");
  
  detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
  rotate90Degrees(avoidanceDirection);
  avoidanceStage = STAGE_NONE;
  turnRight();
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
    rotateRight();
    return; // TODO:
  } else if (LT_M && LT_R) {
    setSpeed(225);
    rotateRight();
    lastDirection = RIGHT;
  } else if (LT_M && LT_L) {
    setSpeed(225);
    rotateLeft();
    lastDirection = LEFT;
  } else if (LT_M) {
    setSpeed(150);
    forward();
    lastDirection = FORWARD;
  }
  else if (LT_R) {
    setSpeed(225);
    turnRight();
    lastDirection = RIGHT;
  } else if (LT_L) {
    setSpeed(225);
    turnLeft();
    lastDirection = LEFT;
  }
}
