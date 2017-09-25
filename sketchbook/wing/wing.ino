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

/////////// Fire effect /////////////
// Could also be bubble effect with different pallette
// taken from FastLED example and modified
// ideally should only apply to vertical strips, and should be applied
// separately to each one

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING  40

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 50

#define FIRE_NUM_LEDS 23

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
    int y = random8(3);
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

  // Step 4.  Map from heat cells to LEDs on the wing..
  // XXX this is awful, jesus. Don't judge me!
  /*
  20 19 18 17 16 15 14 13 12 11 10 /  09 08 07 06 05 04 03 02 01 00
21 22 23 24 25 26 27 28 29 30 31   /    32 33 34 35 36 37 38 39 40 41 42
  */

  int j = 0;
  leds[10] = leds[9] = HeatColor(heat[j+=2]);
  leds[11] = leds[8] = HeatColor(heat[j+=2]);
  leds[12] = leds[7] = HeatColor(heat[j+=2]);
  leds[13] = leds[6] = HeatColor(heat[j+=2]);
  leds[14] = leds[5] = HeatColor(heat[j+=2]);
  leds[15] = leds[4] = HeatColor(heat[j+=2]);
  leds[16] = leds[3] = HeatColor(heat[j+=2]);
  leds[17] = leds[2] = HeatColor(heat[j+=2]);
  leds[18] = leds[1] = HeatColor(heat[j+=2]);
  leds[19] = leds[0] = HeatColor(heat[j+=2]);
  leds[20] = HeatColor(heat[j]);

  j = 1;
  leds[31] = leds[32] = HeatColor(heat[j+=2]);
  leds[30] = leds[33] = HeatColor(heat[j+=2]);
  leds[29] = leds[34] = HeatColor(heat[j+=2]);
  leds[28] = leds[35] = HeatColor(heat[j+=2]);
  leds[27] = leds[36] = HeatColor(heat[j+=2]);
  leds[26] = leds[37] = HeatColor(heat[j+=2]);
  leds[25] = leds[38] = HeatColor(heat[j+=2]);
  leds[24] = leds[39] = HeatColor(heat[j+=2]);
  leds[23] = leds[40] = HeatColor(heat[j+=2]);
  leds[22] = leds[41] = HeatColor(heat[j+=2]);
  leds[21] = leds[42] = HeatColor(heat[j]);

}

void start_fire(unsigned long duration) {
  unsigned long endpoint = millis() + duration;

  for (int i=0; i < LEDCOUNT; i++) {
    leds[i].setRGB(0,0,0);
  }
  FastLED.show(); // display this frame
  FastLED.delay(10); // flush octows2811

  while (millis() < endpoint) {
    fire_iter();
    FastLED.show();
    FastLED.delay(30);
  }
}


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



void glow_iter() {
  const int top_hue = 0;
  const int low_hue = 29;
  static uint8_t top_easing = 0;
  static uint8_t low_easing = 0;
  static uint8_t top_easeIn  = 0;
  static uint8_t low_easeIn  = 64;

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

  FastLED.show();
  FastLED.delay(10);
}

void red_glow(unsigned long duration) {
    unsigned long endpoint = millis() + duration;
    while (millis() < endpoint) {
      glow_iter();
    }
}



void loop() {
    start_fire(180000);
    red_glow(60000);
}

