#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

// OutputPin* - toggled here when there's enough power

#undef TCDEBUG
#define NEOPIN 2
#define NEOLEDCOUNT 150

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOLEDCOUNT, NEOPIN, NEO_GRB + NEO_KHZ800);

int redled = 17; // RXLED on micro pro
int greenled = 9;
int blueled = 10;
int sensorPin = A1;
int outputPinA = 5;
int outputPinB = 6;
bool long_running = false;

void led_setup(int pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
}

void enable_relay(int pin) {
    digitalWrite(pin, LOW);
}

void disable_relay(int pin) {
    digitalWrite(pin, HIGH);
}

void blank_leds() {
    static int leds[3] = { redled, greenled, blueled };
    for (int i=0; i<3; i++) {
        digitalWrite(leds[i], LOW);
    }
}

void enable_one_led(int pin) {
    blank_leds();
    digitalWrite(pin, HIGH);
}

void flash_one_led(int pin) {
    static int status = 0;
    blank_leds();
    if (status) {
        status=0;
    }
    else {
        status = 1;
        digitalWrite(pin, HIGH);
    }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void WheelArray(byte WheelPos, uint8_t *col) {
    if(WheelPos < 85) {
        col[0] = WheelPos * 3;
        col[1] = 255 - WheelPos * 3;
        col[2] = 0;
    } else if(WheelPos < 170) {
        WheelPos -= 85;
        col[0] = 255;
        col[1] = 0;
        col[2] = WheelPos * 3;
    } else {
        WheelPos -= 170;
        col[0] = 0;
        col[1] = WheelPos * 3;
        col[2] = 255 - WheelPos * 3;
    }
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void setup() {
    int leds[] = { redled, greenled, blueled };

    randomSeed(analogRead(0));

    // Enable INPUT+pull-up on all pins; this does save ~10mA
    // skip setting pin=2 as that's now neopixel output
    for (int i=3; i <= 19; i++) {
        pinMode(i, INPUT);
        digitalWrite(i, HIGH); // enables pull-up on input apparently?
    }

    // Now setup the ones we actually want to output upon..
    pinMode(outputPinA, OUTPUT);
    pinMode(outputPinB, OUTPUT);
    disable_relay(outputPinA);
    disable_relay(outputPinB);
    for (int i=0; i<3; i++) {
        led_setup(leds[i]);
    }

    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    // strip.setBrightness(32); // start with a conservatively-low brightness

#ifdef TCDEBUG
    Serial.begin(38400);
#endif

    // make it obvious we've booted up:
    colorWipe(strip.Color(128, 0, 0), 10); 
    colorWipe(strip.Color(0, 128, 0), 10);
    colorWipe(strip.Color(0, 0, 128), 10);
}

// Low Power Delay.  Drops the system clock
// to its lowest setting and sleeps for 256*quarterSeconds milliseconds.
void lpDelay(int quarterSeconds) {
    // clock_div_t previous_speed;
    // previous_speed = clock_prescale_get();

    delay(16); // required or else shit seems to go wrong

    clock_prescale_set(clock_div_256); // 1/256 speed == 60KHz

    delay(quarterSeconds);  // since the clock is slowed way down, delay(n) now acts like delay(n*256)

    //clock_prescale_set(previous_speed); // restore original speed
    clock_prescale_set(clock_div_1); // back to full speed

    delay(16); // required or else shit seems to go wrong
}


inline uint16_t abs_led_position(int16_t p) {
    if (p >= 0) {
        return p % NEOLEDCOUNT;
    }
    return NEOLEDCOUNT - ( abs(p) % NEOLEDCOUNT );
}


void spinCycle() {
  static int16_t i = 0;
  static uint32_t black = strip.Color(0,0,0);
  int16_t j, position;
  uint8_t col[3];

  position = i % NEOLEDCOUNT;

  WheelArray(i%255, col);

  // blank stuff out:
  for (j=32; j > 15; j--) {
      strip.setPixelColor(abs_led_position(position - j), black);
  }

  strip.setPixelColor(position-1, col[0], col[1], col[2]);
  strip.setPixelColor(position, col[0], col[1], col[2]);
  strip.setPixelColor(position+1, col[0], col[1], col[2]);
  for (j=2; j <= 16; j++) {
      uint32_t c = strip.Color(col[0] >> (j/2), col[1] >> (j/2), col[2] >> (j/2));
      strip.setPixelColor(abs_led_position(position + j), c);
      strip.setPixelColor(abs_led_position(position - j), c);
  }

  strip.show();
  i++;
}

void blank_strip() {
    for(int i=0; i < NEOLEDCOUNT; i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
    strip.show();
}


void loop() {
    int accval = 0;
    int rawval;
    float voltage;

    for (int i=0; i<3; i++) {
        rawval = analogRead(sensorPin);
#ifdef TCDEBUG
        Serial.print("Raw ADC: ");
        Serial.println(rawval);
#endif
        accval += rawval;
        delay(5);
    }

#ifdef TCDEBUG
    Serial.print("Average ADC: ");
    Serial.println(accval/3.0);
#endif

    // Using a voltage divider means we get a third of the real
    // value. We then take 3 readings, though, accumulating them.
    // Multiply by 5 given that analogRead's 1024=5V
    // Then finally divide the result out to get to the 12V reading.
    voltage = float(accval) / float(208.0);

#ifdef TCDEBUG
    Serial.print("Voltage: ");
    Serial.println(voltage); // automatically fixed to 2 decimal places
#endif  

    if (voltage > 13.1) {
        // ie. We're charging, so there's lots of sun
        // So disable the lighting relay.
        flash_one_led(greenled);
        disable_relay(outputPinA);
        enable_relay(outputPinB);
    }
    else if (voltage > 12.9) {
        flash_one_led(greenled);
        // Do nothing; this is to provide a little hysterisis
        // between the two states above and below this one
    }
    else if (voltage > 12.0) {
        enable_one_led(greenled);
        enable_relay(outputPinA);
        enable_relay(outputPinB);
    }
    else if (voltage < 11.5) {
        flash_one_led(redled);
        disable_relay(outputPinA);
        disable_relay(outputPinB);
    }
    else {
        // between 11.5 and 12.0:
        enable_one_led(redled);
    }

    if (voltage <= 11.5) {
        blank_strip();
        lpDelay(25); // quarter seconds
    }
    else {
        spinCycle();
        delay(10);
    }

}

