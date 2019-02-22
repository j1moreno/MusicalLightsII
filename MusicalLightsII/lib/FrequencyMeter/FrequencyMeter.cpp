#include <FrequencyMeter.h>
#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht
#include <FHT.h> // include the library

// Constructor for FrequencyMeter object. 
// Sets number of triggers to be zero.
// Initialize method will need to be called before usage. 
FrequencyMeter::FrequencyMeter() {
  // number of triggers starts off as zero
  number_of_triggers_ = 0;
}

// Private method for calculating the bit offset for register ADCSRA
// based on desired sampling rate. Actual sampling rate may not be 
// exactly equal to input parameter, because it is calculated using
// powers of 2; actual sampling rate will be closest configurable value.
uint8_t FrequencyMeter::GetPrescaleOffset(uint16_t sampling_rate) {
  uint16_t bin_value;
  // define default value, in case bin_value is calculated to be out of bounds
  uint8_t default_value = 5;
  // formula is based on default 16MHz system clock on Arduino Uno
  // 13 is the number of clock cycles it takes to complete one conversion.
  // prescaler_value = system_clock/13/sampling rate
  float prescaler_value = system_clock_frequency_/13/sampling_rate;
  // define bin_value to be closest power of 2 by taking log base 2
  // since this may not be a whole number, round first.
  bin_value = (int) round(log(prescaler_value)/log(2));
  
  // if bin_value is determined to be between prescaler bounds, return it
  // otherwise return default value, which will set the sampling rate 
  // to be about 38.5kHz, sufficient for capturing most frequencies found 
  // in audio signals.
  return (bin_value <= 7 && bin_value >= 0) ? bin_value : default_value;
}

// Initializes the FrequencyMeter object by setting up and preparing the
// ADC to sample the desired analog input pin at the specified sample rate.
void FrequencyMeter::Initialize(uint8_t analog_input_pin, uint16_t sample_rate) {
  uint8_t prescaler_offset = GetPrescaleOffset(sample_rate);
  // Set true calculated ADC sample rate
  adc_sample_rate_ = system_clock_frequency_/13/pow(2, prescaler_offset);
  // Begin ADC setup for continuous sampling of analog pin 0 at 38.5kHz:
  // Disable interrupts for now
  cli();
  // Clear ADCSRA and ADCSRB registers, set each bit in the registers to be 0
  ADCSRA = 0;
  ADCSRB = 0;
  // Set input pin
  uint8_t input_pin = analog_input_pin - PIN_A0;
  if (input_pin > 0) {
    // only have to set bits if input pin is something other than A0
    ADMUX |= input_pin;
  }
  // Set reference voltage -- REFS[2:0] = 1 will select AVcc
  ADMUX |= (1 << REFS0);
  // Set ADC clock with prescaler
  ADCSRA |= prescaler_offset;
  // Enable auto-trigger; source is given by ADTS[2:0] in ADCSRB register
  ADCSRA |= (1 << ADATE);
  // Explicitly define auto-trigger source
  ADCSRB &= 0xF4; // last 3 bits set to 0 -> free running mode
  // Enable ADC
  ADCSRA |= (1 << ADEN);
  // Start ADC measurements
  ADCSRA |= (1 << ADSC);
  // Re-enable interrupts
  sei();
}

// Adds a listener for a frequency range containing the specified frequency, 
// and triggers specified output pin when frequency magnitude is above the 
// given threshold.
void FrequencyMeter::AddFrequencyListener(uint8_t pin_trigger, uint16_t frequency, uint16_t threshold) {
  triggers_[number_of_triggers_] = pin_trigger;
  frequencies_[number_of_triggers_] = frequency;
  thresholds_[number_of_triggers_] = threshold;
  pinMode(pin_trigger, OUTPUT);
  // increase number of triggers
  number_of_triggers_++;
}

// Determines which FHT output bin contains frequency range corresponding 
// to the desired frequency.
uint16_t FrequencyMeter::GetBinIndexFromFrequency(uint16_t frequency) {
  int chunk_size = adc_sample_rate_/(FHT_N/2);
  int bin_index = frequency/chunk_size;
  
  return bin_index;
}

// Sets pins HIGH based on frequency listeners.
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

// Samples input signal at adc_sample_rate until required number of samples 
// for FHT is reached. FHT operation is then performed to determine 
// frequency components present in sampled signal.
void FrequencyMeter::ReadFrequencies() {
  // Disable interrupts for now
  cli();
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
  // Re-enable interrupts
  sei();
}