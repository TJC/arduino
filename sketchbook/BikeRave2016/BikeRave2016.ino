#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN 8
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];
bool midCopLights = false;

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup();
void loop();
void rainbow();
void nextPattern();
void rainbow();
void rainbowWithGlitter();
void addGlitter( fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void bpm2();
void juggle();
void copLights();

void setup() {
  delay(2000); // 2 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  randomSeed(analogRead(0));
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { bpm2, copLights, confetti, bpm, rainbowWithGlitter, juggle };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 5 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 30 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

inline void fill(CRGB col) {
  fill_solid(leds, NUM_LEDS, col);
  FastLED.show();  
}

CRGB primary_colours[] = {
  CRGB::Blue,
  CRGB::Blue,
  CRGB::Red,
  CRGB::Red,
  CRGB::Green,
  CRGB::Yellow,
  CRGB::Purple,
  CRGB::Black
};
#define num_pri_colours 8

void copLights()
{
  static int col1, col2;
  if (! midCopLights) {
    col2 = random8(num_pri_colours);
    col1 = random8(num_pri_colours);
    while (col2 == col1) {
      col2 = random8(num_pri_colours);
    }
    midCopLights = true;
  }

  for (int j=0; j<4; j++) {
    fill(primary_colours[col1]);
    delay(60);
    fill(CRGB::Black);
    delay(60);
  }

  for (int j=0; j<4; j++) {
    fill(primary_colours[col2]);
    delay(60);
    fill(CRGB::Black);
    delay(60);
  }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // XXX hack, reset double blink effect
  if (midCopLights) { midCopLights = false; }

  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 12);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 130;
  CRGBPalette16 palette = HeatColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 32, 240);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, (i*2), beat+(i*16));
  }
}

void bpm2()
{
  uint8_t BeatsPerMinute = 130;
  CRGBPalette16 palette = HeatColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 0, 160);
  fill_solid(leds, NUM_LEDS,
    ColorFromPalette(palette, beat, 255)
  );
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

