#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>

////Kim's WiFi
//char network[] = "ATT8s7N3kF";
//char password[] = "6trp7q?vtm3a";

char picture_data[2000]; //define here
char user[20]; //define here

char host[] = "608dev-2.net";
const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request

//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request //response_buffer

char request_cam_data[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request

void setup() {
  Serial.begin(115200); //for debugging if needed.
  setup_wifi();
}

void loop() {
  cam_data();

}

void cam_data() {
  char body[2000];
  sprintf(body, "user=%s&img=%s", user, picture_data); //generate body with the data
  int body_len = strlen(body); //calculate body length)
  sprintf(request_cam_data, "POST http://608dev-2.net/sandbox/sc/team044/espchat_dir/espchat.py HTTP/1.1\r\n");
  strcat(request_cam_data, "Host: 608dev-2.net\r\n");
  //strcat(request_cam_data, "Content-Type: application/x-www-form-urlencoded\r\n");
  strcat(request_cam_data, "Content-Type: text/plain\r\n");
  sprintf(request_cam_data + strlen(request_cam_data), "Content-Length: %d\r\n", body_len);
  strcat(request_cam_data, "\r\n"); //added
  strcat(request_cam_data, body); //body
  Serial.println(request_cam_data);
  strcat(request_cam_data, "\r\n"); //header
  Serial.println(request_cam_data);
  do_http_request(host, request_cam_data, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
}


void setup_wifi() {
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
}

/*----------------------------------
  char_append Function:
  Arguments:
     char* buff: pointer to character array which we will append a
     char c:
     uint16_t buff_size: size of buffer buff

  Return value:
     boolean: True if character appended, False if not appended (indicating buffer full)
*/
uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

/*----------------------------------
   do_http_request Function:
   Arguments:
      char* host: null-terminated char-array containing host to connect to
      char* request: null-terminated char-arry containing properly formatted HTTP request
      char* response: char-array used as output for function to contain response
      uint16_t response_size: size of response buffer (in bytes)
      uint16_t response_timeout: duration we'll wait (in ms) for a response from server
      uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
   Return value:
      void (none)
*/



void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  Serial.println("INSIDE THE HTTP_REQUEST");
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
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
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }

    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  }

  //not able to connect
  else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
