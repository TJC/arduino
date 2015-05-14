/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(4, 5, 8,9,10,11);

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

void customChar(int col, int vals[])
{
  lcd.setCursor(col, 0);
  lcd.write(vals[0]);
  lcd.write(vals[1]);
  lcd.write(vals[2]);
  lcd.setCursor(col, 1);
  lcd.write(vals[3]);
  lcd.write(vals[4]);
  lcd.write(vals[5]);
}

void custom0(int col)
{
  int c[] = {2,8,1, 2,6,1};
  customChar(col, c);
}

void custom1(int col)
{
  int c[] = {32,32,1, 32,32,1};
  customChar(col, c);
}

// Narrow version
void narrow1(int col)
{
  lcd.setCursor(col,0);
  lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(1);
}

void custom2(int col)
{
  lcd.setCursor(col,0);
  lcd.write(5);
  lcd.write(3);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(2);
  lcd.write(6);
  lcd.write(6);
}


void custom3(int col)
{
  int c[] = {5,3,1, 7,6,1};
  customChar(col, c);
}


void custom4(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(6);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}

void custom5(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(1);
}

void custom6(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(2);
  lcd.write(6);
  lcd.write(1);
}

void custom7(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(8);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}


void custom8(int col)
{
  int c[] = {2,3,1, 2,6,1};
  customChar(col, c);
}


void custom9(int col)
{
  lcd.setCursor(col, 0); 
  lcd.write(2);  
  lcd.write(3); 
  lcd.write(1);
  lcd.setCursor(col, 1); 
  lcd.write(7);  
  lcd.write(6);  
  lcd.write(1);
}

void customA(int col)
{
  int c[] = {2,3,1, 2,32,1};
  customChar(col, c);
}


void customE(int col)
{
  int c[] = {2,3,4, 2,6,6};
  customChar(col, c);
}


void customJ(int col)
{
  int c[] = {32,32,1, 2,6,1};
  customChar(col, c);
}

void customU(int col)
{
  int c[] = {2,32,1, 2,6,1};
  customChar(col, c);
}

void customL(int col)
{
  int c[] = {2,32,32, 2,6,32};
  customChar(col, c);
}

void customP(int col)
{
  int c[] = {2,3,1, 2,32,32};
  customChar(col, c);
}

void customR(int col)
{
  int c[] = {2,3,1, 2,96,164};
  customChar(col, c);
  //lcd.setCursor(col+1, 1);
  //lcd.write("\\");
}

void customS(int col)
{
  custom5(col);
}


void draw1985()
{
  lcd.clear();
  custom1(0);
  custom9(4);
  custom8(8);
  custom5(12);
}

void draw4JULY()
{
  lcd.clear();
  custom4(0);
  customJ(5);
  customU(8);
  customL(11);
  custom4(13);
}

void draw15APR()
{
  lcd.clear();
  narrow1(0);
  custom5(1);
  customA(5);
  customP(9);
  customR(13);
}

void draw11SEP()
{
  lcd.clear();
  narrow1(0);
  narrow1(2);
  customS(5);
  customE(9);
  customP(13);
}

void drawTime()
{
  lcd.clear();
  custom2(0);
  custom3(3);
  lcd.setCursor(7,0);
  lcd.write(165);
  lcd.setCursor(7,1);
  lcd.write(165);
  custom4(9);
  custom5(12);
}

void setup()
{
  delay(100);
  // assignes each segment a write number
  lcd.createChar(1,bar1);
  lcd.createChar(2,bar2);
  lcd.createChar(3,bar3);
  lcd.createChar(4,bar4);
  lcd.createChar(5,bar5);
  lcd.createChar(6,bar6);
  lcd.createChar(7,bar7);
  lcd.createChar(8,bar8);
  
  // sets the LCD's rows and colums:
  lcd.begin(16, 2);

}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  // lcd.setCursor(0, 0);
  drawTime();
  delay(1500);
  draw15APR();
  delay(1500);
  draw1985();
  delay(1500);

  //draw11SEP();
  //delay(1500);
  //draw4JULY();
  // lcd.scrollDisplayLeft();
  //delay(1500);
}

