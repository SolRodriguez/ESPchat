#ifndef espchat_h 
#define espchat_h
#include <Wire.h>
#include <ArduCAM.h>
#include "memorysaver.h"
#include "Arduino.h"

// Camera, Button, Microphone, RequestSender
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


class Microphone{
  private:
    int pin;
    uint32_t sr;
    uint16_t read_value;
    uint8_t * buff;
    bool recording = false;
    uint32_t time_since_sample;
    int sample_num;
    float time_between_samples;
    int samp_len;
  public:
    Microphone(int p, int sample_rate, uint8_t * audio_buffer);
    uint16_t rawRead();
    uint8_t read();
    void record_all(int sample_length);
    void start_recording(int sample_length);
    bool on_update();
    void on_finish();
};


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
  //char user_name[10];
  
  public:
    RequestSender();
    void begin_wifi(char* network, char* password) ;
    void set_host(char* host_);
    void set_destination(char* dest);
    void send_video(char* img_ptr, char* audio_ptr);
    void set_username(char* username_);

    void get_users(char* users_available); 
    void download_data(char* user_name, char* download_user_data);
  private:
    uint8_t char_append(char* buff, char c, uint16_t buff_size);
    void generate_video_request_header(char* img_ptr, char* audio_ptr);
};

#endif
