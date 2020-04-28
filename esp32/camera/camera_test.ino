#include "Camera.h"
#include <ArduCAM.h>
#include "memorysaver.h"
#include <Arduino.h>
#include "RequestSender.h"
#include <TFT_eSPI.h>
#include <SPI.h>

Camera myCam;
RequestSender myRequest;
TFT_eSPI tft = TFT_eSPI();

const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;


void setup(){
    delay(1000);
    tft.init();
    tft.setRotation(2);
    tft.fillScreen(TFT_BLACK);

    Serial.begin(115200);
    myCam.setup();

    Serial.print("Starting WiFi.\n");
    myRequest.begin_wifi("2WIRE782", "4532037186");
    myRequest.set_host("608dev-2.net");
    myRequest.set_destination("/sandbox/sc/team044/espchat/server/vmreyes/espchat.py");
    myRequest.set_username("vmreyes");

    Serial.println("Finished setup.");
}

void loop(){
    uint8_t* data = myCam.get_image();
    //Serial.println("\nPrinting first 10..");
    //for(int i = 0; i < 10; i++){
    //    Serial.print((uint8_t) data[i]);
    //    Serial.print(",");
    //}

    //Serial.print("\n");

    tft.pushImage(0,0,80,60,data);
    //tft.drawImage(0,0)
    //for(int i = 0; i < 120*160; i++){
    //    tft.pushColor(data[i]);
    //}

    char audio[] = "abc";

    myRequest.set_host("608dev-2.net");
    myRequest.send_video((char*) data, audio);

    delay(2000);
}

