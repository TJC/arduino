/*
 Display back-to-the-future style time and dates on a bunch of
 HD44780 16x02 LCD displays. Uses a large font style.
 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd1(4, 5, 8,9,10,11);
LiquidCrystal lcd2(4, 6, 8,9,10,11);
LiquidCrystal lcd3(4, 7, 8,9,10,11);

// the 8 arrays that form each segment of the custom numbers
byte bar1[8] = 
{
        B11000,
        B11100,
        B11100,
        B11100,
        B11100,
        B11100,
        B11100,
        B11000
};
byte bar2[8] =
{
        B00011,
        B00111,
        B00111,
        B00111,
        B00111,
        B00111,
        B00111,
        B00011
};
byte bar3[8] =
{
        B11111,
        B11111,
        B00000,
        B00000,
        B00000,
        B00000,
        B11111,
        B11111
};
byte bar4[8] =
{
        B11110,
        B11100,
        B00000,
        B00000,
        B00000,
        B00000,
        B11000,
        B11100
};
byte bar5[8] =
{
        B01111,
        B00111,
        B00000,
        B00000,
        B00000,
        B00000,
        B00011,
        B00111
};
byte bar6[8] =
{
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B11111,
        B11111
};
byte bar7[8] =
{
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00111,
        B01111
};
byte bar8[8] =
{
        B11111,
        B11111,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000
};

int bigFont[64][6] = {
  {2,8,1, 2,6,1},       // 0
  {32,1,32, 32,1,32},   // 1 (2 col narrow)
  {5,3,1, 2,6,6},       // 2
  {5,3,1, 7,6,1},       // 3
  {2,6,1, 32,32,1},     // 4
  {2,3,4, 7,6,1},       // 5
  {2,3,4, 2,6,1},       // 6
  {2,8,1, 32,32,1},     // 7
  {2,3,1, 2,6,1},       // 8
  {2,3,1, 7,6,1},       // 9
  {32,165,32, 32,165,32}, // :
  {}, // ;
  {}, // <
  {32,6,32, 32,6,32},   // =
  {}, // >
  {}, // ?
  {}, // @
  {2,3,1, 2,32,1},      // A
  {}, // B
  {2,8,8, 2,6,6},       // C
  {}, // D
  {2,3,4, 2,6,6},       // E
  {2,3,4, 2,32,32},     // F
  {}, // G
  {2,6,1, 2,32,1},      // H
  {32,1,32, 32,1,32},   // I (2 col narrow)
  {32,32,1, 2,6,1},     // J
  {}, // K
  {2,32,32, 2,6,32},    // L (2 col narrow)
  {}, // M
  {}, // N
  {2,8,1, 2,6,1},       // O (same as 0)
  {2,3,1, 2,32,32},     // P
  {}, // Q
  {2,3,1, 2,96,164},    // R
  {2,3,4, 7,6,1},       // S (same as 5)
  {}, // T
  {2,32,1, 2,6,1},      // U
  {}, // V
  {}, // W
  {}, // X
  {2,6,1, 32,32,1},     // Y (same as 4)
  {}, // Z
};


void customChar(LiquidCrystal lcd, int col, int vals[])
{
  if (vals == NULL) { return; }
  lcd.setCursor(col, 0);
  lcd.write(vals[0]);
  lcd.write(vals[1]);
  lcd.write(vals[2]);
  lcd.setCursor(col, 1);
  lcd.write(vals[3]);
  lcd.write(vals[4]);
  lcd.write(vals[5]);
}

/*
  displayString -- writes a string of numbers and capital letters to the LCD
  Params:
    col -- Initial column to start at. Probably zero for most use.
  Note:
    Actual space characters are narrower than the rest.
  Example:
    displayString(0, 1, "01:22");
    displayString(0, 0, "1 9 8 5");
*/
void displayString(LiquidCrystal lcd, int col, char *s) {
    int charval;
    while (*s != '\0') {
        charval = *s;
        if (charval == 32) {
            col += 1;
        }
        else if (charval == 'L' || charval == '1' || charval == 'I') {
            customChar(lcd, col, bigFont[charval-48]);
            col += 2;
        }
        else if (charval >= 48 && charval <= 90) {
            // ord('0') = 48 and our font array starts at 0
            customChar(lcd, col, bigFont[charval-48]);
            col += 3;
        }
        s++;
    }
}

// Narrow version
void narrow1(LiquidCrystal lcd, int col)
{
  lcd.setCursor(col,0);
  lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(1);
}
 
void setupChars(LiquidCrystal lcd)
{
  lcd.createChar(1,bar1);
  lcd.createChar(2,bar2);
  lcd.createChar(3,bar3);
  lcd.createChar(4,bar4);
  lcd.createChar(5,bar5);
  lcd.createChar(6,bar6);
  lcd.createChar(7,bar7);
  lcd.createChar(8,bar8);
}

void setup()
{
  delay(100);

    // sets the LCD's rows and colums:
  lcd1.begin(16, 2);
  lcd2.begin(16, 2);
  lcd3.begin(16, 2);

  setupChars(lcd1);
  setupChars(lcd2);
  setupChars(lcd3);

}

void displayWipe(LiquidCrystal lcd) {
  lcd.scrollDisplayLeft();
  for (int i=0; i<16; i++) {
    delay(60);
    lcd.scrollDisplayLeft();
  }
}

char messages[][12] = {
  "01:22",
  "15  APR",
  " 1 9 8 5",
  "11  SEP",
  "4 JULY",
};

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  // lcd.setCursor(0, 0);
/*
  for (int i=0; i<5; i++) {
    lcd.clear();
    displayString(0, messages[i]);
    delay(1500);
    displayWipe();
    lcd.clear();
    delay(500);
  }
*/
  lcd1.clear();
  lcd2.clear();
  lcd3.clear();
  displayString(lcd1, 0, messages[0]);
  displayString(lcd2, 0, messages[1]);
  displayString(lcd3, 0, messages[2]);
  delay(1000);
  displayWipe(lcd1);
  displayWipe(lcd2);
  displayWipe(lcd3);
  
}

