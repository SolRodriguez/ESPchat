#include "Camera.h"
#include <ArduCAM.h>
#include "memorysaver.h"
#include <Arduino.h>

Camera myCam;

void setup(){
    Serial.begin(115200);
    myCam.setup();
}

void loop(){
    char* data = myCam.get_image();
    Serial.println("\nPrinting first 10..");
    for(int i = 0; i < 10; i++){
        Serial.print((uint8_t) data[i]);
        Serial.print(",");
    }
    Serial.print("\n");
    delay(5000);
}