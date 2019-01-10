#include <FrequencyMeter.h>

FrequencyMeter::FrequencyMeter() {
    // number of triggers starts off as zero
    this.number_of_triggers = 0;
}

void FrequencyMeter::Initialize(uint8_t prescaler_value) {
  cli(); //disable interrupts
  //set up continuous sampling of analog pin 0 at 38.5kHz:
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  ADMUX |= (1 << REFS0); //set reference voltage
  // ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  // ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  sei();//enable interrupts
}

void FrequencyMeter::AddFrequencyListener(uint8_t pin_trigger, uint16_t frequency, uint16_t threshold) {
    this.triggers[this.number_of_triggers] = pin_trigger;
    this.frequencies[this.number_of_triggers] = frequency;
    this.thresholds[this.number_of_triggers] = threshold;
    pinMode(pin_trigger, OUTPUT);
    // increase number of triggers
    this.number_of_triggers++;
}

int FrequencyMeter::GetBinIndexFromFrequency(uint16_t frequency) {
  int chunk_size = SAMPLE_RATE/(FHT_N/2);
  int bin_index = frequency/chunk_size;
  
  return bin_index;
}

void FrequencyMeter::DisplayLevels() {
  int value;
  for(uint16_t i = 0; i < this.number_of_triggers; i++) {
    value = fht_log_out[GetBinIndexFromFrequency(this.frequencies[i])];
    if (value > this.thresholds[i]) {
        digitalWrite(this.triggers[i], HIGH);
    } else { 
        digitalWrite(this.triggers[i], LOW); 
    }
  }
}

void FrequencyMeter::ReadFrequencies() {
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei();
}