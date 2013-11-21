#include "Arduino.h"
#include "LiquidCrystal.h"
#include <Adafruit_NeoPixel.h>
#include <math.h>

#define SPEAKERPIN 8
#define NEOPIN 9

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, NEOPIN, NEO_GRB + NEO_KHZ800);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned long cur_millis = 0;

/* Returns batshit insane values. WHY?!

const uint16_t segment_lookup[15] = {
    16, 32, 48, 96,
    128, 160, 192, 256,
    384, 448, 512, 720,
    800, 900, 2000
};

// min returned = 1
// max returned = 15
uint16_t num_segments(uint16_t val) {
    uint16_t i = 0;
    while (val < segment_lookup[i]) { i++; }
    return i+1;  // ie. max will be 15
}
*/

uint16_t num_segments(uint16_t val) {
    // uint16_t i;
    // for (i=0; val < segment_lookup[i]; i++);
    // return(i+1);  // ie. max will be 15
    if (val < 24) { return 1; }
    else if (val < 48) { return 2; }
    else if (val < 96) { return 3; }
    else if (val < 128) { return 4; }
    else if (val < 160) { return 5; }
    else if (val < 192) { return 6; }
    else if (val < 256) { return 7; }
    else if (val < 384) { return 8; }
    else if (val < 448) { return 9; }
    else if (val < 512) { return 10; }
    else if (val < 600) { return 11; }
    else if (val < 700) { return 12; }
    else if (val < 800) { return 13; }
    else if (val < 900) { return 14; }
    else return 15;
}

// Fill the dots one after the other with a color
void colourWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void setup() {
    Serial.begin(38400);
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    lcd.begin(16,2);
    pinMode(SPEAKERPIN, OUTPUT);
    digitalWrite(SPEAKERPIN, LOW);

    colourWipe(strip.Color(0,128,0), 125);
}

void display_bar_graph(uint16_t val) {
    uint16_t segments; 
    char buf[40];
    static unsigned long elapsed = 0;
    static unsigned char flash_state = 0;

    if (cur_millis - elapsed < 800) {
        return;
    }
    elapsed = cur_millis;

    segments = num_segments(val);

    snprintf(buf, 40, "r:%04d seg:%02d", val, segments);
    Serial.println(buf);


    lcd.setCursor(0, 0);
    if (flash_state) {
        flash_state = 0;
        if (segments < 3) {
            lcd.print("*   SAFE LEVEL *");
        }
        else if (segments < 8) {
            lcd.print("*   WARNING    *");
        }
        else {
            lcd.print("**  !DANGER!  **");
        }
    }
    else { // initial state:
        lcd.print("*   RADIATION  *");
        flash_state = 1;
    }

    for (uint16_t i=0; i<=segments; i++) {
        buf[i] = '#';
    }
    buf[segments+1] = '>';
    for (uint16_t i=segments+2; i<16; i++) {
        buf[i] = ' '; // blank out previous stuff
    }
    buf[16] = '\0';

    lcd.setCursor(0, 1);
    lcd.print(buf);
}

void flash_danger() {
    lcd.setCursor(0, 1);
    lcd.print("-=== DANGER ===-");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("-===        ===-");
    delay(1000);
}

void clicker(uint16_t val) {
    static unsigned long last_click = 0;
    if (cur_millis < last_click + 1050-val) {
        return;
    }

    tone(SPEAKERPIN, 4000, 5);

    unsigned int cdelay = rand() % (1250-val);
    last_click = cur_millis + cdelay; // adds some random timing
}

// Actually more like a mangled, offset sine..
inline static short val_to_led_scale_128(uint16_t val) {
    if (val < 256) {
        return 0;
    }
    else if (val > 512) {
        return 128;
    }
    return int(128.0 * sinf(1.5707 * float(val-256)/256.0));
}

void cycle_leds(uint16_t val) {
    static uint8_t next_position = 0;
    static uint8_t dir_fwd = 0;
    static unsigned long elapsed = 0;
    static int8_t colour_adj = 0;
    static uint8_t colour_dir = 1;
    int16_t red, green;

    // 0-256, but exponentially
    // uint16_t expval = int( powf(float(val) / 64.0, 2) );
    // if (cur_millis - elapsed < (158 - (expval/2))) { return; }
    
    uint16_t delayer = 50 + 64 - (num_segments(val) * 4);

    if (cur_millis - elapsed < delayer) { return; }
    elapsed = cur_millis;

    red = val_to_led_scale_128(val) - colour_adj;
    green = val_to_led_scale_128(1023-val) - colour_adj;

    // cap values at moderately-high brightness:
    if (green > 128) { green = 129; }
    if (red > 128) { red = 129; }
    // cap underflows
    if (red < 0) { red = 0; }
    if (green < 0) { green = 0; }

    uint32_t fillcolour = strip.Color(red, green, 0);

    for (uint8_t i=0; i<8; i++) {
        strip.setPixelColor(i, fillcolour);
    }
    strip.setPixelColor(next_position, strip.Color(0,0,255));
    strip.show();

    if (dir_fwd) {
        next_position++;
        if (next_position > 7) {
            next_position = 6;
            dir_fwd = 0;
        }
    }
    else {
        next_position--;
        if (next_position > 7) {
            next_position = 1;
            dir_fwd = 1;
        }
    }

    if (colour_dir) {
        colour_adj += 3;
    }
    else {
        colour_adj -= 3;
    }
    if (colour_adj >= 48) { colour_dir = 0; }
    if (colour_adj <= 3) { colour_dir = 1; }
}

void loop() {
    uint16_t alc;

    cur_millis = millis();

    alc = analogRead(5);
    display_bar_graph(alc);
    cycle_leds(alc);
    clicker(alc);
}

