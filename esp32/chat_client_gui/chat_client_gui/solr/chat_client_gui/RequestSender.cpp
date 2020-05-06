#include "Arduino.h"
#include "requestSender.h"
#include <WiFi.h>

RequestSender::RequestSender(){
  
}

void RequestSender::begin_wifi(char* network, char* password) {
  WiFi.begin(network, password);
  uint8_t count = 0;
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) {
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else {
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart();
  }
}

void RequestSender::set_host(char* host_){
  sprintf(host, "%s", host_);
}

void RequestSender::set_destination(char* dest){
  sprintf(destination, "%s", dest);
}

void RequestSender::set_username(char* username_){
  sprintf(username, "%s", username_);
}

uint8_t RequestSender::char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len>buff_size) return false;
  buff[len] = c;
  buff[len+1] = '\0';
  return true;
}

void RequestSender::send_video(char* img_ptr, char* audio_ptr) {
  bool serial = true;
  int response_size = out_buffer_size;
  char intermediate1[25];
  sprintf(intermediate1, "user=%s&img=", username);

  char intermediate2[] = "&audio=";


  WiFiClient client; //instantiate a client object

  generate_video_request_header(img_ptr, audio_ptr);

  if (client.connect(host, 80)) { //try to connect to host on port 80
    Serial.print(request_header);//Can do one-line if statements in C without curly braces
    Serial.print(intermediate1);
    Serial.print(img_ptr);
    Serial.print(intermediate2);
    Serial.print(audio_ptr);

    client.print(request_header);
    client.print(intermediate1);
    client.print(img_ptr);
    client.print(intermediate2);
    client.print(audio_ptr);

    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response,client.read(), out_buffer_size);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}

void RequestSender::generate_video_request_header(char* img_ptr, char* audio_ptr) {
  char intermediate1[25];
  sprintf(intermediate1, "user=%s&img=", username);

  char intermediate2[] = "&audio=";

  sprintf(request_header, "POST %s HTTP/1.1\r\n", destination);
  sprintf(request_header + strlen(request_header), "Host: %s\r\n", host);
  strcat(request_header, "Content-Type: text/plain\r\n");
  sprintf(request_header + strlen(request_header), "Content-Length: %d\r\n\r\n",
          strlen(intermediate1) + strlen(img_ptr) + strlen(intermediate2) + strlen(audio_ptr));
}

//void RequestSender::menu_request(){}
