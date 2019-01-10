#include <Arduino.h>
#include <FrequencyMeter.h>
// 19.2kHz = arduino default freq * default prescaler / custom prescaler
// 12.2kHz = 9.6kHz * 128 / 32
#define SAMPLE_RATE 19200 
#define PRESCALER_VALUE 32

int frequencies[]     = {40,  200,  500,  40,   5000, 10000};
int freq_thresholds[] = {175, 154,  120,  140,  75,  75};
// int freq_thresholds[] = {200, 204,  120,  140,  125,  110};

FrequencyMeter meter;

void setup(){
  meter.Initialize(PRESCALER_VALUE, SAMPLE_RATE);
  meter.AddFrequencyListener(13, 40, 175);
  meter.AddFrequencyListener(12, 200, 154);
  meter.AddFrequencyListener(11, 500, 120);
  meter.AddFrequencyListener(10, 40, 140);
  meter.AddFrequencyListener(9, 5000, 75);
  meter.AddFrequencyListener(8, 10000, 75);
  // // set LEDs for testing:
  // pinMode(13, OUTPUT);  // configure pin as output
  // pinMode(12, OUTPUT);  // configure pin as output
  // pinMode(11, OUTPUT);  // configure pin as output
  // pinMode(10, OUTPUT);  // configure pin as output
  // pinMode(9, OUTPUT);  // configure pin as output
  // pinMode(8, OUTPUT);  // configure pin as output
}

// int GetBinIndexFromFrequency(int frequency) {
//   int chunk_size = SAMPLE_RATE/(FHT_N/2);
//   int bin_index = frequency/chunk_size;
  
//   return bin_index;
// }

// void DisplayLevels() {
//   int pin = 13;
//   int value;
//   for(uint16_t i = 0; i < (sizeof(frequencies)/sizeof(int)); i++) {
//     value = fht_log_out[GetBinIndexFromFrequency(frequencies[i])];
//     if (value > freq_thresholds[i]) {
//       digitalWrite(pin, HIGH);
//     } else { digitalWrite(pin, LOW); }
//     pin--;
//   }
// }

void loop() {
  // put your main code here, to run repeatedly:

  meter.ReadFrequencies();
  meter.DisplayLevels();


  // cli();  // UDRE interrupt slows this way down on arduino1.0
  //   for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
  //     while(!(ADCSRA & 0x10)); // wait for adc to be ready
  //     ADCSRA = 0xf5; // restart adc
  //     byte m = ADCL; // fetch adc data
  //     byte j = ADCH;
  //     int k = (j << 8) | m; // form into an int
  //     k -= 0x0200; // form into a signed int
  //     k <<= 6; // form into a 16b signed int
  //     fht_input[i] = k; // put real data into bins
  //   }
  //   fht_window(); // window the data for better frequency response
  //   fht_reorder(); // reorder the data before doing the fht
  //   fht_run(); // process the data in the fht
  //   fht_mag_log(); // take the output of the fht
  //   sei();
  //   // for (byte i = 0 ; i < 6; i++) {
  //   //   Serial.print(fht_log_out[GetBinIndexFromFrequency(frequencies[i])]); // send out the data
  //   //   Serial.print("\t");
  //   // }
  //   // Serial.print("\n");
  //   DisplayLevels();
}