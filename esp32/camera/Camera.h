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

  static const size_t imgSize = (80*60);
  uint8_t img_data[imgSize];

  uint8_t temp = 0;
  uint8_t temp_last = 0;
  bool is_header = false;

  public:
  Camera();
  void setup();
  uint8_t* get_image();

  private:
  void start_capture();
};
#endif
