#ifndef requestSender_h
#define requestSender_h
#include "Arduino.h"
class RequestSender
{
  public:
  RequestSender();
  void begin_wifi(char* network, char* password) ;
  uint8_t char_append(char* buff, char c, uint16_t buff_size);
  void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint16_t out_buffer_size, uint8_t serial);
};
#endif
