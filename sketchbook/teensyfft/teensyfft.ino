#include "Arduino.h"
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIN 3
#define NEOLEDCOUNT 146

const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioInputI2S       audioInput;         // audio shield: mic or line-in
AudioAnalyzeFFT256  myFFT(2);
AudioOutputI2S      audioOutput;        // audio shield: headphones & line-out

// Create Audio connections between the components
//
AudioConnection c1(audioInput, 0, audioOutput, 0);
AudioConnection c2(audioInput, 0, myFFT, 0);
AudioConnection c3(audioInput, 1, audioOutput, 1);

// Create an object to control the audio shield.
// 
AudioControlSGTL5000 audioShield;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOLEDCOUNT, NEOPIN, NEO_GRB + NEO_KHZ800);

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
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
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}


void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);
  
  Serial.begin(115200);
  randomSeed(analogRead(0));
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 10); 
  colorWipe(strip.Color(192, 192, 0), 10);
  colorWipe(strip.Color(0, 192, 192), 10);
  //strip.setBrightness(190); // we can't handle full brightness and white
  colorWipe(strip.Color(128, 128, 128), 10);

  // Enable the audio shield and set the output volume.
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(60);
}

int nsum[8] = {1, 2, 3, 6, 10, 24, 80};
int oct_out[8];

void loop() {
  int total;
  int n;
  int j;

  if (myFFT.available()) {
    j = 0;
    // each time new FFT data is available
    // print it all to the Arduino Serial Monitor
    for (int i=0; i<8; i++) {
      total = 0;
      n = nsum[i];
      while (n > 0) {
        total += myFFT.output[j++];
        n--;
      }
      oct_out[i] = total;
    }
    serialgraph();
    ledgraph();
  }
}

void serialgraph() {
    int zz;
    int max = 0;
    Serial.write(27);   // Print "esc"
    Serial.print("[2J");     // Clear screen

    for (int i=0; i < 8; i++) {
        Serial.print(i, DEC);
        Serial.print(" ");

        if (oct_out[i] > max) { max = oct_out[i]; }

        zz = oct_out[i] / 10;
        while (zz > 0) {
            Serial.write('=');
            zz--;
        }
        Serial.println();
    }
    Serial.print("max = ");
    Serial.print(max, DEC);
    Serial.println();
}

void blank_strip() {
    for(int i=0; i < NEOLEDCOUNT; i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
    // strip.show();
}

void ledgraph() {
  int offset;
  int colorLevel;
  int num_bars;

  blank_strip();

  for (int i=0; i < 8; i++) {
    offset = 16 * i;
    colorLevel = oct_out[i] / 16;
    num_bars = oct_out[i] / 256;
    for (int j=0; j<num_bars; j++) {
        if (i == 0) {
          strip.setPixelColor(offset+j, 0, 0, colorLevel);
        }
        else if (i == 1) {
          strip.setPixelColor(offset+j, 0, colorLevel, 0);
        }
        else if (i == 2) {
          strip.setPixelColor(offset+j, colorLevel, 0, 0);
        }
        else if (i == 3) {
          strip.setPixelColor(offset+j, colorLevel, colorLevel, 0);
        }
        else if (i == 4) {
          strip.setPixelColor(offset+j, 0, colorLevel, colorLevel);
        }
        else if (i == 5) {
          strip.setPixelColor(offset+j, colorLevel, 0 , colorLevel);
        }
        else if (i == 6) {
          strip.setPixelColor(offset+j, colorLevel, colorLevel, colorLevel);
        }
        else if (i == 7) {
          strip.setPixelColor(offset+j, colorLevel, colorLevel, 0);
        }
    }

  }
  strip.show();
}
