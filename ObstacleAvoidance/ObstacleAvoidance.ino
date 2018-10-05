//www.elegoo.com

#include <FastLED.h>
#include "Car.h"
#include "Sonar.h"
//#include "LineTracking.h"

#define LOOP_LIMIT 40

#define LT_R !digitalRead(3)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

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
  NOT_FOUND
};

enum AvoidanceStage {
  NONE,
  INITIAL, // Find direction
  FIRST, // After first turn
  SECOND, // After second turn
  THIRD, // After third turn
};

Direction lastDirection;
AvoidanceStage avoidanceStage = NONE;
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
  FastLED.clear();



  // Go forward, scan opposite direction (Left->Right angle)
  // When opposite direction > 30: rotate opposite direction (Left->Right)
  // Turn on line tracking
  // Go forward, scan opposite direction (Left->Right Angle)
  // When opposite direction > 30: rotate opposite direction (Left->Right)
  // Go until line?
  switch (avoidanceStage) {
    case NONE:
      traceLine();
      if (sonar.pingDistance() < 20) {
        avoidanceStage = INITIAL;
        stop();
      }
      break;
    case INITIAL:
      stop();
      avoidanceDirection = getAvoidanceDirection();
      switch (avoidanceDirection) {
        case LEFT:
        case RIGHT:
          Serial.println("Found avoidance, rotating");
          setSpeed(255);
          rotate90Degrees(avoidanceDirection);
          avoidanceStage = FIRST;
          break;
        case NOT_FOUND:
          Serial.println("Avoidance not found, taking drastic measures");
          // Rotate left & right to figure it out
          rotate90Degrees(LEFT);
          if (sonar.pingDistance(RIGHT_ANGLE) > 20) {
            avoidanceStage = FIRST;
            avoidanceDirection = LEFT;
          } else {
            rotate90Degrees(RIGHT);
            rotate90Degrees(RIGHT);
            if (sonar.pingDistance(LEFT_ANGLE) > 20) {
              avoidanceStage = FIRST;
              avoidanceDirection = RIGHT;
            } else {
              Serial.println("CONFUSED!");
              backward();
              delay(10);
            }
          }
          break;
      }
      break;
    case FIRST:
    case SECOND:
      setSpeed(175);
      if (sonar.pingDistance(MIDDLE_ANGLE) < 20) {
        Serial.println("WALL!");
        stop();
      } else {
        Serial.println("Creeping forward");
        forward();
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
          rotateDirection = RIGHT;
          break;
        default:
          Serial.println("UNKNOWN ANGLE");
          break;
      }
      delay(max(4.5*distance, 700));

      Serial.print("FIRST/SECOND Distance: ");
      Serial.println(distance);

      if (distance > 25) {
          setSpeed(255);
          rotate90Degrees(rotateDirection);
          if (avoidanceStage == FIRST) {
            avoidanceStage = SECOND;
          } else {
            avoidanceStage = THIRD;
          }
      }
      break;
    case THIRD:
      attachInterrupt(digitalPinToInterrupt(2), callbackLeftTrace, LOW);
      attachInterrupt(digitalPinToInterrupt(3), callbackRightTrace, LOW);
      setSpeed(125);
      if (sonar.pingDistance(MIDDLE_ANGLE) < 20) {
        Serial.println("WALL!");
        stop();
      } else {
        Serial.println("Creeping forward");
        forward();
      }
      delay(100);
  }
}

void rotate90Degrees(Direction dir) {
  stop();
  setSpeed(255);
  switch (dir) {
    case LEFT: rotateLeft(); break;
    case RIGHT: rotateRight(); break;
    default:
      return;
  }
  delay(450);
  stop();
}

Direction getAvoidanceDirection() {
  DistanceAngle minDistanceAngle(0, 10000);
  DistanceAngle maxDistanceAngle(0, -1);
  for (int i = LEFT_ANGLE; i < RIGHT_ANGLE; i += 5) {
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
  avoidanceStage = NONE;
  Serial.println("Interrupt Left");
  detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
  turnLeft();
}

void callbackRightTrace() {
  avoidanceStage = NONE;
  Serial.println("Interrupt Right");
  detachInterrupt(digitalPinToInterrupt(3));
  detachInterrupt(digitalPinToInterrupt(2));
  turnRight();
}

void traceLine() {
  if (!LT_M && !LT_R && !LT_L) {
    setSpeed(175);
    switch (lastDirection) {
      case FORWARD:
        backward();
        break;
      case LEFT:
        rotateLeft();
        //        lastDirection = LEFT;
        break;
      case RIGHT:
        rotateRight();
        //        lastDirection = RIGHT;
        break;
    }
  } else if (LT_M && LT_R && LT_L) {
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
