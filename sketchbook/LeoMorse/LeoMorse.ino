
/*
LeoMorse.ino
Morse Code sender

Luke Weston (@lukeweston), 2012
License: GPLv3.
Version: 0.0

Documentation: hah, yeah, right.

Derived from the excellent Arduino Morse library by Erik Linder.
Specifically intended for the Freetronics LeoStick - does not use PWM timer at all.
Could be used with any Arduino, though.

This file uses the ".ino" extension used with Arduino IDE v1.0, but you could rename it to *.pde if you have the older IDE.

Usage: Once the program is uploaded and running, open a serial terminal to the LeoStick at 38400 baud and type in text...

*/

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif
  
	byte morseTable[] = {
	// Essentially, this works by storing the table of Morse code characters as binary numbers.
	// Those entries equal to 1 have no Morse code.
	107, // ASCII 33 !   
	82,  // ASCII 34 "   
	1,   // ASCII 35 #   
	137, // ASCII 36 $   
	1,   // ASCII 37 %   
	40,  // ASCII 38 &   
	94,  // ASCII 39 '  
	109, // ASCII 40 (
	109, // ASCII 41 )   
	1,   // ASCII 42 *   
	42,  // ASCII 43 +   
	115, // ASCII 44 ,   
	97,  // ASCII 45 -   
	85,  // ASCII 46 .   
	50,  // ASCII 47 /  
	63,  // ASCII 48 0
	62,  // ASCII 49 1
	60,  // ASCII 50 2
	56,  // ASCII 51 3
	48,  // ASCII 52 4
	32,  // ASCII 53 5
	33,  // ASCII 54 6
	35,  // ASCII 55 7
	39,  // ASCII 56 8
	47,  // ASCII 57 9
	120, // ASCII 58 :
	53,  // ASCII 59 ;
	1,   // ASCII 60 <
	49,  // ASCII 61 =
	1,   // ASCII 62 >
	76,  // ASCII 63 ?
	69,  // ASCII 64 @
	6,   // ASCII 65 A
	17,  // ASCII 66 B
	21,  // ASCII 67 C
	9,   // ASCII 68 D
	2,   // ASCII 69 E
	20,  // ASCII 70 F
	11,  // ASCII 71 G
	16,  // ASCII 72 H
	4,   // ASCII 73 I
	30,  // ASCII 74 J
	13,  // ASCII 75 K
	18,  // ASCII 76 L
	7,   // ASCII 77 M
	5,   // ASCII 78 N
	15,  // ASCII 79 O
	22,  // ASCII 80 P
	27,  // ASCII 81 Q
	10,  // ASCII 82 R
	8,   // ASCII 83 S
	3,   // ASCII 84 T
	12,  // ASCII 85 U
	24,  // ASCII 86 V
	14,  // ASCII 87 W
	25,  // ASCII 88 X
	29,  // ASCII 89 Y
	19,  // ASCII 90 Z
	1,   // ASCII 91 [    
	1,   // ASCII 92 \    
	1,   // ASCII 93 ]    
	1,   // ASCII 94 ^    
	77,  // ASCII 95 _    
	94,  // ASCII 96 `    
	6,   // ASCII 97 a
	17,  // ASCII 98 b
	21,  // ASCII 99 c
	9,   // ASCII 100 d
	2,   // ASCII 101 e
	20,  // ASCII 102 f
	11,  // ASCII 103 g
	16,  // ASCII 104 h
	4,   // ASCII 105 i
	30,  // ASCII 106 j
	13,  // ASCII 107 k
	18,  // ASCII 108 l
	7,   // ASCII 109 m
	5,   // ASCII 110 n
	15,  // ASCII 111 o
	22,  // ASCII 112 p
	27,  // ASCII 113 q
	10,  // ASCII 114 r
	8,   // ASCII 115 s
	3,   // ASCII 116 t
	12,  // ASCII 117 u
	24,  // ASCII 118 v
	14,  // ASCII 119 w
	25,  // ASCII 120 x
	29,  // ASCII 121 y
	19   // ASCII 122 z
};
 
	int frequency = 4000; // 4 kHz is about right for the resonance frequency for the LeoStick piezo buzzer.
	byte pin = 11;  // Hardware speaker pin, 11 on LeoStick
	byte speed = 30;  // Speed - can go faster or slower if you're less experienced or more experienced with Morse.
  
	int dotlength = (1200 / speed);
	int dashlength =  (3 * dotlength);
  
void setup()
{
	Serial.begin(38400);
	Serial.println("Enter some text; terminate with a carriage return to send the string.");
	pinMode(pin, OUTPUT);
        pinMode(13, OUTPUT);
        pinMode(9, OUTPUT);
        pinMode(10, OUTPUT);
        digitalWrite(9, HIGH);
        digitalWrite(13, HIGH);
        digitalWrite(10, HIGH);
}

void loop()
{
	char incomingByte = 0;

	if (Serial.available())
	{
		incomingByte = Serial.read();
		if (incomingByte >= 97)  // Convert all input characters to upper case.
		incomingByte -= 32;
		sendMsg(&incomingByte);
	}
}

void sendMsg(char *str)
{
	while (*str)
		send(*str++);
}

void send(char c)
{
	byte i;
	byte p;

	if (c == ' ')
	{
		delay( 7 * dotlength);
		return;
	}
  
	else
	{
		i = ((byte) c) - 33;
		p = morseTable[i];
	}

	while (p != 1)
	{
		if (!(p & 1))
			dit();
		else
			dah();
		p /= 2;
	}
	delay(2 * dotlength);
}

void dit()
{
	int i = 0;
	for(i = 0; i < ((frequency / 1000) * dotlength); i++)
	{
		digitalWrite(pin, HIGH);
		delayMicroseconds(500000 / frequency);
		digitalWrite(pin, LOW);
		delayMicroseconds(500000 / frequency);
	}
	
	digitalWrite(pin, LOW);
	for(i = 0; i < dotlength; i++)
		delay(1);
}

void dah()
{
	int i = 0;
	for(i = 0; i < ((frequency / 1000) * dashlength); i++)
	{
		digitalWrite(pin, HIGH);
		delayMicroseconds(500000 / frequency);
		digitalWrite(pin, LOW);
		delayMicroseconds(500000 / frequency);
	}
	
	digitalWrite(pin, LOW);	
	for(i = 0; i < dotlength; i++)
	delay(1);
}

