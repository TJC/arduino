/*
 Display back-to-the-future style time and dates on a bunch of
 HD44780 16x02 LCD displays. Uses a large font style.
 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>


// global LCD array
LiquidCrystal displays[3] = {
 { 4, 7, 8,9,10,11 },
 { 4, 5, 8,9,10,11 },
 { 4, 6, 8,9,10,11 }
};

/*
// Requires platform.txt modified to include compile.cpp.flags += -std=gnu++11
#include <array>
std::array <LiquidCrystal, 3> displays = {{
 { 4, 5, 8,9,10,11 },
 { 4, 6, 8,9,10,11 },
 { 4, 7, 8,9,10,11 }
}};
*/

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

/*
    Useful built-in symbols:
     47 /
     84 T
    124 |
    164 central dot
    255 solid block
     96 on 164 is kinda \
*/

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
  {2,8,1, 2,47,32},     // D
  {2,3,4, 2,6,6},       // E
  {2,3,4, 2,32,32},     // F
  {2,3,1, 7,6,1},       // G (copy of 9)
  {2,6,1, 2,32,1},      // H
  {32,1,32, 32,1,32},   // I (2 col narrow)
  {32,32,1, 2,6,1},     // J
  {}, // K
  {2,32,32, 2,6,32},    // L (2 col narrow)
  {2,118,1, 2,32,1},    // M
  {2,164,1, 2,96,1},    // N
  {2,8,1, 2,6,1},       // O (same as 0)
  {2,3,1, 2,32,32},     // P
  {}, // Q
  {2,3,1, 2,96,164},    // R
  {2,3,4, 7,6,1},       // S (same as 5)
  {32,84,8, 32,124,32}, // T
  {2,32,1, 2,6,1},      // U
  {2,32,1, 2,47,32},   // V
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
  delay(100); // seems to avoid initialisation issues

  // initialize the library with the numbers of the interface pins
  /*
  displays[0] = new LiquidCrystal(4, 5, 8,9,10,11);
  displays[1] = new LiquidCrystal(4, 6, 8,9,10,11);
  displays[2] = new LiquidCrystal(4, 7, 8,9,10,11);
  */

  for (int i=0; i<3; i++) {
    displays[i].begin(16, 2);
    setupChars(displays[i]);
  }

}

void displayWipe(LiquidCrystal lcd) {
  lcd.scrollDisplayLeft();
  for (int i=0; i<16; i++) {
    delay(60);
    lcd.scrollDisplayLeft();
  }
}

void clearAllDisplays() {
  for (int i=0; i<3; i++) {
    displays[i].clear();
  }
}

void wipeAllDisplays() {
  for (int i=0; i<16; i++) {
    delay(60);
    for (int i=0; i<3; i++) {
      displays[i].scrollDisplayLeft();
    }
  }
}

void dissolve() {
  int dseq[32] = { // actually dseq[2][16]
    2, 11, 6, 15,
    4, 9, 13, 0,
    10, 5, 14, 8,
    1, 12, 3, 7,
    13, 4, 9, 0,
    11, 6, 2, 15,
    5, 10, 1, 7,
    14, 3, 12, 8
  };
  for (int i=0; i<16; i++) {
    for (int d=0; d<3; d++) {
      for (int j=0; j<2; j++) {
        displays[d].setCursor(dseq[j*16 + i], j);
        displays[d].write(32); // ie. blank space
        delay(10);
      }
    }
  }
}

char messages[][3][12] = {
  { "00:01", "DEC 25", " 0 0 0 0" },   // birth of christ
  { "00:30", "NOV 05", " 1 6 0 5" },   // guy fawkes discovered
  { "03:22", "JUL  07", " 1 9 4 7" },  // roswell incident
  { "12:29", "NOV 22", " 1 9 6 3" },   // JFK
  { "15:37", "DEC 06", " 1 9 8 2" },   // Akira incident / WW3
  { "05:01", "MAY 12", " 1 9 8 4" },   // terminator
  { "01:22", "OCT 26", " 1 9 8 5" },   // back to the future
  { "02:14", "AUG 29", " 1 9 9 7" },   // judgement day
  { "08:46", "SEP  11", " 2 0 0 1" },  // sep 11 terrorism attack
  { "10:04", "OCT  21", " 2 0 1 5" },  // back to the future 2
};

void do_message(int mId) {
  clearAllDisplays();

  for (int i=0; i<3; i++) {
    displayString(displays[i], 0, messages[mId][i]);
  }

  delay(3000);

  //wipeAllDisplays();
  dissolve();
  delay(500);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  // lcd.setCursor(0, 0);

  for (int m=0; m<10; m++) {
    do_message(m);
  }
  
}

