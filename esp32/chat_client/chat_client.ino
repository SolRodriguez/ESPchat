#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <TFT_eSPI.h>
#include<string.h>

#define IDLE 0
#define STATE1 1
#define STATE2 2
#define STATE3 3
#define STATE4 4
#define STATE5 5
#define STATE6 6
#define STATE7 7
#define STATE8 8
#define STATE9 9
#define STATE10 10
#define BACKGROUND TFT_BLACK

const int LONG_TIMEOUT = 6000; //long press timeout
const int SHORT_TIMEOUT = 1000; //short press limit
const int CUTOFF = 2000;

const uint8_t INPUT_PIN1 = 16; //pin connected to button
const uint8_t INPUT_PIN2 = 5; //pin connected to button

uint8_t state;
uint8_t state2;
uint8_t b1;
uint8_t b2;
//timers 
uint32_t Time_pressed;
uint32_t timer;

//Content of users 
bool selected_user = false;
bool content = false;


//temps for video recording'''
uint32_t max_time_to_record = 3000;// 3 secs of recording
uint32_t time_temp;


TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;

//char network[] = "";  //SSID CHANGE!!
//char password[] = ""; //Password for WiFi CHANGE!!!


//Kim's WiFi
char network[] = "ATT8s7N3kF";
char password[] = "6trp7q?vtm3a";

char host[] = "608dev-2.net";

void setup() {
  Serial.begin(115200); //for debugging if needed.
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK); //set color of font to green foreground, black background
  pinMode(INPUT_PIN1, INPUT_PULLUP); //set input pin as an input!
  pinMode(INPUT_PIN2, INPUT_PULLUP); //set input pin as an input!
  state = IDLE;
}

void loop() {
  b1 = digitalRead(INPUT_PIN1);
  b2 = digitalRead(INPUT_PIN2);
  fsm(b1, b2);
}

void fsm(uint8_t b1, uint8_t b2) {
  switch (state) {
    case IDLE: { //main selection
        Serial.println("HOME MENU");
        tft.setCursor(35, 0, 1);
        tft.print(" MAIN MENU     ");
        tft.setCursor(0, 40, 1);
        tft.print("Left button to make a post");
        tft.setCursor(0, 60, 1);
        tft.print("Right button to see a post");

        if (b1 == 0 && b2 != 0) { //take and image or video
          //  timer = millis();
          Time_pressed = millis();
          tft.fillScreen(BACKGROUND);
          state = STATE2;
        }

        if (b2 == 0 && b1 != 0) { //look at a users image or video
          Time_pressed = millis();
          tft.fillScreen(BACKGROUND);
          state = STATE3;
        }
      }
      break;

    case STATE2: { //look up at an image
        Serial.println("TAKE AN IMAGE OR VIDEO");
        tft.setCursor(26, 0, 1);
        tft.print("TAKE PIC OR VID");
        tft.setCursor(0, 40, 1);
        tft.print("Left short - take image");
        if (millis() - Time_pressed > LONG_TIMEOUT) {//these options are available now
          tft.setCursor(0, 60, 1);
          tft.print("Left long - take video");
          tft.setCursor(0, 80, 1);
          tft.print("Right long - upload content");
        }
        tft.setCursor(0, 100, 1);
        tft.print("Right short - back to main menu");

        if (b1 == 0 && (millis() - Time_pressed > LONG_TIMEOUT)) { //take video
          tft.fillScreen(BACKGROUND);
          Serial.println("recording video ...");
          time_temp = millis();
          state = STATE6;
        }
        if (b1 == 0 && (millis() - Time_pressed > SHORT_TIMEOUT) && (millis() - Time_pressed < LONG_TIMEOUT)) { //take picture
          tft.fillScreen(BACKGROUND);
          Serial.println("taking image...");
          state = STATE5;
        }
        if (b2 == 0 && b1 != 0 && (millis() - Time_pressed > LONG_TIMEOUT) && content == true) { //upload content
          tft.fillScreen(BACKGROUND);
          Serial.println("UPLOAD content");
          state = STATE7;
        }
        if ((b2 == 0 && b1 != 0 && (millis() - Time_pressed > LONG_TIMEOUT) && content == false) || (b2 == 0 && b1 != 0 && (millis() - Time_pressed > SHORT_TIMEOUT) && (millis() - Time_pressed < LONG_TIMEOUT))) { //IF no content
          timer = millis();
          state = STATE9;
        }
      }
      break;


    case STATE3: {//USER menu options
        tft.setCursor(25, 0, 1);
        tft.print("USER's MENU ");

        tft.setCursor(0, 40, 1);
        tft.print("Right short - change users");
        if (millis() - Time_pressed > LONG_TIMEOUT) {
          tft.setCursor(0, 60, 1);
          tft.print("Left long - confirm selecting    ");
        }
        tft.setCursor(0, 80, 1);
        tft.print("Left  - go back to home menu         ");

        if (b1 == 0 && b2 != 0 && (millis() - Time_pressed > SHORT_TIMEOUT) && (millis() - Time_pressed < LONG_TIMEOUT ) ||
            b1 == 0 && b2 != 0 && (millis() - Time_pressed > LONG_TIMEOUT) && selected_user == false) { //go back to HOME menu, even if user was not selected u can go back
          timer = millis();
          state = STATE9;
        }

        if (b2 == 0 && (millis() - Time_pressed > SHORT_TIMEOUT) && (millis() - Time_pressed < LONG_TIMEOUT)) {//look through options of users
          tft.fillScreen(BACKGROUND);
          Serial.println("Looking at different USERS...");
          //          if (b1 == 0) {
          //            tft.setCursor(0, 40, 1);
          //          }
          timer = millis();
          state = STATE10;
        }

        if (b1 == 0 && selected_user == true && (millis() - Time_pressed > LONG_TIMEOUT)) {//select/confirm the user  //b2 ==0
          tft.fillScreen(BACKGROUND);
          Serial.println("Select user");
          state = STATE8;//USer selected
        }

      }
      break;

    case STATE4: {//change user option
        Serial.println("IN STATE4");
        tft.setCursor(0, 40, 1);
        tft.print("b1 - clicking through a list of user's");   // or is a new user returned to us... and we confirm them by going into state 5//
        tft.setCursor(0, 80, 1);
        tft.print("b2 - once done --> user menu");
        //iterate throughout options
        //must display options of users..
        selected_user = true;
        Time_pressed = millis();   /// restart timeer to have the option to leave or see post//
        //go back to state 3 --> button2 == 0
        if (b2 == 0) { //go back to state3  //b1==0//
          state = STATE3;
        }
      }
      break;

    case STATE5: {//takes picture
        tft.setCursor(0, 40, 1);
        tft.print("Taking a picture....");
        delay(1200);
        content = true;
        state = STATE2;
      }
      break;

    case STATE6: {//takes video
        tft.setCursor(0, 40, 1);
        tft.print("Taking a video....");
        //takes video at some cut off
        if (millis() - time_temp > max_time_to_record) {
          content = true;
          Time_pressed = millis();   //gives user option to upload or go back home
          state = STATE2;
        }
      }
      break;

    case STATE7: {//upload content such as video and image.
        tft.setCursor(0, 40, 1);
        tft.print("Uploading content....");
        //Time_pressed = millis(); //time_pressed should be resetted
        delay(2000);
        content = false;
        state = IDLE;   //would it be better for the user to return them to IDLE? .. initially it was to STATE2
      }
      break;

    case STATE8: {//user selected
        tft.setCursor(0, 40, 1);
        tft.print("Sending GET request to get user's images");
        timer = millis(); //time_pressed should be resetted
        delay(2000);
        selected_user = false;
        state = IDLE; 
      }
      break;




    case STATE9: { //RESET state - helpful when avoiding button memory overlapping
        if (b2 == 1 && b1 == 1 && millis() - timer < LONG_TIMEOUT) {
          tft.fillScreen(BACKGROUND);
          Serial.println("back to main menu");
          selected_user = false; ////adeddd///
          content = false;
          delay(100);
          state = IDLE;
        }
      }
      break;

    case STATE10: {
        Serial.println("IN STATE 10");
        if (b2 == 1 && b1 == 1 && millis() - timer < LONG_TIMEOUT) {
          state = STATE4;
        }
      }
  }
}
