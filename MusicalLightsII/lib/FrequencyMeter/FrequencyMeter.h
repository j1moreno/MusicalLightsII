class FrequencyMeter {
    private:
        int *triggers;
        int *frequencies;
        int *thresholds;
        int prescaler_value;
        int adc_sample_rate;
        int GetBinIndexFromFrequency(int frequency);
    public:
        FrequencyMeter();
        void Initialize(unsigned int prescaler_value);
        void DisplayLevels();
        void ReadFrequencies();
        void AddFrequencyListener(int pin_trigger, int frequency, int threshold);
}