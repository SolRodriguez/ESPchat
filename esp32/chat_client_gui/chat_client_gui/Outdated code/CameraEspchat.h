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
    Microphone();
    void Microphone_setup(int p, int sample_rate, uint8_t * audio_buffer);
    uint16_t rawRead();
    uint8_t read();
    void record_all(int sample_length);
    void start_recording(int sample_length);
    bool on_update();
    void on_finish();
};


//class from camera.h
class Camera
{
  private:
    const int CS = 17;
    ArduCAM arduCam = ArduCAM(OV2640, CS);
    uint8_t vid;
    uint8_t pid;

    static const size_t imgSize = (240 * 320);
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
