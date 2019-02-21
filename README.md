# MusicalLightsII
Arduino library to sync a custom-length string of lights to music.

Detects frequency ranges in audio signals and activates lights accordingly.

## Usage

```C++
FrequencyMeter meter;

// higher sampling rate yields better resolution for input, but less granularity for frequency ranges
meter.Initialize(sampling_rate);
// when frequency is detected at or above sensitivity level, pin will go HIGH
meter.AddFrequencyListener(pin_to_activate, frequency_in_hz, sensitivity);
// add as many listeners as desired, limited by number of digital output pins on arduino (14 on uno).

meter.ReadFrequencies();  // detemine frequency ranges present in audio signal
meter.DisplayLevels();    // activate pins based on frequency listeners
```

## Installation
This code was created using PlatformIO and VS Code; therefore the easiest way to use the code is to open it in a similar environment.
### PlatformIO

- Clone repository into an empty directory
- Open PlatformIO plugin within VS Code
- Select Open Project
- Browse for cloned repository and open it

That's it! Compile and run main.cpp (it contains a demo of functionality).
### Arduino IDE

For running in the Arduino IDE, you will need to add the library code to the Arduino Libraries directory in order to access it from your main ino file, instructions are here: https://www.arduino.cc/en/Guide/Libraries#toc4

## How it works
This library makes use of the ArduinoFHT library, to perform Fast Hartley Transforms on input audio and detemine frequency ranges (for more information on this library, see http://wiki.openmusiclabs.com/wiki/ArduinoFHT); however, it simplifies the setup and usage of it. High frequency audio sampling is also abstracted, resulting in a straightforward library that can be used to easily detect and react to frequency ranges in audio signals.


