//Originals from chat_client_gui
//#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <TFT_eSPI.h>
#include <string.h>
#include "Button.h"

//Original from camera_test
#include "CameraEspchat.h"
#include <ArduCAM.h>
#include "memorysaver.h"
#include <Arduino.h>
Camera myCam;
RequestSender myRequest;


//Original code from chat_client_gui
// State stuff
#define TO_MAIN_MENU 0
#define MAIN_MENU 1

#define TO_STATE1 2
#define STATE1 3

#define TO_SELECT 4
#define SELECT 5

#define TO_USER_MENU 6
#define USER_MENU 7

#define TO_STATE4 8
#define STATE4 9

#define TO_IMAGE 10
#define IMAGE 11

#define TO_VIDEO 12
#define VIDEO 13

#define TO_UPLOAD 14
#define UPLOAD 15

#define TO_STATE8 16
#define STATE8 17

#define TO_RESET 18
#define RESET 19

#define TO_USER_CHANGE 20
#define USER_CHANGE 21

// Color stuff
#define BACKGROUND TFT_BLACK
#define TEXT_COLOR TFT_RED
#define CURSOR_COLOR TFT_BLUE

// Things no longer needed now that Button class is implemented
//const int LONG_TIMEOUT = 6000; //long press timeout
//const int SHORT_TIMEOUT = 1000; //short press limit
//const int CUTOFF = 2000;
//uint8_t b1;
//uint8_t b2;

const uint8_t INPUT_PIN1 = 16; //pin connected to button
const uint8_t INPUT_PIN2 = 5; //pin connected to button

Button left_button(INPUT_PIN1);
Button right_button(INPUT_PIN2);

uint8_t state;
//uint8_t state2;

uint8_t current_choice; // used for UI menu selection

// timer stuff
uint32_t Time_pressed;
uint32_t timer;

// user stuff
bool selected_user = false;
bool content = false;

//temps for video recording'''
uint32_t max_time_to_record = 3000; // 3 secs of recording
uint32_t time_temp;


TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;

//char network[] = "";  //SSID CHANGE!!
//char password[] = ""; //Password for WiFi CHANGE!!!


//Kim's WiFi
//char network[] = "ATT8s7N3kF";
//char password[] = "6trp7q?vtm3a";
//char host[] = "608dev-2.net";

void setup() {
  //Original set-up from chat_client_gui
  Serial.begin(115200); //for debugging if needed.
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TEXT_COLOR, BACKGROUND); //set color of font to red foreground, black background
  pinMode(INPUT_PIN1, INPUT_PULLUP); //set input pin as an input!
  pinMode(INPUT_PIN2, INPUT_PULLUP); //set input pin as an input!
  state = TO_MAIN_MENU;
  current_choice = 0;

  //Original from set-up camera_test
  
  myRequest.begin_wifi("ATT8s7N3kF", "6trp7q?vtm3a");
  myRequest.set_host("608dev-2.net");
  myRequest.set_destination("/sandbox/sc/vmreyes/final/echo.py");
  myRequest.set_username("vmreyes");
}

void loop() {
  //Original loop from chat_client_gui
  uint8_t left_flag = left_button.update();
  uint8_t right_flag = right_button.update();

  // Testing
  char output[100];
  if (left_flag != 0 or right_flag != 0) {
    sprintf(output, "%d, %d", left_flag, right_flag);
    Serial.println(output);
  }

  fsm(left_flag, right_flag);


  //Original loop from camera_test
//  char* data = myCam.get_image();
//  Serial.println("\nPrinting first 10..");
//  for (int i = 0; i < 10; i++) {
//    Serial.print((uint8_t) data[i]);
//    Serial.print(",");
//  }
//  Serial.print("\n");
//
//  char audio[] = "abc";
//
//  myRequest.send_video(data, audio);
//
//  delay(10000);
}

void display_bottom_ui(char* label_1, char* label_2, char* label_3, char* label_4) {
  tft.drawLine(0, 138, 128, 138, TFT_WHITE);
  tft.drawString(label_1, 5, 140, 1); tft.drawString(label_2, 69, 140, 1);
  tft.drawString(label_3, 5, 150, 1); tft.drawString(label_4, 69, 150, 1);
}

void fsm(uint8_t left_flag, uint8_t right_flag) {
  switch (state) {

    case TO_MAIN_MENU: // lead-in to MAIN MENU
      Serial.println("MAIN MENU");
      tft.fillScreen(BACKGROUND);
      current_choice = 0;
      tft.drawString("MAIN MENU", 30, 0, 2);
      tft.drawString("Make a post", 30, 100, 2);
      tft.drawString("See a post", 30, 120, 2);
      display_bottom_ui("UP", "DOWN", "------", "CONFIRM");
      state = MAIN_MENU;
      break;
    case MAIN_MENU: //main selection
      switch (current_choice) {
        case 0:
          // HOVER OVER "Make a post"
          tft.drawRect(15, 98, 98, 20, CURSOR_COLOR);
          switch (left_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 98, 98, 20, BACKGROUND); current_choice = 1; break;
            case 2: break;
          }
          switch (right_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 98, 98, 20, BACKGROUND); current_choice = 1; break;
            case 2: state = TO_SELECT; current_choice = 0; break; // TO SELECT
          }
          break; // out of case 0
        case 1:
          // HOVER OVER "See a post"
          tft.drawRect(15, 118, 98, 20, CURSOR_COLOR);
          switch (left_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 118, 98, 20, BACKGROUND); current_choice = 0; break;
            case 2: break;
          }
          switch (right_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 118, 98, 20, BACKGROUND); current_choice = 0; break;
            case 2: state = TO_USER_MENU; break; // TO USER_MENU
          }
          break; // out of case 1
      }
      break; // out of switch(current_choice)

    case TO_SELECT:
      Serial.println("TAKE AN IMAGE OR VIDEO");
      tft.fillScreen(BACKGROUND);
      current_choice = 0;
      tft.drawString("TAKE PIC OR VID", 15, 0, 2);
      tft.drawString("Take photo", 30, 60, 2);
      tft.drawString("Take video", 30, 80, 2);
      tft.drawString("Upload", 30, 100, 2);
      tft.drawString("Main Menu", 30, 120, 2);
      display_bottom_ui("UP", "DOWN", "------", "CONFIRM");
      state = SELECT;
      break;
    case SELECT: //look up at an image
      switch (current_choice) {
        case 0:
          // HOVER OVER "Take photo"
          tft.drawRect(15, 58, 98, 20, CURSOR_COLOR);
          switch (left_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 58, 98, 20, BACKGROUND); current_choice = 3; break;
            case 2: break;
          }

          
          switch (right_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 58, 98, 20, BACKGROUND); current_choice = 1; break;
            case 2: state = TO_IMAGE; current_choice = 0; break; // TO IMAGE
          }
          break; // out of case 0
        case 1:
          // HOVER OVER "Take video"
          tft.drawRect(15, 78, 98, 20, CURSOR_COLOR);
          switch (left_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 78, 98, 20, BACKGROUND); current_choice = 0; break;
            case 2: break;
          }
          switch (right_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 78, 98, 20, BACKGROUND); current_choice = 2; break;
            case 2: state = TO_VIDEO; break; // TO VIDEO
          }
          break; // out of case 1
        case 2:
          // HOVER OVER "Upload content"
          tft.drawRect(15, 98, 98, 20, CURSOR_COLOR);
          switch (left_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 98, 98, 20, BACKGROUND); current_choice = 1; break;
            case 2: break;
          }
          switch (right_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 98, 98, 20, BACKGROUND); current_choice = 3; break;
            case 2: state = TO_UPLOAD; break; // TO UPLOAD
          }
          break; // out of case 2
        case 3:
          // HOVER OVER "Main menu"
          tft.drawRect(15, 118, 98, 20, CURSOR_COLOR);
          switch (left_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 118, 98, 20, BACKGROUND); current_choice = 2; break;
            case 2: break;
          }
          switch (right_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 118, 98, 20, BACKGROUND); current_choice = 0; break;
            case 2: state = TO_RESET; break; // TO RESET
          }
          break; // out of case 3
      }
      break; // out of switch(current_choice)

    case TO_USER_MENU:
      Serial.println("USER MENU");
      tft.fillScreen(BACKGROUND);
      current_choice = 0;
      tft.drawString("USER MENU", 30, 0, 2);
      tft.drawString("Change users", 30, 80, 2);
      tft.drawString("Confirm", 30, 100, 2);
      tft.drawString("Main Menu", 30, 120, 2);
      display_bottom_ui("UP", "DOWN", "------", "CONFIRM");
      state = USER_MENU;
      break;
    case USER_MENU: //USER menu options
      switch (current_choice) {
        case 0:
          // HOVER OVER "Change users"
          tft.drawRect(15, 78, 98, 20, CURSOR_COLOR);
          switch (left_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 78, 98, 20, BACKGROUND); current_choice = 2; break;
            case 2: break;
          }
          switch (right_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 78, 98, 20, BACKGROUND); current_choice = 1; break;
            case 2: state = TO_USER_CHANGE; break; // TO USER_CHANGE
          }
          break; // out of case 1
        case 1:
          // HOVER OVER "Confirm"
          tft.drawRect(15, 98, 98, 20, CURSOR_COLOR);
          switch (left_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 98, 98, 20, BACKGROUND); current_choice = 0; break;
            case 2: break;
          }
          switch (right_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 98, 98, 20, BACKGROUND); current_choice = 2; break;
            case 2: state = TO_STATE8; break; // TO STATE8
          }
          break; // out of case 2
        case 2:
          // HOVER OVER "Main Menu"
          tft.drawRect(15, 118, 98, 20, CURSOR_COLOR);
          switch (left_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 118, 98, 20, BACKGROUND); current_choice = 1; break;
            case 2: break;
          }
          switch (right_flag) {
            case 0: break;
            case 1: tft.drawRect(15, 118, 98, 20, BACKGROUND); current_choice = 0; break;
            case 2: timer = millis(); state = TO_RESET; break; // TO RESET
          }
          break; // out of case 3
      }
      break; // out of switch(current_choice)

    case TO_STATE4:
      Serial.println("IN STATE4");
      tft.fillScreen(BACKGROUND);
      state = STATE4;
      break;
    case STATE4://change user option
      tft.setCursor(0, 40, 1);
      tft.print("Choose users here");
      tft.drawString("DUMMY SCREEN", 0, 80, 2);
      delay(2000);
      selected_user = true;
      Time_pressed = millis();
      state = TO_USER_MENU;
      break;
    //      tft.setCursor(0, 40, 1);
    //      tft.print("b1 - clicking through a list of user's");   // or is a new user returned to us... and we confirm them by going into state 5//
    //      tft.setCursor(0, 80, 1);
    //      tft.print("b2 - once done --> user menu");
    //      //iterate throughout options
    //      //must display options of users..
    //      selected_user = true;
    //      Time_pressed = millis();   /// restart timeer to have the option to leave or see post//
    //      //go back to state 3 --> button2 == 0
    //      if (b2 == 0) { //go back to state3  //b1==0//
    //        state = TO_USER_MENU;
    //      }

    case TO_IMAGE:
      tft.fillScreen(BACKGROUND);
      myCam.setup();  //moved it here,  since it SPI interfers with the main menu display on lcd making it go blank-k
      tft.drawString("Taking a picture....", 0, 40, 1);
      tft.drawString("DUMMY SCREEN", 0, 80, 2);
      state = IMAGE;
      break;
      
    case IMAGE: //takes picture
      delay(1200);
      Serial.println("GETTING image");///kim_delete
      myCam.capture();   ///kim_delete
      // Magical picture-taking code courtesy of Victor
      //

      content = true;
      Serial.println("BACK");
      state = TO_SELECT;
      break;

    case TO_VIDEO:
      tft.fillScreen(BACKGROUND);
      tft.drawString("Taking a video....", 0, 40, 1);
      tft.drawString("DUMMY SCREEN", 0, 80, 2);
      time_temp = millis();
      state = VIDEO;
      break;
    case VIDEO: //takes video
      //
      // Maybe have a separate state that actually records video,
      // while this state waits for a button hold
      // (video would only record while the button is held, or for a certain max length)
      //
      delay(1000);
      content = true;
      Time_pressed = millis();   //gives user option to upload or go back home
      state = TO_SELECT;
      break;

    case TO_UPLOAD:
      tft.fillScreen(BACKGROUND);
      if (!content) {
        tft.drawString("No content....", 0, 40, 1);
        delay(1000); // REMOVE FOR NON-BLOCKING (maybe move to separate state)
        state = TO_RESET;
      }
      else {
        tft.drawString("Uploading content....", 0, 40, 1);
        tft.drawString("DUMMY SCREEN", 0, 80, 2);
        state = UPLOAD;
      }
      break;
    case UPLOAD: //upload content such as video and image.
      //Time_pressed = millis(); //time_pressed should be reset
      delay(2000);
      content = false;
      state = TO_MAIN_MENU;   //would it be better for the user to return them to IDLE? .. initially it was to SELECT
      break;

    case TO_STATE8:
      tft.fillScreen(BACKGROUND);
      Serial.println("Selected user");
      if (selected_user == false) {
        timer = millis();
        tft.drawString("need to select a user", 0, 80, 2);
        state = TO_RESET;
      }

      else{
      tft.setCursor(0, 40, 1);
      tft.print("Sending GET request to get user's images");
      tft.drawString("DUMMY SCREEN", 0, 80, 2);
      state = STATE8;
      }
      break;
    case STATE8: //user selected
      // GET request here?
      timer = millis(); //time_pressed should be resetted
      delay(2000);
      selected_user = false;
      state = TO_MAIN_MENU;
      break;

    case TO_RESET:
      Serial.println("back to main menu");
      tft.fillScreen(BACKGROUND);
      state = RESET;
      break;
    case RESET: //RESET state - helpful when avoiding button memory overlapping
      selected_user = false; 
      content = false;
      delay(100);
      state = TO_MAIN_MENU;
      break;

    case TO_USER_CHANGE:
      Serial.println("IN STATE 10");
      tft.fillScreen(BACKGROUND);
      tft.drawString("Choosing user...", 0, 40, 1);
      tft.drawString("DUMMY SCREEN", 0, 80, 2);
      timer = millis();
      state = USER_CHANGE;
      break;
    case USER_CHANGE:
      state = TO_STATE4;
      // I'm unsure of what's supposed to be going on here - juju

      //      if (b2 == 1 && b1 == 1 && millis() - timer < LONG_TIMEOUT) {
      //        state = TO_STATE4;
      //      }

  } // end of switch(state)
}
