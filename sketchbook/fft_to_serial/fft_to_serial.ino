#include <fix_fft.h>

#define MIC_INPUT_PIN A2
#define NUM_SAMPLES 128
#define HALF_NUM_SAMPLES 64
#define NUM_SAMPLES_PWR_TWO 7

char im[NUM_SAMPLES];
char data[NUM_SAMPLES];

void setup() {
  Serial.begin(38400);
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(13, LOW);
  digitalWrite(9, LOW);
}

void loop() {
  int static i = 0;
  static long tt;
  int val;
  
   if (millis() > tt){
    if (i < NUM_SAMPLES){
      val = analogRead(MIC_INPUT_PIN);
      // that returns 0-1023, so convert to a signed byte:
      data[i] = val / 4 - 128;
      // data[i] = val - 512; // see if this works better for low volumes?
      im[i] = 0;
      i++;  
      
    }
    else{
      //this could be done with the fix_fftr function without the im array.
      fix_fft(data, im, NUM_SAMPLES_PWR_TWO, 0);
      // I am only interested in the absolute value of the transformation
      for (i=0; i < HALF_NUM_SAMPLES;i++){
         data[i] = sqrt(data[i] * data[i] + im[i] * im[i]);
      }

      show_fft_q();

      digitalWrite(13, check_average_rest() ? HIGH : LOW);
      digitalWrite(9, check_average_bass() ? HIGH : LOW);
      
      i = 0; // reset for next sample-taking..
    }
    
    tt = millis();
   }
}

// Given the current histogram, keep a rolling average of max value of all
// bands up to 100 Hz.
// Return true if current sample exceeds the average*1.3
int rolling_avg_bass = 0;
int check_average_bass() {
  int bass_max = 0;

  for (int i=0; i < 12; i++) {
     if (data[i] > bass_max) {
        bass_max = data[i];
     }
  }
  
  // need to increase it so the integer math doesn't round to zero
  bass_max *= 1000;
  
  Serial.print("bass_max: ");
  Serial.println(bass_max);
  rolling_avg_bass = (rolling_avg_bass * 0.95) + (bass_max * 0.05);
  Serial.println(rolling_avg_bass);
  if (bass_max > rolling_avg_bass * 1.3) {
    return(1);
  }
  return(0);
}

int rolling_avg_rest = 0;
int check_average_rest() {
  int val_max = 0;

  for (int i=12; i < HALF_NUM_SAMPLES; i++) {
     if (data[i] > val_max) {
        val_max = data[i];
     }
  }
  
  // need to increase it so the integer math doesn't round to zero
  val_max *= 1000;
  
  Serial.print("rest_max: ");
  Serial.println(val_max);
  rolling_avg_rest = (rolling_avg_rest * 0.95) + (val_max * 0.05);
  Serial.println(rolling_avg_rest);
  if (val_max > rolling_avg_rest * 1.3) {
    return(1);
  }
  return(0);
}

void show_fft_q() {
     int datagg;
     
     Serial.write(27);   // Print "esc"
     Serial.print("[2J");     // Clear screen
      
      for (int i=0; i < 61; i+=3) {
        datagg = (data[i] + data[i+1] + data[i+2]) / 3;
        Serial.print(int((i+1) * 7.8), DEC); // 7.8 Hz per step
        Serial.print(" ");
        while (datagg > 0) {
          Serial.write('=');
          datagg--;
        }
        Serial.println();
      }
      Serial.println(millis(), DEC);
}

void show_fft_full() {
     
     Serial.write(27);   // Print "esc"
     Serial.print("[2J");     // Clear screen
      
      for (int i=0; i < HALF_NUM_SAMPLES; i++) {
        Serial.print(i * 8, DEC); // 7.8 Hz per step
        if (i < 2) { Serial.print("  "); }
        else { Serial.print(" "); }
        while (data[i] > 0) {
          Serial.write('=');
          data[i]--;
        }
        Serial.println();
      }
      Serial.println(millis(), DEC);
}
