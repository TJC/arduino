#define ARM_MATH_CM4
#include "arm_math.h"

#include "IntervalTimer.h"

void timer_callback();
void display_output();

const int ledPin = 13;
const short MAX_SAMPLES = 256;

volatile unsigned short analSamps = 0;
volatile unsigned short samples[MAX_SAMPLES];
float32_t complexSamples[MAX_SAMPLES*2];
uint32_t ifftFlag = 0; 
uint32_t doBitReverse = 1; 
arm_cfft_radix4_instance_f32 fft_inst;  /* CFFT Structure instance */

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

  if (arm_cfft_radix4_init_f32(&fft_inst, 256, ifftFlag, doBitReverse) != ARM_MATH_SUCCESS) {
    my_error("math failure (fftlen?");
  }
  
  // 5,000,000 = 5 seconds
  // 250,000 = 1/4 sec?
  // 80 = ~12500 Hz  

  timer1.begin(timer_callback, 80);
}


volatile unsigned short analVal;
void timer_callback() {
  if (analSamps >= MAX_SAMPLES) {
    analSamps++;
    return;
  }
  /*
  analVal = analogRead(A0);
  analVal ^= (1 << 15);
  samples[analSamps++] = analVal;
  */
  samples[analSamps++] = analogRead(A0);
}



inline float my_cabs(float32_t *arr) {
  return(sqrtf(arr[0]*arr[0] + arr[1]*arr[1]));
}

void my_error(const char *str) {
  while(1) {
    Serial.println("math failure (fftlen?)");
    delay(500);
  }
}

void loop() {
  
  digitalWrite(ledPin, HIGH);   // set the LED on

  if (analSamps <= 0) {
    return;
  }

  short int i;
  for (i=0; i<MAX_SAMPLES*2; i+=2) {
    complexSamples[i] = float32_t(samples[i]) / 32768.0;
    complexSamples[i+1] = 0.0;
    // complexSamples[i+1] = 0x8000; // fract15 version of zero
  }
  
  /*
  if (analSamps < 256) {
    for (i=analSamps*2; i<MAX_SAMPLES*2; i++) {
      complexSamples[i] = 0.0;
    }
  }
  */
  
  
  arm_cfft_radix4_f32(&fft_inst, complexSamples);

  display_output();
  
  analSamps = 0;
  
  digitalWrite(ledPin, LOW);    // set the LED off

  delay(20);

}


float accum;
short strides[9] = {2, 4, 8, 12, 16, 24, 32, 48, 64};
short idx;
short stars;
char buf[80];
void display_output() {
  
  Serial.write(27);  // ESC
  Serial.print("[2J"); // clear screen
  
  Serial.print("Actual Samples: ");
  Serial.println(analSamps);  
  
  idx = 0;
  for (short i=0; i<9; i++) {
    accum = 0.0;
    for (short j=0; j<strides[i]; j++) {
      accum += my_cabs(&complexSamples[idx]);
      idx += 2;
    }
    stars = int(ceilf(accum / 10.0));
    for (short k=0; k<stars; k++) {
      buf[k] = '=';
    }
    buf[stars] = 0;
    Serial.println(buf);
  }
  /*
  for (short i=0; i < 30; i+=2) {
    // complexSamples[i] ^= 1 << 15;
    Serial.println(my_cabs(&complexSamples[i]));
  }
  */

}
