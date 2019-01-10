#define BUFFER_SIZE 10
#include <Arduino.h>

class FrequencyMeter {
    private:
        uint8_t number_of_triggers;
        uint16_t triggers[BUFFER_SIZE];
        uint16_t frequencies[BUFFER_SIZE];
        uint16_t thresholds[BUFFER_SIZE];
        uint8_t prescaler_value;
        uint16_t adc_sample_rate;
        uint16_t GetBinIndexFromFrequency(uint16_t frequency);
    public:
        FrequencyMeter();
        void Initialize(uint8_t prescaler_value);
        void DisplayLevels();
        void ReadFrequencies();
        void AddFrequencyListener(uint8_t pin_trigger, uint16_t frequency, uint16_t threshold);
}