#include <fix_fft.h>

char im[128];
char data[128];

void setup() {
  Serial.begin(9600);
}

void loop() {
  int static i = 0;
  static long tt;
  int val;
  char datagg;
  static long lastTime = millis();
  long timeTaken;
  
   if (millis() > tt){
    if (i < 64){
      val = analogRead(A5);
      data[i] = val / 4 - 128;
      im[i] = 0;
      i++;  
      
    }
    else{
      //this could be done with the fix_fftr function without the im array.
      fix_fft(data,im,6,0);
      // I am only interested in the absolute value of the transformation
      for (i=0; i< 32;i++){
         data[i] = sqrt(data[i] * data[i] + im[i] * im[i]);
      }

      // do this anyway, for sake of benchmarking      
      for (i=0; i < 32; i+=2) {
        datagg = (data[i] + data[i+1]) / 2;
      }
      
      // Now, how long since we were last here?
      timeTaken = millis() - lastTime;
      Serial.print("Time taken: ");
      Serial.print(timeTaken, DEC);
      Serial.print("\n");

      lastTime = millis();
    }
    
    tt = millis();
   }
} 

