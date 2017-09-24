#include "Arduino.h"
#include <stdlib.h>
#include <FastLED.h>
// Was built against version 3.1

#define LEDPIN 17
#define LEDCOUNT 43

CRGB leds[LEDCOUNT+1];
int ledTweak[LEDCOUNT+1] = {
  0, 1, 2, 3, 4, 5, 6, 8, 10, 12,       12, 11, 10, 8, 6, 5, 4, 3, 2, 1, 0,
  0, 1, 2, 3, 4, 5, 6, 8, 10, 11, 12,   12, 11, 10, 8, 6, 5, 4, 3, 2, 1, 0
};

///////////// Test sequence ////////////
void boot_up() {
  int hue;
  for (int i=0; i < LEDCOUNT; i++) {
    hue = (i * 6)%256;
    leds[i] = CHSV(hue, 255, 128);
    FastLED.show();
    FastLED.delay(20);
  }
  FastLED.delay(2000);
}

inline CRGB rand_new_star(int chance) {
  if (random16(1000) > chance) {
    return CRGB::Black;
  }

  //int hue = random8(159, 255);
  int hue = random8(0, 255);

  int brightness = random8(128,255);

  return CHSV( hue, 255, brightness);
}

/////////////// Pulse effect /////////////

void pulse(int hue, int times) {
  int bright;

  for (int i=0; i < times; i++) {
    for (bright=0; bright <= 255; bright++) {
      fill_solid( leds, LEDCOUNT, CHSV( hue, 255, ease8InOutCubic(bright) ) );
      FastLED.show();
      FastLED.delay(9);
    }

    FastLED.delay(100);

    for (bright=255; bright >= 0; bright--) {
      if (bright%4 == 0 && hue > 0) { hue--; } // fade to different colour
      fill_solid( leds, LEDCOUNT, CHSV( hue, 255, ease8InOutCubic(bright) ) );
      FastLED.show();
      FastLED.delay(5);
    }

    if (times > 0) {
      FastLED.delay(500);
    }

  }
}

/////////////////////////////////

void setup() {
  delay(1000);
  randomSeed(analogRead(0));
  random16_set_seed(analogRead(0));

  FastLED.addLeds<NEOPIXEL, LEDPIN>(leds, LEDCOUNT);
  FastLED.setBrightness(48);
  // set to blank
  for (int i=0; i < LEDCOUNT; i++) {
    leds[i].setRGB(0,0,0);
  }

  FastLED.show();

  boot_up();
}


void loop() {
  const int top_hue = 0;
  const int low_hue = 29;
  static uint8_t top_easing = 0;
  static uint8_t low_easing = 0;
  static uint8_t top_easeIn  = 0;
  static uint8_t low_easeIn  = 64;


/*
  top_easing = ease8InOutQuad((easeInVal + 32)%256) / 3;
  low_easing = ease8InOutQuad(easeInVal) / 3;
  easeInVal++;
*/
  
  top_easing = quadwave8(top_easeIn) / 3;
  low_easing = quadwave8(low_easeIn) / 3;
  top_easeIn++;
  low_easeIn+=2;


  for (int i=0; i <= 20; i++) {
    leds[i] = CHSV(top_hue, 255, 120 + (4 * ledTweak[i]) + top_easing);
  }

  for (int i=21; i <= 42; i++) {
    leds[i] = CHSV(low_hue, 255, 120 + (4 * ledTweak[i]) + low_easing);
  }
 

  FastLED.delay(10);
}
