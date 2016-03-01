#include "Arduino.h"
#include <stdlib.h>
#include <FastLED.h>
// Was built against version 3.1.0

/*
 * Anglerfish LED code. Assumes use of OctoWS2811 PortD pins via adapter board.
 * Was thrown together quickly due to rather short amounts of time
 * to spend working on this; would love to have had more time.
 * One extra effect I have planned is "bubbles" travelling upwards along
 * vertically mounted LED strips.
* Longest strip is 3950mm = 118 or 119 LEDs
 * - Toby
 */

// Notes:
// HSV Blue is ~140 to 180 (going from cyan end to purple end)
// HSV yellow=64, red is backwards at 0 or 255
// Pin layouts on the teensy 3.1:
// WS2811_PORTD: 2,14,7,8,6,20,21,5

#define NUM_LEDS_PER_STRIP 120
#define NUM_STRIPS 8
#define LEDCOUNT (8*120)

CRGB leds[LEDCOUNT+1];


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
      if (bright > 190) {
        final_hue +=  delta * (bright - 191);
      }
      fill_solid( leds, LEDCOUNT, CHSV( final_hue, 255, ease8InOutCubic(bright) ) );
      FastLED.show();
      FastLED.delay(12);
    }

    for (bright=255; bright >= 0; bright--) {
      final_hue = hue;
      if (bright > 190) {
        final_hue +=  delta * (bright - 191);
      }
      fill_solid( leds, LEDCOUNT, CHSV( final_hue, 255, ease8InOutCubic(bright) ) );
      FastLED.show();
      FastLED.delay(8);
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

}

/////////// Fire effect /////////////
// Could also be bubble effect with different pallette
// taken from FastLED example and modified
// ideally should only apply to vertical strips, and should be applied
// separately to each one

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING  60

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 65

#define FIRE_NUM_LEDS 120

bool gReverseDirection = false;
void fire_iter() {
  // Array of temperature readings at each simulation cell
  static byte heat[FIRE_NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < FIRE_NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / FIRE_NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= FIRE_NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for( int j = 0; j < FIRE_NUM_LEDS; j++) {
    CRGB color = HeatColor( heat[j]);
    int pixelnumber;
    if( gReverseDirection ) {
      pixelnumber = (FIRE_NUM_LEDS-1) - j;
    } else {
      pixelnumber = j;
    }
    // duplicate this over all strands:
    for (int k=0; k<8; k++) {
      leds[k*NUM_LEDS_PER_STRIP + pixelnumber] = color;
    }
  }
}

void start_fire(unsigned long duration) {
  unsigned long endpoint = millis() + duration;

  for (int i=0; i < LEDCOUNT; i++) {
    leds[i].setRGB(0,0,0);
  }
  FastLED.show(); // display this frame
  FastLED.delay(10); // flush octows2811

  FastLED.setBrightness(128);

  while (millis() < endpoint) {
    fire_iter();
    FastLED.show();
    FastLED.delay(25);
  }

  FastLED.setBrightness(255);
}



/////////////////////////////////

void setup() {
  delay(1000);
  randomSeed(analogRead(0));
  random16_set_seed(analogRead(0));

  FastLED.addLeds<WS2811_PORTD,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);

  FastLED.setDither(0); // I don't think it'll work right with OctoWS2811's async show

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
  pulse(2);
  twinkle(60000L);
  pulse(3);
  pulse(2);
  pulse(1);
  start_fire(60000L);
}
