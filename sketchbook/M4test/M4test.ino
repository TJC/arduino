#define ARM_MATH_CM4
#include "arm_math.h"

#define TEST_LENGTH_SAMPLES 1024 
#include "arm_fft_bin_data.h"

// NOTE: q15t is int16_t in arm_math.h
 
/* ------------------------------------------------------------------ 
* Global variables for FFT Bin Example 
* ------------------------------------------------------------------- */ 
uint32_t ifftFlag = 0; 
uint32_t doBitReverse = 1; 
 
/* Reference index at which max energy of bin ocuurs */ 
uint32_t refIndex = 213, testIndex = 0; 
 

void setup() {
  Serial.begin(19200);
  pinMode(13, OUTPUT);
  for (int i=0; i < 5; i++) {
    Serial.println(" start program ");
    delay(1000);
  }
  
}

float my_cabs(float *arr) {
  return(sqrtf(arr[0]*arr[0] + arr[1]*arr[1]));
}

void loop() {

    arm_cfft_radix4_instance_f32 fft_inst;  /* CFFT Structure instance */
    arm_cfft_radix4_init_f32(&fft_inst, 256, ifftFlag, doBitReverse);
    
    Serial.println("step 1"); 
    /* Process the data through the CFFT/CIFFT module */ 
    arm_cfft_radix4_f32(&fft_inst, testInput_f32_10khz);
    Serial.println("step 2");  

    for (int i=0; i < 40; i+=2) {
      Serial.println(my_cabs(&testInput_f32_10khz[i]));
    }     

    while (1) {
        digitalWrite(13, HIGH);
        Serial.println("done");
        delay(5000);
        digitalWrite(13, LOW);
        delay(5000);
    }      

    
}
