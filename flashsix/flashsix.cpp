#include "Arduino.h"
// D9 = green
// D10 = blue
// D13 = red
// D11 = piezo buzzer

/* Aim of the game - flash LEDs 8,9,10,11,12,13 */

static int reg_delay = 250;

void setup() {
    Serial.begin(9600);
    for (int i=8; i<14; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
}

void loop() {
    static int i;
    for (i=8; i<14; i++) {
        digitalWrite(i, HIGH);
        //Serial.print("pin: ");
        //Serial.println(i);
        delay(reg_delay);
        digitalWrite(i, LOW);
    }
    for (i=12; i>8; i--) {
        digitalWrite(i, HIGH);
        //Serial.print("pin: ");
        //Serial.println(i);
        delay(reg_delay);
        digitalWrite(i, LOW);
    }
}

