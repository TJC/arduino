#include "Arduino.h"
#include <stdlib.h>
#include <FastLED.h>
// Was built against version 3.1

#define LEDCOUNT 32

CRGB leds[LEDCOUNT+1];

///////////// Test sequence ////////////
void boot_up() {
  int hue;
  for (int i=0; i < LEDCOUNT; i++) {
    hue = (i * 8)%256;
    leds[i] = CHSV(hue, 255, 128);
    FastLED.show();
    delay(20);
  }
  delay(2000);
}

/////////////// Pulse effect /////////////

void pulse() {
  int bright;
  for (bright=0; bright <= 255; bright++) {
    fill_solid( leds, LEDCOUNT, CHSV( 64, 255, bright) ); // yellow
    FastLED.show();
    delay(6);
  }

  delay(750);

  for (bright=254; bright >= 0; bright-=2) {
    fill_solid( leds, LEDCOUNT, CHSV( 64, 255, bright) ); // yellow
    FastLED.show();
    delay(6);
  }
}

/////////// Twinkle effect /////////////
typedef struct {
  int16_t current;
  int16_t target;
  int8_t rate;
  uint8_t hue;
} TwinkleStars;

TwinkleStars tStars[LEDCOUNT+1];

// Chance of starting a new twinkle (per tick, which is ~20ms)
inline bool rand_new_led() {
  return (rand()%1000 < 7);
}

void twinkle_iter() {
  for (int i=0; i < LEDCOUNT; i++) {
    if (tStars[i].current == 0) {
      int h = rand()%165 + 150;
      if (h > 255) { h -= 255; }
      if (rand_new_led()) {
        tStars[i].current = 1;
        tStars[i].rate = 3 + rand()%5;
        tStars[i].target = 96 + rand()%159;
        tStars[i].hue = h;
      }
    }
    else {
      tStars[i].current += tStars[i].rate;
      if (tStars[i].current <= 0) {
        tStars[i].current = 0;
      }
      else if (tStars[i].current > tStars[i].target) {
        tStars[i].current = tStars[i].target;
        tStars[i].rate = -tStars[i].rate;
      }
    }
  }

  for (int i=0; i < LEDCOUNT; i++) {
    leds[i] = CHSV(tStars[i].hue, 128, tStars[i].current);
  }

  FastLED.show();

  delay(20);
}

void twinkle(unsigned long duration) {
  unsigned long endpoint = millis() + duration;

  for (int i=0; i < LEDCOUNT; i++) {
    tStars[i].current = 0;
    tStars[i].target = 0;
  }

  while (millis() < endpoint) {
    twinkle_iter();
  }

  // TODO: Fade all out.
  // Or not.. if left in, the warp mode works nicely.
}

/////////////////////////////////

void setup() {
  randomSeed(analogRead(0));

  // all led counts +1 for paranoia
  FastLED.addLeds<NEOPIXEL, 4>(leds, 6);
  FastLED.addLeds<NEOPIXEL, 5>(&leds[6], 10); 
  FastLED.addLeds<NEOPIXEL, 6>(&leds[16], 6);
  FastLED.addLeds<NEOPIXEL, 7>(&leds[22], 9);

  // set to blank
  for (int i=0; i < LEDCOUNT; i++) {
    leds[i].setRGB(0,0,0);
  }

  FastLED.show();

  boot_up();
}


void loop() {
  pulse();
  twinkle(60000L);
}

