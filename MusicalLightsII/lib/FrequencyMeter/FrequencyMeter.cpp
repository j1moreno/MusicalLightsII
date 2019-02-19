#include <FrequencyMeter.h>
#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht
#include <FHT.h> // include the library

FrequencyMeter::FrequencyMeter() {
  // number of triggers starts off as zero
  number_of_triggers_ = 0;
}

uint8_t FrequencyMeter::TranslatePrescaleValue(uint8_t prescaler_value) {
  uint8_t bin_value;
  uint8_t default_value = 5;
  if (prescaler_value < 2) {
    bin_value = default_value;
  } else if ((prescaler_value % 2) != 0) {
    bin_value = default_value;
  } else {
    // log base 2
    bin_value = log(prescaler_value)/log(2);
  }

  return bin_value;
}

void FrequencyMeter::Initialize(uint8_t prescaler_value, uint16_t sample_rate) {
  prescaler_value_ = TranslatePrescaleValue(prescaler_value);
  adc_sample_rate_ = sample_rate;
  cli(); //disable interrupts
  //set up continuous sampling of analog pin 0 at 38.5kHz:
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  ADMUX |= (1 << REFS0); //set reference voltage
  // ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  ADCSRA |= prescaler_value_; //set ADC clock with prescaler
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  // ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  sei();//enable interrupts
}

void FrequencyMeter::AddFrequencyListener(uint8_t pin_trigger, uint16_t frequency, uint16_t threshold) {
  triggers_[number_of_triggers_] = pin_trigger;
  frequencies_[number_of_triggers_] = frequency;
  thresholds_[number_of_triggers_] = threshold;
  pinMode(pin_trigger, OUTPUT);
  // increase number of triggers
  number_of_triggers_++;
}

uint16_t FrequencyMeter::GetBinIndexFromFrequency(uint16_t frequency) {
  int chunk_size = adc_sample_rate_/(FHT_N/2);
  int bin_index = frequency/chunk_size;
  
  return bin_index;
}

void FrequencyMeter::DisplayLevels() {
  uint16_t value;
  for (uint16_t i = 0; i < number_of_triggers_; i++) {
    value = fht_log_out[GetBinIndexFromFrequency(frequencies_[i])];
    if (value > thresholds_[i]) {
      digitalWrite(triggers_[i], HIGH);
    } else { 
      digitalWrite(triggers_[i], LOW);
    }
  }
}

void FrequencyMeter::ReadFrequencies() {
  cli();  // UDRE interrupt slows this way down on arduino uno
  for (int i = 0 ; i < FHT_N ; i++) { // sample size is FHT_N
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5;            // restart adc
    byte m = ADCL;            // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m;     // form into an int
    k -= 0x0200;              // form into a signed int
    k <<= 6;                  // form into a 16b signed int
    fht_input[i] = k;         // put real data into bins
  }
  fht_window();   // window the data for better frequency response
  fht_reorder();  // reorder the data before doing the fht
  fht_run();      // process the data in the fht
  fht_mag_log();  // take the output of the fht
  sei();
}