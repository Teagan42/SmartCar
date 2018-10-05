#ifndef __LINETRACKING_H__
#define __LINETRACKING_H__

#include <TimerOne.h>
#include <Thread.h>
#include <ThreadController.h>

#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

enum Direction {
  LEFT,
  RIGHT,
  FORWARD,
  PERPENDICULAR,
  NOT_FOUND
};

class LineTrackingThread: public Thread {
  public:
    LineTrackingThread(void (*callback)(Direction dir));
    void run();

  private:
    void (*callback)(Direction dir);
};

#endif
