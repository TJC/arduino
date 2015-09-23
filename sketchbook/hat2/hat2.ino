#include "Arduino.h"
#include <stdlib.h>
#include <FastLED.h>
// Was built against version 3.1

#define LEDPIN 17
#define LEDCOUNT 32

CRGB leds[LEDCOUNT+1];

///////////// Test sequence ////////////
void boot_up() {
  int hue;
  for (int i=0; i < LEDCOUNT; i++) {
    hue = (i * 4)%256;
    leds[i] = CHSV(hue, 255, 128);
    FastLED.show();
    delay(20);
  }
  delay(500);
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

  return CHSV( hue, 160, brightness);
}

////////////////// Warp sequence ////////////////

#define MINSPEED 1
#define MAXSPEED 40
void warpspeed(bool decelerate) {
  unsigned long initialtime, idletime, maxspeed, endpoint, cTime;
  int speed;

  // fill_solid( leds, LEDCOUNT, CRGB::Black ); // yellow
  // Maybe scatter a few random stars around first?

  initialtime = millis();
  idletime = initialtime + 5L*1000L;
  maxspeed = initialtime + 20L*1000L;
  endpoint = maxspeed + 5L*1000L;

  cTime = millis();
  while (cTime < endpoint) {
    // Shift all points away from us, by one:
    for (int i=LEDCOUNT; i > 0; i--) {
      leds[i] = leds[i-1];
    }

    // Set end point to new random setting:
    leds[0] = rand_new_star(100);

    FastLED.show();

    if (cTime <= idletime) {
      speed = 0;
    }
    else if (cTime >= maxspeed && cTime < endpoint) {
      speed = MAXSPEED;
    }
    else if (cTime < maxspeed) {
      speed = MAXSPEED * (cTime - initialtime) / (maxspeed - initialtime);
    }

    if (decelerate) {
      delay(45 - (MAXSPEED-speed));
    }
    else {
      delay(45 - speed);
    }

    cTime = millis();
  }
}

/////////////// Pulse effect /////////////

void pulse() {
  int bright;
  int hue = rand()%256;
  for (bright=0; bright <= 255; bright++) {
    fill_solid( leds, LEDCOUNT, CHSV( hue, 255, bright) );
    FastLED.show();
    delay(6);
  }

  delay(500);

  for (bright=254; bright >= 0; bright-=2) {
    fill_solid( leds, LEDCOUNT, CHSV( hue, 255, bright) );
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

TwinkleStars tStars[LEDCOUNT];

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

  FastLED.addLeds<NEOPIXEL, LEDPIN>(leds, LEDCOUNT);

  // set to blank
  for (int i=0; i < LEDCOUNT; i++) {
    leds[i].setRGB(0,0,0);
  }

  FastLED.show();

  boot_up();
}


void loop() {
  twinkle(10000L);
  warpspeed(false);
  pulse();
  // warpspeed(true);
}

