#include <Arduino.h>
#include <FrequencyMeter.h>

#define SAMPLE_RATE 40000 

FrequencyMeter meter;

void setup(){
  meter.Initialize(A0, SAMPLE_RATE);
  meter.AddFrequencyListener(13, 100, 154);
  meter.AddFrequencyListener(12, 400, 154);
  meter.AddFrequencyListener(11, 1000, 120);
  meter.AddFrequencyListener(10, 40, 140);
  meter.AddFrequencyListener(9, 10000, 75);
  meter.AddFrequencyListener(8, 16000, 75);
}

void loop() {
  meter.ReadFrequencies();
  meter.DisplayLevels();
}