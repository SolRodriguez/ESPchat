#include "CameraEspchat.h"  ////change to CameraEspchat.h from  Camera.h
#include <ArduCAM.h>
#include "memorysaver.h"
#include <Arduino.h>
//#include "RequestSender.h"       ////change to CameraEspchat.h from RequestSender.h

Camera myCam;
RequestSender myRequest;

void setup(){
    Serial.begin(115200);
    myCam.setup();
    myRequest.begin_wifi("ATT8s7N3kF", "6trp7q?vtm3a");
    myRequest.set_host("608dev-2.net");
    myRequest.set_destination("/sandbox/sc/vmreyes/final/echo.py");
    myRequest.set_username("vmreyes");
}

void loop(){
    char* data = myCam.get_image();
    Serial.println("\nPrinting first 10..");
    for(int i = 0; i < 10; i++){
        Serial.print((uint8_t) data[i]);
        Serial.print(",");
    }
    Serial.print("\n");

    char audio[] = "abc";

    myRequest.send_video(data, audio);

    delay(10000);
}
