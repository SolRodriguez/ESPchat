#include <WiFi.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <string.h>

TFT_eSPI tft = TFT_eSPI();
const int IMAGE_SIZE = 128;
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;

char network[] = "2WIRE782";  //SSID CHANGE!!
char password[] = "4532037186"; //Password for WiFi CHANGE!!!
char host[] = "608dev-2.net";

const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

uint16_t image[IMAGE_SIZE*IMAGE_SIZE];

void setup() {
  Serial.begin(115200); 
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);

  setup_wifi();
}

void loop() {
  int time1 = millis();
  //random_draw();
  generate_image();
  Serial.printf("It took %d milliseconds to generate a %dx%d image.\n", millis()-time1, IMAGE_SIZE, IMAGE_SIZE);

  int time2 = millis();
  //random_draw();
  //generate_image();
  tft.pushImage(0, 0, IMAGE_SIZE, IMAGE_SIZE, image);
  Serial.printf("It took %d milliseconds to display a %dx%d image.\n", millis()-time2, IMAGE_SIZE, IMAGE_SIZE);
  delay(100);
}

void random_draw() {
  for(uint8_t y = 0; y < IMAGE_SIZE; y++){
    for(uint8_t x = 0; x < IMAGE_SIZE; x++){
      tft.pushColor(random(65536));
      //tft.drawPixel(x, y, random(65536)); // colors from 0 to 2^16-1
    }
  }
}

void generate_image() {
  for(uint16_t i = 0; i < IMAGE_SIZE*IMAGE_SIZE; i++){
    image[i] = random(65536);
  }
}

void setup_wifi() {
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
}