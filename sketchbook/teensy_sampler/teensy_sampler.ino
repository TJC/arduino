#include "IntervalTimer.h"

void timer_callback();

const int ledPin = 13;


volatile unsigned short analSamps = 0;
volatile short samples[2000];

IntervalTimer timer1;
  
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  Serial.begin(38400);
  analogReadRes(16); // Increase ADC resolution

  digitalWrite(ledPin, HIGH);   // set the LED on
  delay(2000);
  digitalWrite(ledPin, LOW);   // set the LED on
  Serial.println("Starting...");
  
  // 5,000,000 = 5 seconds
  // 250,000 = 1/4 sec?
  // 80 = ~12500 Hz  

  timer1.begin(timer_callback, 80);
}

void timer_callback() {
  samples[analSamps++] = analogRead(A0) - 32767;
}

// the loop() methor runs over and over again,
// as long as the board has power

void loop() {
  float avg = 0;
  long avgSamp = 0;
  short maxSamp = -32767;
  short minSamp = 32767;
  
  digitalWrite(ledPin, HIGH);   // set the LED on

  for (short i=0; i < analSamps; i++) {
    if (samples[i] > maxSamp) { maxSamp = samples[i]; }
    if (samples[i] < minSamp) { minSamp = samples[i]; }
    avgSamp += samples[i];
  }
  
  if (analSamps > 0) {
    avg = float(avgSamp)/analSamps;
  }
  
  Serial.print("samples: ");
  Serial.print(analSamps);
  Serial.print("\t avg val: ");
  Serial.print(avg);
  Serial.print("\tmin: ");
  Serial.print(minSamp);
  Serial.print("\tmax: ");
  Serial.println(maxSamp);
  
  analSamps = 0;
  
  digitalWrite(ledPin, LOW);    // set the LED off

  delay(50);

}

