#include "Arduino.h"
#include <stdlib.h>
#include <FastLED.h>
// Was built against version 3.1

// Notes:
// HSV Blue is ~140 to 180 (going from cyan end to purple end)
// HSV yellow=64, red is backwards at 0
// Pin layouts on the teensy 3/3.1:
// WS2811_PORTD: 2,14,7,8,6,20,21,5

#define NUM_LEDS_PER_STRIP 150
#define NUM_STRIPS 8
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

#define LEDCOUNT NUM_STRIPS * NUM_LEDS_PER_STRIP
// CRGB leds[LEDCOUNT+1];

///////////// Test sequence ////////////
void boot_up() {
  int hue;
  for (int i=0; i < LEDCOUNT; i++) {
    hue = (i * 4)%256;
    leds[i] = CHSV(hue, 255, 128);
    FastLED.show();
    FastLED.delay(5);
  }
  delay(2000);
}

/////////////// Pulse effect /////////////

void pulse(int times) {
  int bright;
  int hue;
  int delta;
  int final_hue;

  switch (rand()%3) {
    case 0: hue = 160; delta=0; break; // straight blue
    case 1: hue = 65; delta = -1; break; // yellow to red
    case 2: hue = 96; delta = 1; break; // green to blue
  }

  for (int i=0; i < times; i++) {
    for (bright=0; bright <= 255; bright++) {
      final_hue = hue;
      if (bright >= 191) {
        final_hue +=  delta * (bright - 191);
      }
      fill_solid( leds, LEDCOUNT, CHSV( final_hue, 255, ease8InOutCubic(bright) ) );
      FastLED.show();
      FastLED.delay(13);
    }

    for (bright=255; bright >= 0; bright--) {
      final_hue = hue;
      if (bright >= 191) {
        final_hue +=  delta * (bright - 191);
      }
      fill_solid( leds, LEDCOUNT, CHSV( final_hue, 255, ease8InOutCubic(bright) ) );
      FastLED.show();
      FastLED.delay(9);
    }

    if (times > 1) {
      FastLED.delay(500);
    }
  }

}

/////////// Twinkle effect /////////////
typedef struct {
  int16_t current;
  int16_t target;
  int8_t rate;
  uint8_t hue;
} TwinkleStars;

TwinkleStars tStars[LEDCOUNT];

// Chance of starting a new twinkle (per tick, which is ~20ms)
inline bool rand_new_led() {
  return (random8() < 3);
}

void twinkle_iter() {
  for (int i=0; i < LEDCOUNT; i++) {
    if (tStars[i].current == 0) {
      int h = random8(140,180);
      // if (h > 255) { h -= 255; }
      if (rand_new_led()) {
        tStars[i].current = 1;
        tStars[i].rate = random8(3,8);
        tStars[i].target = random8(96,255);
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
  delay(1000);
  randomSeed(analogRead(0));
  random16_set_seed(analogRead(0));

  FastLED.addLeds<WS2811_PORTD,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);

  // set to blank
  for (int i=0; i < LEDCOUNT; i++) {
    leds[i].setRGB(0,0,0);
  }

  // Set the first n leds on each strip to show which strip it is
  for(int i = 0; i < NUM_STRIPS; i++) {
    for(int j = 0; j <= i; j++) {
      leds[(i*NUM_LEDS_PER_STRIP) + j] = CRGB::Red;
    }
  }

  FastLED.show();
  FastLED.delay(5000);

  boot_up();
}


void loop() {
  pulse(1);
  pulse(7);
  twinkle(30000L);
  // warpspeed(false);
}
