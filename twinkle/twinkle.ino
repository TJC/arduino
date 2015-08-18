#include <stdlib.h>
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define NEOPIN 8
#define LEDCOUNT 150

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDCOUNT, NEOPIN, NEO_GRB + NEO_KHZ800);
// strip.setPixelColor(i, c)
// strip.setPixelColor(i, r, g, b)


typedef struct {
  int16_t current;
  int16_t target;
  int16_t rate;
  int16_t cOffset; // colour offset
} LedItem;

LedItem ledItems[LEDCOUNT];

void setup() {
  randomSeed(analogRead(0));

  strip.begin();
  strip.show();

  for (int i=0; i < LEDCOUNT; i++) {
    ledItems[i].current = 0;
    ledItems[i].cOffset = 0;
  }
}

// Chance of starting a new twinkle (per tick, which is ~20ms)
bool rand_new_led() {
  return (rand()%1000 < 4);
}

void loop() {
  for (int i=0; i < LEDCOUNT; i++) {
    if (ledItems[i].current == 0) {
      if (rand_new_led()) {
        ledItems[i].current = 1;
        ledItems[i].rate = 3 + rand()%5;
        ledItems[i].target = 96 + rand()%159;
        ledItems[i].cOffset = -30 + ( rand() % 60 );
      }
    }
    else {
      ledItems[i].current += ledItems[i].rate;
      if (ledItems[i].current <= 0) {
        ledItems[i].current = 0;
        ledItems[i].cOffset = 0;
      }
      else if (ledItems[i].current > ledItems[i].target) {
        ledItems[i].current = ledItems[i].target;
        ledItems[i].rate = -ledItems[i].rate;
      }
    }
  }

  for (int i=0; i < LEDCOUNT; i++) {
    int16_t blue = ledItems[i].current;
    int16_t redgreen = ledItems[i].current;
    if (ledItems[i].cOffset > 0) {
      redgreen -= ledItems[i].cOffset;
    }
    else if (ledItems[i].cOffset < 0) {
      blue += ledItems[i].cOffset;
    }
    if (blue < 0) { blue = 0; }
    if (redgreen < 0) { redgreen = 0; }
    uint32_t c = strip.Color(redgreen, redgreen, blue);
    strip.setPixelColor(i, c);
  }

  strip.show();
  delay(20);
}

