#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN 8
#define NUM_LEDS    22
CRGB leds[NUM_LEDS];

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
void resetSinelonColour();
void bpm();
void bpm2();
void juggle();
void firey();
void gentlePulse();

void setup() {
  delay(2000); // 2 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setDither(1);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  randomSeed(analogRead(0));
  random16_set_seed(analogRead(0));
  resetSinelonColour();
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { gentlePulse, firey, sinelon, bpm2, confetti, juggle };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
bool fireStarted = false;
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 5 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 90 ) {
    resetSinelonColour();
    fireStarted = false;
    nextPattern();
  } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
  if ( gPatterns[gCurrentPatternNumber] == sinelon) {
    FastLED.setBrightness(128);
  }
  else if (gPatterns[gCurrentPatternNumber] == gentlePulse) {
    FastLED.setBrightness(50);
  }
  else {
    FastLED.setBrightness(BRIGHTNESS);
  }
}

inline void fill(CRGB col) {
  fill_solid(leds, NUM_LEDS, col);
  FastLED.show();  
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
  addGlitter(30);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{

  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 40);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

CRGB sinelon_colour = CRGB::Red;

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 12);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += sinelon_colour;
}

void resetSinelonColour()
{
  static const uint8_t solid_colours[] = {
    0,    // red
    96,   // green
    160,  // blue
    192,  // purple
    224   // pink
  };
  sinelon_colour = CRGB(CHSV(solid_colours[random(0,5)], 255, 255));
}

void gentlePulse() {
  uint8_t BeatsPerMinute = 10;
  uint8_t beat = beatsin8( BeatsPerMinute, 0, 255);

  CRGB col = ColorFromPalette(HeatColors_p, beat);
  fill_solid(leds, NUM_LEDS, col);
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
  // two colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 2; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
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
#define COOLING  70

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 60

// CRGB HeatColor( uint8_t temperature)
// *** G and R swapped! ***
//
// Approximates a 'black body radiation' spectrum for
// a given 'heat' level.  This is useful for animations of 'fire'.
// Heat is specified as an arbitrary scale from 0 (cool) to 255 (hot).
// This is NOT a chromatically correct 'black body radiation'
// spectrum, but it's surprisingly close, and it's fast and small.
//
// On AVR/Arduino, this typically takes around 70 bytes of program memory,
// versus 768 bytes for a full 256-entry RGB lookup table.

CRGB HeatColorGRB( uint8_t temperature)
{
    CRGB heatcolor;

    // Scale 'heat' down from 0-255 to 0-191,
    // which can then be easily divided into three
    // equal 'thirds' of 64 units each.
    uint8_t t192 = scale8_video( temperature, 191);

    // calculate a value that ramps up from
    // zero to 255 in each 'third' of the scale.
    uint8_t heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2; // scale up to 0..252

    // now figure out which third of the spectrum we're in:
    if( t192 & 0x80) {
        // we're in the hottest third
        heatcolor.r = 192; // full red
        heatcolor.g = 255; // full green ahem
        heatcolor.b = heatramp; // ramp up blue

    } else if( t192 & 0x40 ) {
        // we're in the middle third (r and g swapped)
        heatcolor.r = heatramp;
        heatcolor.g = 255;
        heatcolor.b = 0; // no blue

    } else {
        // we're in the coolest third (r and g swapped)
        heatcolor.r = 0;
        heatcolor.g = heatramp;
        heatcolor.b = 0; // no blue
    }

    return heatcolor;
}


void firey() {
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  if (! fireStarted) {
    memset(heat, 0, NUM_LEDS);
    fireStarted = true;
  }


  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

  // Step 4.  Map from heat cells to LED colors
  // Note that on my trousers, 0 is at the top, maximum is at base, so we reverse
  for ( int j = 0; j < NUM_LEDS; j++) {
    CRGB colour = HeatColorGRB( heat[j] );
    leds[NUM_LEDS - j - 1] = colour;
  }
}


