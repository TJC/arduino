#include "Arduino.h"
#include <Audio.h>
#include <Wire.h>
//#include <SD.h>
#include <FastLED.h>

#define NEOPIN 3
#define NEOLEDCOUNT 55

CRGB leds[NEOLEDCOUNT];

const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioInputI2S       audioInput;         // audio shield: mic or line-in
AudioAnalyzeFFT1024  myFFT;
AudioOutputI2S      audioOutput;        // audio shield: headphones & line-out

// Create Audio connections between the components
//
AudioConnection patchCord1(audioInput, 0, myFFT, 0);

// Create an object to control the audio shield.
// 
AudioControlSGTL5000 audioShield;

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
/*
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
*/

void WheelArray(byte WheelPos, uint8_t *col) {
    if(WheelPos < 85) {
        col[0] = WheelPos * 3;
        col[1] = 255 - WheelPos * 3;
        col[2] = 0;
    } else if(WheelPos < 170) {
        WheelPos -= 85;
        col[0] = 255;
        col[1] = 0;
        col[2] = WheelPos * 3;
    } else {
        WheelPos -= 170;
        col[0] = 0;
        col[1] = WheelPos * 3;
        col[2] = 255 - WheelPos * 3;
    }
}


// Fill the dots one after the other with a color
void colorWipe(int r, int g, int b, uint8_t wait) {
  for(uint16_t i=0; i<NEOLEDCOUNT; i++) {
      leds[i].setRGB(r, g, b);
      FastLED.show();
      delay(wait);
  }
}


void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);
  
  Serial.begin(115200);
  randomSeed(analogRead(0));
  FastLED.addLeds<NEOPIXEL, NEOPIN>(leds, NEOLEDCOUNT); 
  // TODO: set all pixels to off

  // Some example procedures showing how to display to the pixels:
  colorWipe(255, 0, 0, 10); 
  colorWipe(192, 192, 0, 10);
  colorWipe(0, 192, 192, 10);
  colorWipe(128, 128, 128, 10);

  // Enable the audio shield and set the output volume.
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.6);
  
  myFFT.windowFunction(AudioWindowHanning1024);
  //myFFT.windowFunction(NULL);

  
}

int nsum[8] = {1, 2, 3, 6, 10, 24, 80};
int oct_out[8];
double rolling_avg = 32000.0;

void loop() {
  int j;
  int imax = 0;
  double divisor;

  if (myFFT.available()) {
    // each time new FFT data is available
    for (int i=1; i < 128; i++) {
      if (myFFT.read(i) > imax) imax = myFFT.read(i);
    }
    if (imax > 99) rolling_avg = (rolling_avg * 0.97) + (double(imax) * 0.03);

    divisor = rolling_avg / 255.0;

    for (int i=0; i<NEOLEDCOUNT; i++) {
      j = myFFT.read(i) / divisor;
      if (j>255) j=255;
      if (j<32) j = 0;
      //strip.setPixelColor(i, j, j, j);
      leds[i].setRGB(j,j,j);
    }
    FastLED.show();
  }
}


void blank_strip() {
    for(int i=0; i < NEOLEDCOUNT; i++) {
      leds[i].setRGB(0,0,0);
    }
}

