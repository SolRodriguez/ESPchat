#ifndef Chat_Client_h
#define Chat_Client_h
#include "Arduino.h"

//original from Camera.h
#include <Wire.h>
#include <ArduCAM.h>
#include "memorysaver.h"

//class from button.h file 
class Button
{
  public:
  uint32_t t_of_state_2;
  uint32_t t_of_button_change;    
  uint32_t debounce_time;
  uint32_t long_press_time;
  uint8_t pin;
  uint8_t flag;
  bool button_pressed;
  uint8_t state;
  
  Button(int p);
  void read();
  int update();
};

//class from camera.h
class Camera 
{
  private:
  const int CS = 17;
  ArduCAM arduCam = ArduCAM(OV2640, CS);
  uint8_t vid;
  uint8_t pid;

  static const size_t imgSize = (80*60);
  char img_data[imgSize];

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

//class from RequestSender
class RequestSender
{
  private:
  int response_timeout = 6000;
  const static int out_buffer_size = 1000;
  char response[out_buffer_size];

  char host[30];
  char username[30];
  char destination[100];

  char request_header[400];

  public:
  RequestSender();
  void begin_wifi(char* network, char* password) ;
  void set_host(char* host_);
  void set_destination(char* dest);
  void send_video(char* img_ptr, char* audio_ptr);
  void set_username(char* username_);


  private:
  uint8_t char_append(char* buff, char c, uint16_t buff_size);
  void generate_video_request_header(char* img_ptr, char* audio_ptr);
};
#endif
