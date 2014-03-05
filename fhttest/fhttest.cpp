/*
fht_adc.pde
guest openmusiclabs.com 9.5.12
example sketch for testing the fht library.
it takes in data on ADC0 (Analog0) and processes them
with the fht. the data is sent out over the serial
port at 115.2kb.  there is a pure data patch for
visualizing the data.
*/

#include <Arduino.h>
#include <avr/pgmspace.h>

#define OCT_NORM 1
#define OCTAVE 1
#define FHT_N 256 // set to 256 point fht

#include <FHT.h> // include the library

void show_fft_q();

void setup() {
  Serial.begin(115200); // use the serial port

  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0

}

void loop2() {

    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      fht_input[i] = analogRead(A0) - 512;
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_octave(); // take the output of the fht

    show_fft_q();
    delay(25);

}

void loop() {

    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_octave(); // take the output of the fht
    sei();
    show_fft_q();
    delay(25);

}

void show_fft_q() {
     int zz;
     
     Serial.write(27);   // Print "esc"
     Serial.print("[2J");     // Clear screen
      
      for (int i=0; i < 8; i++) {
        Serial.print(i, DEC);
        Serial.print(" ");
	zz = fht_oct_out[i] / 2;
        while (zz > 0) {
          Serial.write('=');
          zz--;
        }
        Serial.println();
      }
}

