#include "LineTracking.h"

LineTrackingThread::LineTrackingThread(void (*callback)(Direction dir)) {
  this->callback = callback;
}

void LineTrackingThread::run() {
  Serial.println("Tracking Line");
  if (!LT_M && !LT_R && !LT_L) {
    callback(NOT_FOUND);
  } else if (LT_M && LT_R && LT_L) {
    callback(PERPENDICULAR);
  } else if (LT_M) {
    callback(FORWARD);
  } else if (LT_R) {
    callback(RIGHT);
  } else if (LT_L) {
    callback(LEFT);
  }

  runned();
}
