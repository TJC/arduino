#include "Arduino.h"
#include "LiquidCrystal.h"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

typedef unsigned short ushort;
typedef unsigned int uint;

unsigned long cur_millis = 0;

/* Returns batshit insane values. WHY?!

const ushort segment_lookup[15] = {
    16, 32, 48, 96,
    128, 160, 192, 256,
    384, 448, 512, 720,
    800, 900, 2000
};

// min returned = 1
// max returned = 15
ushort num_segments(ushort val) {
    ushort i = 0;
    while (val < segment_lookup[i]) { i++; }
    return i+1;  // ie. max will be 15
}
*/

ushort num_segments(ushort val) {
    // ushort i;
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

void setup() {
    Serial.begin(38400);
    lcd.begin(16,2);
    pinMode(8, OUTPUT);
    digitalWrite(8, LOW);
}

void display_bar_graph(ushort val) {
    ushort segments; 
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

    for (ushort i=0; i<=segments; i++) {
        buf[i] = '#';
    }
    buf[segments+1] = '>';
    for (ushort i=segments+2; i<16; i++) {
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

void clicker(ushort val) {
    static unsigned long last_click = 0;
    if (cur_millis < last_click + 1050-val) {
        return;
    }

    tone(8, 4000, 5);

    unsigned int cdelay = rand() % (1250-val);
    last_click = cur_millis + cdelay; // adds some random timing
}

void loop() {
    ushort alc;

    cur_millis = millis();

    alc = analogRead(5);
    display_bar_graph(alc);
    clicker(alc);
}

