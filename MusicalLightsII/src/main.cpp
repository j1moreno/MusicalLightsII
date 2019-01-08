#include <Arduino.h>
#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht
#include <FHT.h> // include the library
// 19.2kHz = arduino default freq * default prescaler / custom prescaler
// 12.2kHz = 9.6kHz * 128 / 32
#define SAMPLE_RATE 19200 

volatile int newData;

int frequencies[]     = {40,  200,  500,  40,   5000, 10000};
int freq_thresholds[] = {175, 154,  120,  140,  75,  75};
// int freq_thresholds[] = {200, 204,  120,  140,  125,  110};

void setup(){
  
  // Serial.begin(9600);
  
  cli();//diable interrupts
  
  //set up continuous sampling of analog pin 0 at 38.5kHz
 
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

  // set LEDs for testing:
  pinMode(13, OUTPUT);  // configure pin as output
  pinMode(12, OUTPUT);  // configure pin as output
  pinMode(11, OUTPUT);  // configure pin as output
  pinMode(10, OUTPUT);  // configure pin as output
  pinMode(9, OUTPUT);  // configure pin as output
  pinMode(8, OUTPUT);  // configure pin as output
}

int GetBinIndexFromFrequency(int frequency) {
  int chunk_size = SAMPLE_RATE/(FHT_N/2);
  int bin_index = frequency/chunk_size;
  
  return bin_index;
}

void DisplayLevels() {
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

void loop() {
  // put your main code here, to run repeatedly:

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
    // for (byte i = 0 ; i < 6; i++) {
    //   Serial.print(fht_log_out[GetBinIndexFromFrequency(frequencies[i])]); // send out the data
    //   Serial.print("\t");
    // }
    // Serial.print("\n");
    DisplayLevels();
}