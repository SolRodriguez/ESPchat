#include "Arduino.h"
#include "requestSender.h"
#include <WiFi.h>
#include "bsupport_functions.h"


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
  // char test[5];
  // base64_decode(test, "YWJj",4);
  // Serial.println('buffer1');
  // Serial.println(test);
  // Serial.println('buffer1');
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

void RequestSender::get_video(char* username, uint8_t* video, uint8_t* audio) {
  bool serial = true;
  int response_size = out_buffer_size;

  WiFiClient client; //instantiate a client object

  generate_get_video_response_header(username);

  if (client.connect(host, 80)) { //try to connect to host on port 80
    Serial.print(request_header);//Can do one-line if statements in C without curly braces

    client.print(request_header);

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
    int video_index = 0;
    int audio_index = 0;
    int general_index = 0;
    bool switch_arrays = false;
    char holder[4];
    char sample[3];
    char read_val;
    while (client.available()) { //read out remaining text (body of response)
    Serial.println("LINE 145");
    // ('
      for(uint8_t i = 0; i < 2; i++){
        Serial.println((char) client.read());
      }
      Serial.print("\n");
    Serial.println("LINE 150");
    // image_data
    bool storing = true;
    while(storing){
      for(uint8_t i = 0; i < 4; i++){
        read_val = (char) client.read();
        delayMicroseconds(10);
        if(read_val == '\''){
          storing = false;
          break;
        }
        holder[i] = read_val;
      }
      if(storing){
        base64_decode(sample, holder, 4);
        Serial.println(holder);
        for(uint8_t i = 0; i < 3; i++){
          video[video_index] = (uint8_t) sample[i];
          video_index++;
        }
      }
    }

      // ,\n'
      while(client.read() != '\'');

      Serial.println("LINE 185");
      bool storing_a = true;
      while(storing_a){
        for(uint8_t i = 0; i < 4; i++){
          read_val = (char) client.read();
          delayMicroseconds(10);
          if(read_val == '\''){
            storing_a = false;
            break;
          }
          holder[i] =  read_val;
        }
        if(storing_a){
          base64_decode(sample, holder, 4);
          Serial.println((char*) holder);
          for(uint8_t i = 0; i < 3; i++){
            audio[audio_index] = (uint8_t) sample[i];
            audio_index++;
          }
        }
      }
      Serial.println("LINE 205");
      // ')
      break;
    }
    client.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}

char RequestSender::get_users(char* users_available) {
  int response_size = out_buffer_size;
  int response_timeout = 6000;
  bool serial = true;
  char request_header[400];
  //for reference  myRequest.set_destination("/sandbox/sc/team044/espchat/server/espchat.py");

  sprintf(request_header, "GET %s?menu=%s HTTP/1.1\r\n", destination, "True");
  sprintf(request_header + strlen(request_header), "Host: %s\r\n", host);
  strcat(request_header, "\r\n"); //add blank line!

  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request_header);//Can do one-line if statements in C without curly braces
    client.print(request_header);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), response_size);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection FAILED :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
  memset(users_available, NULL, 60);
  strncpy(users_available, response, 60);
}


void RequestSender::generate_get_video_response_header(char* other_user) {
  //char intermediate1[25];
  //sprintf(intermediate1, "user=%s&img=", username);

  //char intermediate2[] = "&audio=";

  sprintf(request_header, "GET /sandbox/sc/team044/espchat/server/espchat.py?menu=False&user=%s HTTP/1.1\r\n", other_user);
  sprintf(request_header + strlen(request_header), "Host: %s\r\n", host);
  strcat(request_header, "\r\n");
  //strcat(request_header, "Content-Type: text/plain\r\n");
  //sprintf(request_header + strlen(request_header), "Content-Length: %d\r\n\r\n",
          //strlen(intermediate1) + strlen(img_ptr) + strlen(intermediate2) + strlen(audio_ptr));
}
