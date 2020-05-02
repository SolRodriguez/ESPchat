#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiClientSecure.h>
//WiFiClientSecure is a big library. It can take a bit of time to do that first compile

TFT_eSPI tft = TFT_eSPI();

const int DELAY = 1000;
const int SAMPLE_FREQ = 8000;                          // Hz, telephone sample rate
const int SAMPLE_DURATION = 3;                        // duration of fixed sampling (seconds)
const int NUM_SAMPLES = SAMPLE_FREQ * SAMPLE_DURATION;  // number of of samples
const int ENC_LEN = (NUM_SAMPLES + 2 - ((NUM_SAMPLES + 2) % 3)) / 3 * 4;  // Encoded length of clip

const uint16_t RESPONSE_TIMEOUT = 6000;
const uint16_t OUT_BUFFER_SIZE = 500; //size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

/* CONSTANTS */
//Prefix to POST request:
//const char PREFIX[] = "{\"config\":{\"encoding\":\"MULAW\",\"sampleRateHertz\":8000,\"languageCode\": \"en-US\"}, \"audio\": {\"content\":\"";
//const char PREFIX[] = "";
//const char SUFFIX[] = "\"}}"; //suffix to POST request
const int AUDIO_IN = A0; //pin where microphone is connected
const int SPEAKER_OUT = A18;
const char API_KEY[] = "AIzaSyDqpeTAs-OsdG1Q4W4ANSNrNJKv8T7PgX8"; //don't change this


const uint8_t PIN_1 = 16; //button 1
const uint8_t PIN_2 = 5; //button 2


/* Global variables*/
uint8_t button_state; //used for containing button state and detecting edges
int old_button_state; //used for detecting button edges
uint8_t button_state_2;
int old_button_state_2;
uint32_t time_since_sample;      // used for microsecond timing


//char speech_data[ENC_LEN + 200] = {0}; //global used for collecting speech data
const char* NETWORK = "DD4416";     // your network SSID (name of wifi network)
const char* PASSWORD = "38649592"; // your network password

uint8_t old_val;
uint32_t timer;

//WiFiClientSecure client; //global WiFiClient Secure object

void setup() {
  Serial.begin(115200);               // Set up serial port
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  Serial.begin(115200); //begin serial comms
  delay(100); //wait a bit (100 ms)
  pinMode(PIN_1, INPUT_PULLUP);
  pinMode(PIN_2, INPUT_PULLUP);
  pinMode(14, OUTPUT);
  pinMode(SPEAKER_OUT, OUTPUT);

//  WiFi.begin(NETWORK, PASSWORD); //attempt to connect to wifi
//  uint8_t count = 0; //count used for Wifi check times
//  Serial.print("Attempting to connect to ");
//  Serial.println(NETWORK);
//  while (WiFi.status() != WL_CONNECTED && count < 12) {
//    delay(500);
//    Serial.print(".");
//    count++;
//  }
//  delay(2000);
//  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
//    Serial.println("CONNECTED!");
//    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
//                  WiFi.localIP()[1], WiFi.localIP()[0],
//                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
//    delay(500);
//  } else { //if we failed to connect just Try again.
//    Serial.println("Failed to Connect :/  Going to restart");
//    Serial.println(WiFi.status());
//    ESP.restart(); // restart the ESP (proper way)
//  }
  timer = millis();
  old_val = digitalRead(PIN_1);
}

char username[] = "dshkreli";
bool flag = true;
char host[] = "608dev-2.net";
//char audio_data[7000];
int recording_size = 0;
uint8_t thing[NUM_SAMPLES];
//main body of code
void loop() {
  button_state = digitalRead(PIN_1);
  button_state_2 = digitalRead(PIN_2);
  if (!button_state && button_state != old_button_state) {
    Serial.println("listening...");
    record_audio();
    if (flag == false){
      Serial.println("sending...");
      //do_http_request(host, audio_data, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
      play_audio();
      //sprintf(thing, "");
      flag = true;
    }
  }
  if (!button_state_2 && button_state_2 != old_button_state_2){
//    sprintf(audio_data, "POST /sandbox/sc/dshkreli/dummy_project/dummy_server.py HTTP/1.1\r\n");
//    sprintf(audio_data + strlen(audio_data), "Host: %s\r\n", host);
//    strcat(audio_data, "Content-Type: multipart/form-data\r\n");
//    sprintf(thing,"user=%s&sd=%s", username, "0");
//    sprintf(audio_data + strlen(audio_data), "Content-Length: %d\r\n\r\n", strlen(thing));
//    strcat(audio_data, thing);
//    do_http_request(host, audio_data, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  }
  old_button_state = button_state;
  old_button_state_2 = button_state_2;
}

//function used to record audio at sample rate for a fixed nmber of samples
void record_audio() {
  memset(thing, 0, recording_size);
  
  int sample_num = 0;    // counter for samples
  float time_between_samples = 1000000/SAMPLE_FREQ; 
  int read_value = 0;
  Serial.println("starting");
  uint32_t start = millis();
  time_since_sample = micros();
//  sprintf(audio_data, "POST /sandbox/sc/dshkreli/dummy_project/dummy_server.py HTTP/1.1\r\n");
//  sprintf(audio_data + strlen(audio_data), "Host: %s\r\n", host);
//  strcat(audio_data, "Content-Type: multipart/form-data\r\n");
//  sprintf(thing,"user=%s&sd=%s&audio=", username, "1");
//  int thing_length = strlen(thing);
//  Serial.println("this length is essential");
//  Serial.println(thing_length);
  int thing_length = 0;
  int t1 = millis();
  while (sample_num < NUM_SAMPLES) { //read in NUM_SAMPLES worth of audio data
    read_value = analogRead(AUDIO_IN);  //make measurement
    //strcpy(holder, "");
    //int wo_offset = ((value-1241)/16)-1;
    uint8_t reading = (read_value >> 4)-1;
    if(reading > 258){
      Serial.println("CLIP");
    }
    thing[thing_length] = reading;
    sample_num++;
    thing_length++;
    //Serial.printf("reading of %d led to storing of %d\n", value, reading);
    while (micros() - time_since_sample <= time_between_samples); //wait...
    time_since_sample = micros();
  }
  Serial.println(millis()-t1);
  Serial.println(millis() - start);
  Serial.println("out");
  flag = false;
  recording_size = thing_length;
}

void play_audio(){
//  for(int i=0; i<strlen(thing); i++){
//    Serial.println(i);
//    char reading = thing[i]; 
//    uint8_t num_reading = (uint8_t) reading;
//    Serial.println(num_reading);
//    digitalWrite(3, HIGH);
//    digitalWrite(3,num_reading);
//    delay(10);
//    digitalWrite(SPEAKER_OUT, LOW);
//  }
  //Serial.println(thing);
  int max_val = 0;
  int min_val = 256;
  float time_between_samples = 1000000/SAMPLE_FREQ;
  time_since_sample = micros();
  for(int i=0;i<recording_size;i++){
    uint8_t num_reading = thing[i];

    if (num_reading > max_val) max_val = num_reading;
    else if (num_reading < min_val) min_val = num_reading;
    
    dacWrite(25,num_reading);
    while (micros() - time_since_sample <= time_between_samples); //wait...
    time_since_sample = micros();
  } 
  for(int i=0;i<20000;i++){
    Serial.print(thing[i]);
    Serial.print(",");
  } 
}


int8_t mulaw_encode(int16_t sample) {
  //paste the fast one here.
   const uint16_t MULAW_MAX = 0x1FFF;
   const uint16_t MULAW_BIAS = 33;
   uint16_t mask = 0x1000;
   uint8_t sign = 0;
   uint8_t position = 12;
   uint8_t lsb = 0;
   if (sample < 0)
   {
      sample = -sample;
      sign = 0x80;
   }
   sample += MULAW_BIAS;
   if (sample > MULAW_MAX)
   {
      sample = MULAW_MAX;
   }
   for (; ((sample & mask) != mask && position >= 5); mask >>= 1, position--)
        ;
   lsb = (sample >> (position - 4)) & 0x0f;
   return (~(sign | ((position - 5) << 4) | lsb));
}


uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
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
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
