#include "Arduino.h"
#include <stdlib.h>
#include <FastLED.h>
// Was built against version 3.1

#define LEDPIN 8
#define LEDCOUNT 150

CRGB leds[LEDCOUNT];

///////////// Test sequence ////////////
void boot_up() {
  int hue;
  for (int j=0; j < LEDCOUNT; j++) {
    for (int i=0; i < j; i++) {
      hue = (i * 4)%256;
      leds[i] = CHSV(hue, 255, 128);
    }
    FastLED.show();
    delay(50);
  }
  delay(1000);
}

inline CRGB rand_new_star(int chance) {
  if (rand()%1000 > chance) {
    return CRGB::Black;
  }

  // Blue starts from around 150, goes up to 255 (red), drops back to zero and goes to 60 (orange)
  // So range of 165
  int hue = rand()%165 + 150;
  if (hue > 255) {
    hue -= 255;
  }

  int brightness = rand()%128+127;

  return CHSV( hue, 255, brightness);
}

////////////////// Warp sequence ////////////////

#define MINSPEED 1
#define MAXSPEED 40
void warpspeed(bool decelerate) {
  unsigned long initialtime, maxspeed, endpoint, cTime;
  int speed;

  fill_solid( leds, LEDCOUNT, CRGB::Black ); // yellow
  // Maybe scatter a few random stars around first?

  initialtime = millis();
  maxspeed = initialtime + 30L*1000L;
  endpoint = maxspeed + 30L*1000L;

  cTime = millis();
  while (cTime < endpoint) {
    // Shift all points away from us, by one:
    for (int i=LEDCOUNT; i > 1; i--) {
      leds[i] = leds[i-1];
    }

    // Set end point to new random setting:
    leds[0] = rand_new_star(100);

    FastLED.show();

    if (cTime >= maxspeed && cTime < endpoint) {
      speed = MAXSPEED;
    }
    else if (cTime < maxspeed) {
      speed = MAXSPEED * (cTime - initialtime) / (maxspeed - initialtime);
    }

    if (decelerate) {
      delay(50 - (MAXSPEED-speed));
    }
    else {
      delay(50 - speed);
    }

    cTime = millis();
  }
}

/////////////// Pulse effect /////////////

void pulse() {
  int bright;
  for (bright=0; bright <= 255; bright++) {
    fill_solid( leds, LEDCOUNT, CHSV( 64, 255, bright) ); // yellow
    FastLED.show();
    delay(15);
  }

  delay(750);

  for (bright=255; bright >= 0; bright--) {
    fill_solid( leds, LEDCOUNT, CHSV( 64, 255, bright) ); // yellow
    FastLED.show();
    delay(15);
  }

  delay(750);
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
  return (rand()%1000 < 4);
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

void twinkle(int duration) {
  unsigned long endpoint = millis() + duration;

  for (int i=0; i < LEDCOUNT; i++) {
    tStars[i].current = 0;
    tStars[i].target = 0;
  }

  while (millis() < endpoint) {
    twinkle_iter();
  }

  // TODO: Fade all out.
}

/////////////////////////////////

void setup() {
  randomSeed(analogRead(0));

  FastLED.addLeds<NEOPIXEL, LEDPIN>(leds, LEDCOUNT);

  // set to blank
  for (int i=0; i < LEDCOUNT; i++) {
    leds[i].setRGB(0,0,0);
  }

  FastLED.show();

  boot_up();
}


void loop() {
  twinkle(10000);
  warpspeed(false);
  pulse();
  warpspeed(true);
}

