#include "Arduino.h"
// D9 = green
// D10 = blue
// D13 = red
// D11 = piezo buzzer on Leostick

// OutputPin* - toggled here when there's enough power

int redled = 17; // RXLED on micro pro
int greenled = 9;
int blueled = 10;
int sensorPin = A1;
int outputPinA = 5;
int outputPinB = 6;

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

void setup() {
    int leds[] = { redled, greenled, blueled };

    // Enable INPUT+pull-up on all pins; supposedly can save ~10mA that way.
    for (int i=0; i <= 19; i++) {
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
    Serial.begin(9600);
}

// Low Power Delay.  Drops the system clock
// to its lowest setting and sleeps for 256*quarterSeconds milliseconds.
int lpDelay(int quarterSeconds) {
    int oldClkPr = CLKPR;  // save old system clock prescale
    CLKPR = 0x80;    // Tell the AtMega we want to change the system clock
    CLKPR = 0x08;    // 1/256 prescaler = 60KHz for a 16MHz crystal
    delay(quarterSeconds);  // since the clock is slowed way down, delay(n) now acts like delay(n*256)
    CLKPR = 0x80;    // Tell the AtMega we want to change the system clock
    CLKPR = oldClkPr;    // Restore old system clock prescale
}

void loop() {
    int accval = 0;
    int rawval;
    float voltage;

    for (int i=0; i<3; i++) {
        rawval = analogRead(sensorPin);
        Serial.print("Raw ADC: ");
        Serial.println(rawval);
        accval += rawval;
        delay(100);
    }

    Serial.print("Average ADC: ");
    Serial.println(accval/3.0);

    // Using a voltage divider means we get a third of the real
    // value; thus, taking three readings brings us up to the
    // actual. Then we divide by 1024 to get the actual voltage.
    // And of course, multiply by 5.

    voltage = 5.1 * float(accval) / 1024.0;
    // hopefully compiler optimises that to just /204.8

    Serial.print("Voltage: ");
    Serial.println(voltage); // automatically fixed to 2 decimal places
   
    if (voltage > 13.1) {
        // ie. We're charging, so there's lots of sun
        // So disable the lighting relay.
        flash_one_led(greenled);
        disable_relay(outputPinA);
        enable_relay(outputPinB);
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

    // Delay for longer if we're running on very low power:
    if (voltage < 11.5) {
        lpDelay(15); // quarter seconds
    }
    else {
        lpDelay(6); // quarter seconds
    }
}

