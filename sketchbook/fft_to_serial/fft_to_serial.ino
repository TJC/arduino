#include <fix_fft.h>

char im[128];
char data[128];

void setup() {
  Serial.begin(57600);
}

void loop() {
  int static i = 0;
  static long tt;
  int val;
  char datagg;
  
   if (millis() > tt){
    if (i < 128){
      val = analogRead(A5);
      data[i] = val / 4 - 128;
      im[i] = 0;
      i++;  
      
    }
    else{
      //this could be done with the fix_fftr function without the im array.
      fix_fft(data,im,7,0);
      // I am only interested in the absolute value of the transformation
      for (i=0; i< 64;i++){
         data[i] = sqrt(data[i] * data[i] + im[i] * im[i]);
      }

      Serial.write(27);   // Print "esc"
      Serial.print("[2J");     // Clear screen
      
      for (i=0; i < 32; i+=2) {
        datagg = (data[i] + data[i+1]) / 2;
        Serial.print(i * 16, DEC); // 7.8 Hz per step
        if (i < 1) { Serial.print("  "); }
        else { Serial.print(" "); }
        while (datagg > 0) {
          Serial.write('=');
          datagg--;
        }
        Serial.println();
      }
      Serial.println(tt, DEC);
    }
    
    tt = millis();
   }
} 

