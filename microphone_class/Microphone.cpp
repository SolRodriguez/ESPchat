#include "Arduino.h"
#include "Microphone.h"

Microphone::Microphone(int p, int sample_rate, uint8_t * audio_buffer){
  pin = p;
  sr = sample_rate;
  time_between_samples = 1000000/sr;
  buff = audio_buffer;
}

uint16_t Microphone::rawRead(){
  read_value = analogRead(pin);
  return read_value;
}

uint8_t Microphone::read(){
  // converts 12-bit to 8-bit
  read_value = analogRead(pin) >> 4;
  return read_value;
}

void Microphone::record_all(int sample_length){
  // The current, blocking method of recording
  samp_len = sample_length;
  memset(buff, 0, sizeof(buff));
  sample_num = 0;
  while (sample_num < samp_len) {
    buff[sample_num] = read();
    sample_num++;
    while (micros() - time_since_sample <= time_between_samples); //blocking
    time_since_sample = micros();
  }
}

void Microphone::start_recording(int sample_length){
  if (recording) return;
  Serial.println("Start recording");
  samp_len = sample_length;
  recording = true;
  memset(buff, 0, sizeof(buff));
  sample_num = 0;
}

bool Microphone::on_update(){
  // called as often as possible ; non--blocking alternative
  // returns true when the recording is finished
  if (!recording) return false;
  if (sample_num > samp_len){
    on_finish();
    return true;
  }
  if (micros() - time_since_sample >= time_between_samples){
    buff[sample_num] = read();
    sample_num++;
    time_since_sample = micros();
  }
  return false; 
}

void Microphone::on_finish(){
  Serial.println("Done recording");
  recording = false;
}
  
