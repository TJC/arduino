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

void pulse(int hue) {
  int bright;

    for (bright=0; bright <= 255; bright++) {
      fill_solid( leds, LEDCOUNT, CHSV( hue, 255, ease8InOutCubic(bright) ) );
      FastLED.show();
      FastLED.delay(9);
    }

    FastLED.delay(500);

    for (bright=255; bright >= 0; bright--) {
      if (hue > 0) { hue--; } // fade to red
      fill_solid( leds, LEDCOUNT, CHSV( hue, 255, ease8InOutCubic(bright) ) );
      FastLED.show();
      FastLED.delay(5);
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
  return (random16(1000) < 7);
}

void twinkle_iter() {
  for (int i=0; i < LEDCOUNT; i++) {
    if (tStars[i].current == 0) {
      int h = random16(150, 315);
      if (h > 255) { h -= 255; }
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

  FastLED.delay(20);
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

  // all led counts +1 for paranoia and possible miscounting
  FastLED.addLeds<NEOPIXEL, 4>(leds,  0,  6);
  FastLED.addLeds<NEOPIXEL, 5>(leds,  6, 10); 
  FastLED.addLeds<NEOPIXEL, 6>(leds, 16,  6);
  FastLED.addLeds<NEOPIXEL, 7>(leds, 22,  9);

  // set to test pattern per strand
  for (int i=0; i < 6; i++) {
    leds[i].setRGB(255,0,0);
  }
  for (int i=6; i < 16; i++) {
    leds[i].setRGB(0,255,0);
  }
  for (int i=16; i < 22; i++) {
    leds[i].setRGB(0,0,255);
  }
  for (int i=22; i < LEDCOUNT; i++) {
    leds[i].setRGB(192,0,192);
  }

  FastLED.show();
  FastLED.delay(5000);

  boot_up();
}

int pulseColours[] = {64, 96, 160, 192, 255};
void loop() {
  pulse(64);
  twinkle(120000L);
  pulse(pulseColours[random8(5)]);
  twinkle(120000L);
}

