#include <FrequencyMeter.h>

FrequencyMeter::FrequencyMeter() {
    ;
}

void FrequencyMeter::Initialize(unsigned int prescaler_value) {
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

void FrequencyMeter::AddFrequencyListener(int pin_trigger, int frequency, int threshold) {
    // TODO: Write frequency and threshold to private fields
    pinMode(pin_trigger, OUTPUT);
}

int FrequencyMeter::GetBinIndexFromFrequency(int frequency) {
  int chunk_size = SAMPLE_RATE/(FHT_N/2);
  int bin_index = frequency/chunk_size;
  
  return bin_index;
}

void FrequencyMeter::DisplayLevels() {
  int pin = 13;
  int value;
  for(uint16_t i = 0; i < (sizeof(frequencies)/sizeof(int)); i++) {
    value = fht_log_out[GetBinIndexFromFrequency(frequencies[i])];
    if (value > freq_thresholds[i]) {
      digitalWrite(pin, HIGH);
    } else { digitalWrite(pin, LOW); }
    pin--;
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