#ifndef requestSender_h
#define requestSender_h
#include "Arduino.h"
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
