#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define NEOPIN 2
#define NEOLEDCOUNT 150

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


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

inline uint16_t abs_led_position(int16_t p) {
    p = p % NEOLEDCOUNT;
    if (p >= 0) {
        return (p);
    }
    return( NEOLEDCOUNT - p );
}

void spinCycle(uint8_t wait) {
  int16_t i, j; // i will be replaced by millis() later
  int16_t position;
  uint32_t black = strip.Color(0,0,0);
  uint8_t cr = random(8,255);
  uint8_t cg = random(8,255);
  uint8_t cb = random(8,255);

  for (i=0; i < 10000; i++) {
      position = (i >> 10) % NEOLEDCOUNT;

      // blank stuff out:
      for (j=-24; j < -7; j++) {
          strip.setPixelColor(abs_led_position(position + j), black);
      }

      strip.setPixelColor(position, cr, cg, cb);
      for (j=1; j <= 8; j++) {
          uint32_t c = strip.Color(cr >> j, cb >> j, cb >> j);
          strip.setPixelColor(abs_led_position(position + j), c);
          strip.setPixelColor(abs_led_position(position - j), c);
      }

      strip.show();

      delay(wait); // not needed as we base location on millis()
  }
}


void setup() {
  Serial.begin(38400);
  randomSeed(analogRead(0));
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(32); // start with a conservatively-low brightness
}

uint8_t brightness = 30; // initially quite conservative

void loop() {
  Serial.print("Brightness: ");
  Serial.println(brightness);
  strip.setBrightness(brightness);
  brightness += 10;

  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  rainbow(20);
  colorWipe(strip.Color(0, 0, 0), 20); // blank
  rainbowCycle(20);
  colorWipe(strip.Color(0, 0, 0), 20); // blank
  spinCycle(50);
  colorWipe(strip.Color(0, 0, 0), 20); // blank
}

