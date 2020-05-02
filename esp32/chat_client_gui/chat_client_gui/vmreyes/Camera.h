#ifndef camera_h 
#define camera_h
#include <Wire.h>
#include <ArduCAM.h>
#include "memorysaver.h"

class Camera 
{
  private:
  const int CS = 17;
  ArduCAM arduCam = ArduCAM(OV2640, CS);
  uint8_t vid;
  uint8_t pid;

  uint8_t temp = 0;

  public:
  Camera();
  void setup();
  void capture();

  private:
  void start_capture();
};
#endif
