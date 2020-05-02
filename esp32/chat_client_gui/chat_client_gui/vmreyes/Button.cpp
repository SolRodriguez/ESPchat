#include "Button.h"

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
      if (button_pressed && millis()-t_of_button_change > debounce_time){
        state = 2;
        t_of_state_2 = millis();
      } else if (!button_pressed){
        state = 0;
        t_of_button_change = millis();
      }
      break;
    case 2:
      if (button_pressed && millis()-t_of_state_2 > long_press_time){
        state = 3;
      } else if (!button_pressed){
        state = 4;
        t_of_button_change = millis();
      }
      break;
    case 3:
      if (!button_pressed){
        state = 4;
        t_of_button_change = millis();
      }
      break;
    case 4:      
      if (button_pressed && millis()-t_of_state_2 < long_press_time){
        state = 2;
        t_of_button_change = millis();
      } else if (button_pressed && millis()-t_of_state_2 > long_press_time){
        state = 3;
        t_of_button_change = millis();
      } else if (!button_pressed && millis()-t_of_button_change > debounce_time){
        state = 0;
        if (millis()-t_of_state_2 > long_press_time) flag = 2;
        else flag = 1;
      }
      break;
  }
  return flag;
}
