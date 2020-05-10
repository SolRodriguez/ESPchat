#include <Arduino.h>
#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <string.h>
#include "Button.h"
#include "RequestSender.h"
#include <esp_task_wdt.h>
#include "amicrophone_class.h"

#include <ArduCAM.h>
#include "memorysaver.h"
#include "Camera.h"

#include <TFT_eSPI.h>


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

#define TO_USER_SEL 8
#define USER_SEL 9

#define TO_PLAYBACK 10
#define PLAYBACK 11

#define TO_VIDEO 12
#define VIDEO 13

#define TO_UPLOAD 14
#define UPLOAD 15

#define TO_GET_VIDEO 16
#define GET_VIDEO 17

#define TO_RESET 18
#define RESET 19


// Color stuff
#define BACKGROUND TFT_BLACK
#define TEXT_COLOR TFT_RED
#define CURSOR_COLOR TFT_BLUE

const uint8_t SECONDS = 4;

uint8_t video[80*60*SECONDS];
uint8_t audio[8000*SECONDS];

Camera myCam;
RequestSender myRequest;
TFT_eSPI tft = TFT_eSPI();
Microphone mic = Microphone(36,8000,audio);
TaskHandle_t recordTask;

const uint8_t INPUT_PIN1 = 16; //pin connected to button
const uint8_t INPUT_PIN2 = 5; //pin connected to button

Button left_button(INPUT_PIN1);
Button right_button(INPUT_PIN2);

uint8_t state;

uint8_t current_choice; // used for UI menu selection

uint32_t Time_pressed; // timer
uint32_t timer;

bool user_is_selected = false; // user selections
char selected_user[10] = "";
char users_available[50];
bool content = false;
int num_users = 0;
char user_list[5][10];

const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;



void setup() {
  //Original set-up from chat_client_gui
  Serial.begin(115200); 
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TEXT_COLOR, BACKGROUND); //set color of font to red foreground, black background
  pinMode(INPUT_PIN1, INPUT_PULLUP); //set input pin as an input!
  pinMode(INPUT_PIN2, INPUT_PULLUP); //set input pin as an input!
  state = TO_MAIN_MENU;
  current_choice = 0;

  myCam.setup();

  // myRequest.begin_wifi("2WIRE782", "4532037186");
  // myRequest.set_username("vmreyes");
  myRequest.begin_wifi("HoangSPB6-2G", "sU=09nV=02jG=05=#");
  myRequest.set_username("jghoang");
  myRequest.set_host("608dev-2.net");
  myRequest.set_destination("/sandbox/sc/team044/espchat/server/espchat.py");
}

void loop() {
  //Original loop from chat_client_gui
  uint8_t left_flag = left_button.update();
  uint8_t right_flag = right_button.update();

  // // Testing
  char output[100];
  // if (left_flag != 0 or right_flag != 0) {
  //   sprintf(output, "%d, %d", left_flag, right_flag);
  //   Serial.println(output);
  // }

  fsm(left_flag, right_flag);
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
      tft.drawRoundRect(15, 98 + current_choice*20, 98, 20, 10, CURSOR_COLOR);
      switch (left_flag) {
        case 0: break;
        case 1: tft.drawRoundRect(15, 98 + current_choice*20, 98, 20, 10,BACKGROUND); current_choice = (current_choice == 0) ? 1: current_choice-1; break;
        case 2: break;
      }
      switch (right_flag) {
        case 0: break;
        case 1: tft.drawRoundRect(15, 98 + current_choice*20, 98, 20, 10, BACKGROUND); current_choice = (current_choice + 1) % 2; break;
        case 2: switch(current_choice){
          case 0: state = TO_SELECT; break;
          case 1: state = TO_USER_MENU; break;
        } break;
      }
      break;

    case TO_SELECT:
      Serial.println("SELECT");
      tft.fillScreen(BACKGROUND);
      current_choice = 0;
      tft.drawString("MAKE A VIDEO", 15, 0, 2);
      tft.drawString("Playback", 30, 60, 2);
      tft.drawString("Take video", 30, 80, 2);
      tft.drawString("Upload", 30, 100, 2);
      tft.drawString("Main Menu", 30, 120, 2);
      display_bottom_ui("UP", "DOWN", "------", "CONFIRM");
      state = SELECT;
      break;
    case SELECT: //look up at an image
      tft.drawRoundRect(15, 58 + current_choice*20, 98, 20, 10, CURSOR_COLOR);
      switch (left_flag) {
        case 0: break;
        case 1: tft.drawRoundRect(15, 58 + current_choice*20, 98, 20, 10,BACKGROUND); current_choice = (current_choice == 0) ? 3: current_choice-1; break;
        case 2: break;
      }
      switch (right_flag) {
        case 0: break;
        case 1: tft.drawRoundRect(15, 58 + current_choice*20, 98, 20, 10, BACKGROUND); current_choice = (current_choice + 1) % 4; break;
        case 2: switch(current_choice){
          case 0: state = TO_PLAYBACK; break;
          case 1: state = TO_VIDEO; break;
          case 2: state = TO_UPLOAD; break;
          case 3: state = TO_RESET; break;
        } break;
      }
      break;

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
      tft.drawString("Current User:", 30, 30, 1);
      if (strcmp(selected_user, "") == 0) tft.drawString("NONE", 30, 40, 1);
      else tft.drawString(selected_user, 30, 40, 1);
      tft.drawRoundRect(15, 78 + current_choice*20, 98, 20, 10, CURSOR_COLOR);
      switch (left_flag) {
        case 0: break;
        case 1: tft.drawRoundRect(15, 78 + current_choice*20, 98, 20, 10,BACKGROUND); current_choice = (current_choice == 0) ? 2: current_choice-1; break;
        case 2: break;
      }
      switch (right_flag) {
        case 0: break;
        case 1: tft.drawRoundRect(15, 78 + current_choice*20, 98, 20, 10, BACKGROUND); current_choice = (current_choice + 1) % 3; break;
        case 2: switch(current_choice){
          case 0: state = TO_USER_SEL; break; 
          case 1: state = TO_GET_VIDEO; break;
          case 2: timer = millis(); state = TO_RESET; break;
        } break;
      }
      break; // out of switch(current_choice)

    case TO_USER_SEL:
      Serial.println("USER_SEL");
      tft.fillScreen(BACKGROUND);
      current_choice = 0;
      tft.drawString("USER SELECT", 25, 0, 2);
      tft.drawString("Loading...", 25, 20, 2);
      display_bottom_ui("UP", "DOWN", "------", "CONFIRM");
      myRequest.get_users(users_available);
      num_users = tokenize(users_available, user_list);
      tft.fillRect(0, 20, 128, 20, BACKGROUND);
      for (int i = 0; i < num_users; i++) tft.drawString(user_list[i], 25, 30 + 20 * i, 2);
      current_choice = 0;
      state = USER_SEL;
      break;
    case USER_SEL: //change user option
      tft.drawRect(15, 28 + 20 * current_choice, 98, 20, CURSOR_COLOR);
      switch (left_flag) {
        case 0: break;
        case 1: tft.drawRect(15, 28 + 20 * current_choice, 98, 20, BACKGROUND); if (current_choice == 0) current_choice = num_users - 1; else current_choice --;  break;
        case 2: break;
      }
      switch (right_flag) {
        case 0: break;
        case 1: tft.drawRect(15, 28 + 20 * current_choice, 98, 20, BACKGROUND); current_choice ++; if (current_choice >= num_users) current_choice = 0; break;
        case 2: user_is_selected = true; sprintf(selected_user, user_list[current_choice]); Time_pressed = millis(); state = TO_USER_MENU; break;
      }
      break;

    case TO_PLAYBACK:
      tft.fillScreen(BACKGROUND);
      state = PLAYBACK;
      break;
    case PLAYBACK: //playback
      if (!content){
        tft.drawString("No video...", 0, 40, 1);
        delay(1000);
      }
      else playback(video, audio);
      state = TO_SELECT;
      break;

    case TO_VIDEO:
      tft.fillScreen(BACKGROUND);
      tft.drawString("Taking a video....", 0, 40, 1);
      state = VIDEO;
      break;
    case VIDEO: //takes video
      content = true;
      record();
      Time_pressed = millis();   //gives user option to upload or go back home
      state = TO_SELECT;
      break;

    case TO_UPLOAD:
      tft.fillScreen(BACKGROUND);
      if (!content) {
        tft.drawString("No content....", 0, 40, 1);
        delay(1000); 
        state = TO_RESET;
      }
      else {
        tft.drawString("Uploading content....", 0, 40, 1);
        state = UPLOAD;
      }
      break;
    case UPLOAD: //upload content such as video and image.
      //Time_pressed = millis(); //time_pressed should be reset
      myRequest.set_destination("/sandbox/sc/team044/espchat/server/espchat.py");
      myRequest.set_host("608dev-2.net");
      myRequest.send_video((char*) video, (char*) audio);
      content = false;
      state = TO_MAIN_MENU;   //would it be better for the user to return them to IDLE? .. initially it was to SELECT
      break;

    case TO_GET_VIDEO:
      tft.fillScreen(BACKGROUND);
      Serial.println("GET_VIDEO");
      if (user_is_selected == false) {
        tft.drawString("No selected user", 0, 40, 1);
        delay(1000);
        timer = millis();
        state = TO_RESET;
      }
      tft.drawString("Loading video...", 0, 40, 1);
      state = GET_VIDEO;
      break;
    case GET_VIDEO: 
      myRequest.get_video(selected_user, video, audio);
      content = true;
      timer = millis(); //time_pressed should be resetted
      user_is_selected = false;
      sprintf(selected_user, "");
      state = TO_PLAYBACK;
      break;

    case TO_RESET:
      Serial.println("RESET");
      tft.fillScreen(BACKGROUND);
      state = RESET;
      break;
    case RESET: //RESET state - helpful when avoiding button memory overlapping
      user_is_selected = false; 
      content = false;
      delay(100);
      state = TO_MAIN_MENU;
      break;

  } // end of switch(state)
}


void record(){
  int k = 0;
  xTaskCreatePinnedToCore(
      recordCode, /* Function to implement the task */
      "Recording Task", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      5,  /* Priority of the task */
      &recordTask,  /* Task handle. */
      0); /* Core where the task should run */

  for(int frames = 0; frames < SECONDS; frames++){
    myCam.capture();
    for(int i = 0; i < 240; i++){
      for(int j = 0; j < 320; j++){
        uint8_t hv = SPI.transfer(0x00);
        uint8_t hl = SPI.transfer(0x00);
        if(!(i % 4) && !(j % 4)){
          uint16_t pixel_val = hv << 8 | hl;
          uint8_t pixel_data = ((pixel_val & 0xE000)>>8) | ((pixel_val & 0x0700)>>6) | ((pixel_val & 0x0018)>>3);
          if(pixel_data == 0x00){
            pixel_data += 36;
          }
          video[k++] = pixel_data;
        }
      }
    }
    tft.pushImage(30,60,80,60,video+frames*60*80);
  }
}

void recordCode(void* parameters){
  mic.start_recording(SECONDS*8000);
  bool recording_ = true;
  while(recording_){
    recording_ = !mic.on_update();
  }
  vTaskDelete(NULL);
}

void playback(uint8_t* video, uint8_t* audio) {
  int mil_timer = millis();
  int mic_timer = micros();

  int frames = 0;
  int a_ind = 0;

  tft.pushImage(30, 60, 80, 60, video);
  dacWrite(25, audio[0]);
  while (frames < SECONDS) {
    if (millis() - mil_timer >= 1000) {
      tft.pushImage(30, 60, 80, 60, video + frames * 60 * 80);
      frames++;
      mil_timer = millis();
    }
    if (micros() - mic_timer >= 125) {
      dacWrite(25, audio[a_ind]);
      a_ind++;
      mic_timer = micros();
    }
  }
}

int tokenize(char* menu, char  user_list[][10]) {
  char* temp = strtok(menu, "\n");
  int index = 0;
  while (temp != NULL) {
    sprintf(user_list[index], temp);
    temp = strtok(NULL, "\n");
    index++;
  }
  return index;
}

void display_bottom_ui(char* label_1, char* label_2, char* label_3, char* label_4) {
  tft.drawLine(0, 138, 128, 138, TFT_WHITE);
  tft.drawString(label_1, 5, 140, 1); tft.drawString(label_2, 69, 140, 1);
  tft.drawString(label_3, 5, 150, 1); tft.drawString(label_4, 69, 150, 1);
}
