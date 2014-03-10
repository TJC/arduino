#include <Audio.h>
#include <Wire.h>
#include <SD.h>

const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioInputI2S       audioInput;         // audio shield: mic or line-in
AudioAnalyzeFFT256  myFFT(20);
AudioOutputI2S      audioOutput;        // audio shield: headphones & line-out

// Create Audio connections between the components
//
AudioConnection c1(audioInput, 0, audioOutput, 0);
AudioConnection c2(audioInput, 0, myFFT, 0);
AudioConnection c3(audioInput, 1, audioOutput, 1);

// Create an object to control the audio shield.
// 
AudioControlSGTL5000 audioShield;


void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Enable the audio shield and set the output volume.
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(60);
}

int nsum[8] = {1, 2, 4, 8, 16, 32, 64};
int oct_out[8];

void loop() {
  int total;
  int n;
  int j;

  if (myFFT.available()) {
    j = 0;
    // each time new FFT data is available
    // print it all to the Arduino Serial Monitor
    for (int i=0; i<8; i++) {
      total = 0;
      n = nsum[i];
      while (n > 0) {
        total += myFFT.output[j++];
        n--;
      }
      oct_out[i] = total;
    }
    serialgraph();
  }
}

void serialgraph() {
    int zz;
    Serial.write(27);   // Print "esc"
    Serial.print("[2J");     // Clear screen

    for (int i=0; i < 8; i++) {
        Serial.print(i, DEC);
        Serial.print(" ");
        zz = oct_out[i] / 3;
        while (zz > 0) {
            Serial.write('=');
            zz--;
        }
        Serial.println();
    }
}

