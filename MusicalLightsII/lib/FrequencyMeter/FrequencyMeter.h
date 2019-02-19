#ifndef FREQUENCY_METER_H_
#define FREQUENCY_METER_H_

#define BUFFER_SIZE 10
#include <Arduino.h>

class FrequencyMeter {
 public:
  FrequencyMeter();
  void Initialize(uint8_t prescaler_value, uint16_t sample_rate);
  void DisplayLevels();
  void ReadFrequencies();
  void AddFrequencyListener(uint8_t pin_trigger, uint16_t frequency, uint16_t threshold);

 private:
  uint8_t number_of_triggers_;
  uint16_t triggers_[BUFFER_SIZE];
  uint16_t frequencies_[BUFFER_SIZE];
  uint16_t thresholds_[BUFFER_SIZE];
  uint8_t prescaler_value_;
  uint16_t adc_sample_rate_;
  uint16_t GetBinIndexFromFrequency(uint16_t frequency);
  uint8_t TranslatePrescaleValue(uint8_t prescaler_value);
};
#endif  // FREQUENCY_METER_H_