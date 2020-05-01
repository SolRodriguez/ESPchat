#ifndef CameraEspchat_h 
#define CameraEspchat_h

//original from Camera.h
#include <Wire.h>
#include <ArduCAM.h>
#include "memorysaver.h"


//original from RequestSender.h
#include "Arduino.h"

//class from microphone class
class Microphone{
  public:
  int pin;
  uint32_t sr;
  uint16_t read_value;
  uint8_t * buff;
  bool recording = false;
  uint32_t time_since_sample;
  int sample_num;
  float time_between_samples;
  int samp_len;

  Microphone(int p, int sample_rate, uint8_t * audio_buffer){
    pin = p;
    sr = sample_rate;
    time_between_samples = 1000000/sr;
    buff = audio_buffer;
  }

  void rawRead(){
    read_value = analogRead(pin);
  }

  int read(){
    // converts 12-bit to 8-bit
    read_value = analogRead(pin) >> 4;
    return read_value;
  }

  void record_all(int sample_length){
    // The current, blocking method of recording
    samp_len = sample_length;
    memset(buff, 0, sizeof(buff));
    sample_num = 0;
    while (sample_num < samp_len) {
      buff[sample_num] = read();
      sample_num++;
      while (micros() - time_since_sample <= time_between_samples); //blocking
      time_since_sample = micros();
    }
  }

  void start_recording(int sample_length){
    if (recording) return;
    Serial.println("Start recording");
    samp_len = sample_length;
    recording = true;
    memset(buff, 0, sizeof(buff));
    sample_num = 0;
  }

  bool on_update(){
    // called as often as possible ; non--blocking alternative
    // returns true when the recording is finished
    if (!recording) return false;
    if (sample_num > samp_len){
      on_finish();
      return true;
    }
    if (micros() - time_since_sample >= time_between_samples){
      buff[sample_num] = read();
      sample_num++;
      time_since_sample = micros();
    }
    return false; 
  }

  void on_finish(){
    Serial.println("Done recording");
    recording = false;
  }
  
};




//class from camera.h
class Camera 
{
  private:
  const int CS = 17;
  ArduCAM arduCam = ArduCAM(OV2640, CS);
  uint8_t vid;
  uint8_t pid;

  static const size_t imgSize = (240*320);
  char img_data[imgSize];

  uint8_t temp = 0;
  uint8_t temp_last = 0;
  bool is_header = false;

  public:
  Camera();
  void setup();
  char* get_image();

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
