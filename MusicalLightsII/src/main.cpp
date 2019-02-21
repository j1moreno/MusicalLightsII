#include <Arduino.h>
#include <FrequencyMeter.h>
// sampling rate = adc default freq * default prescaler / custom prescaler
// 38.4kHz = 9.6kHz * 128 / 32
#define SAMPLE_RATE 19200 
#define PRESCALER_VALUE 32

FrequencyMeter meter;

void setup(){
  meter.Initialize(A0, SAMPLE_RATE);
  meter.AddFrequencyListener(13, 75, 175);
  meter.AddFrequencyListener(12, 200, 154);
  meter.AddFrequencyListener(11, 500, 120);
  meter.AddFrequencyListener(10, 40, 140);
  meter.AddFrequencyListener(9, 5000, 75);
  meter.AddFrequencyListener(8, 9000, 75);
}

void loop() {
  meter.ReadFrequencies();
  meter.DisplayLevels();
}