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
  delay(2000);
}

inline CRGB rand_new_star(int chance) {
  if (random16(1000) > chance) {
    return CRGB::Black;
  }

  int hue = random8(159, 255);

  int brightness = random8(128,255);

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
  idletime = initialtime + 10L*1000L;
  maxspeed = initialtime + 15L*1000L;
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
      FastLED.delay(45 - (MAXSPEED-speed));
    }
    else {
      FastLED.delay(45 - speed);
    }

    cTime = millis();
  }
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
  return (random16(1000) < 7);
}

void twinkle_iter() {
  for (int i=0; i < LEDCOUNT; i++) {
    if (tStars[i].current == 0) {
      int h = random16(150,315);
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

  FastLED.addLeds<NEOPIXEL, LEDPIN>(leds, LEDCOUNT);

  // set to blank
  for (int i=0; i < LEDCOUNT; i++) {
    leds[i].setRGB(0,0,0);
  }

  FastLED.show();

  boot_up();
}


void loop() {
  pulse(random8(64,255), 1);
  twinkle(20000L);
  warpspeed(false);
  pulse(random8(64,255), 1);
  FastLED.setBrightness(64);
  pulse(0, 16);
  FastLED.setBrightness(64);
}
