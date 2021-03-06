#include <Wire.h>
#include <ESP32WebServer.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#include "Camera.h"

Camera::Camera(){}

void Camera::setup(){
  pinMode(CS,OUTPUT);
  Wire.begin();
  Serial.println(F("ArduCAM Start!"));
//  SPI.begin();
//  SPI.setFrequency(4000000); //4MHz

  arduCam.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = arduCam.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55){
    Serial.println(F("SPI1 interface Error!"));
    while(1);
}
  arduCam.wrSensorReg8_8(0xff, 0x01);
  arduCam.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  arduCam.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
    Serial.println(F("Can't find OV2640 module!"));
  } else {
  Serial.println(F("OV2640 detected."));
  }

  arduCam.set_format(BMP); // resolution 320x240
  arduCam.InitCAM();
  //arduCam.OV2640_set_JPEG_size(3); 
  arduCam.clear_fifo_flag();
}

void Camera::capture(){
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

void Camera::start_capture(){
    arduCam.flush_fifo();
    arduCam.clear_fifo_flag();
    arduCam.start_capture();
}
