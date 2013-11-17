#include "Arduino.h"
#include "LiquidCrystal.h"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
    lcd.begin(16,2);
    pinMode(8, OUTPUT);
    digitalWrite(8, LOW);
}

void loop() {
    int alc;
    int perc; 

    lcd.setCursor(0, 0);
    lcd.print("Radiation  level");

    alc = analogRead(5);
    perc = float(alc) / float(102400);

    lcd.setCursor(0, 1);
    lcd.print(perc, DEC);
    lcd.print("% (");
    lcd.print(alc, DEC);
    lcd.print(")");
   
    tone(8, 4000, 5);
    //digitalWrite(8, HIGH);
    //delay(5);
    //digitalWrite(8, LOW);
    delay(1250 - alc);

}

void flash_danger() {
    lcd.setCursor(0, 1);
    lcd.print("-=== DANGER ===-");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("-===        ===-");
    delay(1000);
}
