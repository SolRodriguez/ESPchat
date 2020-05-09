#include <Wire.h>
#include <ESP32WebServer.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#include "EspChat.h"
#include "arduino.h"
#include <WiFi.h>

Camera::Camera() {}

void Camera::setup() {
  pinMode(CS, OUTPUT);
  Wire.begin();
  Serial.println(F("ArduCAM Start!"));
  //SPI.begin();
  //SPI.setFrequency(4000000); //4MHz

  arduCam.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = arduCam.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55) {
    Serial.println(F("SPI1 interface Error!"));
    while (1);
  }
  arduCam.wrSensorReg8_8(0xff, 0x01);
  arduCam.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  arduCam.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))) {
    Serial.println(F("Can't find OV2640 module!"));
  } else {
    Serial.println(F("OV2640 detected."));
  }

  arduCam.set_format(BMP); // resolution 320x240
  arduCam.InitCAM();
  //arduCam.OV2640_set_JPEG_size(3);
  arduCam.clear_fifo_flag();
}

void Camera::capture() {
  start_capture();
  Serial.println(F("CAM Capturing"));

  int total_time = 0;
  total_time = millis();
  while (!arduCam.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  total_time = millis() - total_time;
  Serial.print(F("capture total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);
  total_time = 0;
  Serial.println(F("CAM Capture Done."));

  uint32_t len = arduCam.read_fifo_length();
  Serial.printf("The length of the image is %d", len);
  len -= 8; // read_fifo over reports by 8 bytes? unsure.
  Serial.printf("Fixed it to %d.", len);

  arduCam.CS_LOW();
  arduCam.set_fifo_burst();
}

void Camera::start_capture() {
  arduCam.flush_fifo();
  arduCam.clear_fifo_flag();
  arduCam.start_capture();
}




Microphone::Microphone(int p, int sample_rate, uint8_t * audio_buffer) {
  pin = p;
  sr = sample_rate;
  time_between_samples = 1000000 / sr;
  buff = audio_buffer;
}

uint16_t Microphone::rawRead() {
  read_value = analogRead(pin);
  return read_value;
}

uint8_t Microphone::read() {
  // converts 12-bit to 8-bit
  read_value = analogRead(pin) >> 4;
  return read_value;
}

void Microphone::record_all(int sample_length) {
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

void Microphone::start_recording(int sample_length) {
  if (recording) return;
  Serial.println("Start recording");
  samp_len = sample_length;
  recording = true;
  memset(buff, 0, sizeof(buff));
  sample_num = 0;
}

bool Microphone::on_update() {
  // called as often as possible ; non--blocking alternative
  // returns true when the recording is finished
  if (!recording) return false;
  if (sample_num > samp_len) {
    on_finish();
    return true;
  }
  if (micros() - time_since_sample >= time_between_samples) {
    buff[sample_num] = read();
    sample_num++;
    time_since_sample = micros();
  }
  return false;
}

void Microphone::on_finish() {
  Serial.println("Done recording");
  recording = false;
}





Button::Button(int p) {
  flag = 0;
  state = 0;
  pin = p;
  t_of_state_2 = millis(); //init
  t_of_button_change = millis(); //init
  debounce_time = 10;
  long_press_time = 400;
  button_pressed = 0;
}

void Button::read() {
  uint8_t button_state = digitalRead(pin);
  button_pressed = !button_state;
}

int Button::update() {
  read();
  flag = 0;
  switch (state) {
    case 0:
      if (button_pressed) {
        state = 1;
        t_of_button_change = millis();
      }
      break;

    case 1:
      if (button_pressed && millis() - t_of_button_change > debounce_time) {
        state = 2;
        t_of_state_2 = millis();
      } else if (!button_pressed) {
        state = 0;
        t_of_button_change = millis();
      }
      break;
    case 2:
      if (button_pressed && millis() - t_of_state_2 > long_press_time) {
        state = 3;
      } else if (!button_pressed) {
        state = 4;
        t_of_button_change = millis();
      }
      break;
    case 3:
      if (!button_pressed) {
        state = 4;
        t_of_button_change = millis();
      }
      break;
    case 4:
      if (button_pressed && millis() - t_of_state_2 < long_press_time) {
        state = 2;
        t_of_button_change = millis();
      } else if (button_pressed && millis() - t_of_state_2 > long_press_time) {
        state = 3;
        t_of_button_change = millis();
      } else if (!button_pressed && millis() - t_of_button_change > debounce_time) {
        state = 0;
        if (millis() - t_of_state_2 > long_press_time) flag = 2;
        else flag = 1;
      }
      break;
  }
  return flag;
}






RequestSender::RequestSender() {

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

void RequestSender::set_host(char* host_) {
  sprintf(host, "%s", host_);
}

void RequestSender::set_destination(char* dest) {
  sprintf(destination, "%s", dest);
}

void RequestSender::set_username(char* username_) {
  sprintf(username, "%s", username_);
}

uint8_t RequestSender::char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void RequestSender::send_video(char* img_ptr, char* audio_ptr) {  // send video that the user posted
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
      char_append(response, client.read(), out_buffer_size);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
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

//obtain the data from a user (state8)
void RequestSender::download_data(char* user_name, char* download_user_data) {
  int response_size = out_buffer_size;
  int response_timeout = 6000;
  bool serial = true;
  char request_header[400];
 // char user_name[10];

  sprintf(request_header, "GET %s?user=%s&menu=%s HTTP/1.1\r\n", destination,user_name, "False");
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
strncpy(download_user_data, response, 60);
}
