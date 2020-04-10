Camera::Camera(){
  static int i = 0;
  //set the CS as an output:
  pinMode(CS,OUTPUT);
  pinMode(CAM_POWER_ON , OUTPUT);
  digitalWrite(CAM_POWER_ON, HIGH);
  Wire.begin();
  Serial.println(F("ArduCAM Start!"));
  //initialize SPI:
  SPI.begin();

  while(1){
  //Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if(temp != 0x55){
        Serial.println(F("SPI interface Error!"));
        delay(2);
        continue;
    } else {
        break;
    }
  }
}