#include "Arduino.h"
// D9 = green
// D10 = blue
// D13 = red
// D11 = piezo buzzer

// D12 - toggled here when there's enough power

int redled = 13;
int greenled = 9;
int blueled = 10;
int sensorPin = A0;

void led_setup(int pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
}

void enable_relay() {
    digitalWrite(12, HIGH);
}

void disable_relay() {
    digitalWrite(12, LOW);
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
    int leds[3] = { redled, greenled, blueled };
    disable_relay();
    for (int i=0; i<3; i++) {
        led_setup(leds[i]);
    }
    Serial.begin(9600);
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
        delay(200);
    }

    Serial.print("Average ADC: ");
    Serial.println(accval/3.0);

    // Using a voltage divider means we get a third of the real
    // value; thus, taking three readings brings us up to the
    // actual. Then we divide by 1024 to get the actual voltage.

    voltage = float(accval)/1024.0;

    Serial.print("Voltage: ");
    Serial.println(voltage); // automatically fixed to 2 decimal places
   
    if (voltage > 13.0) {
        flash_one_led(greenled);
        enable_relay();
    }
    else if (voltage > 12.0) {
        enable_one_led(greenled);
        enable_relay();
    }
    else if (voltage < 11.5) {
        flash_one_led(redled);
        disable_relay();
    }
    else {
        // between 11.5 and 12.0:
        enable_one_led(redled);
    }

    delay(1000);
}

