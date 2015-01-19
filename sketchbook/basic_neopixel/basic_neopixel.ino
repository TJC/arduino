#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define NEOPIN 8
#define NEOLEDCOUNT 149

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
    if (p >= 0) {
        return p % NEOLEDCOUNT;
    }
    return NEOLEDCOUNT - ( abs(p) % NEOLEDCOUNT );
}

void spinCycle() {
  static uint16_t i = 0;
  static uint32_t black = strip.Color(0,0,0);
  int16_t j, position;
  uint8_t col[3];

  position = i % NEOLEDCOUNT;

  WheelArray(i%255, col);

  // blank stuff out:
  for (j=0; j <= 64; j++) {
      strip.setPixelColor(abs_led_position(position - j), black);
      strip.setPixelColor(abs_led_position(position + j), black);
  }
  

  strip.setPixelColor(position-1, col[0], col[1], col[2]);
  strip.setPixelColor(position, col[0], col[1], col[2]);
  strip.setPixelColor(position+1, col[0], col[1], col[2]);
  for (j=2; j <= 64; j+=2) {
      uint32_t c = strip.Color(col[0] >> (j/4), col[1] >> (j/4), col[2] >> (j/4));
      strip.setPixelColor(abs_led_position(position + j), c);
      strip.setPixelColor(abs_led_position(position + j +1 ), c);
      strip.setPixelColor(abs_led_position(position - j), c);
      strip.setPixelColor(abs_led_position(position - j -1), c);
  }

  strip.show();
  i++;
}

void blank_strip() {
    for(int i=0; i < NEOLEDCOUNT; i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
    strip.show();
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  strip.setBrightness(220); // we can't handle full brightness and white

  //rainbowCycle(10);

  blank_strip();

  strip.setBrightness(255); // we can't handle full brightness and white
}

void loop() {

  //Serial.println("starting spin cycle");
  spinCycle();
  delay(30);
}




