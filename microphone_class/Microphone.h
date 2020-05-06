#ifndef Microphone_h
#define Microphone_h
#include "Arduino.h"
class Microphone{
  private:
    int pin;
    uint32_t sr;
    uint16_t read_value;
    uint8_t * buff;
    bool recording = false;
    uint32_t time_since_sample;
    int sample_num;
    float time_between_samples;
    int samp_len;
  public:
    Microphone(int p, int sample_rate, uint8_t * audio_buffer);
    uint16_t rawRead();
    uint8_t read();
    void record_all(int sample_length);
    void start_recording(int sample_length);
    bool on_update();
    void on_finish();
};
#endif
