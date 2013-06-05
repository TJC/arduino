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

  if (arm_cfft_radix4_init_f32(&fft_inst, MAX_SAMPLES, ifftFlag, doBitReverse) != ARM_MATH_SUCCESS) {
    my_error("math failure (fftlen?");
  }
  
  // 5,000,000 = 5 seconds
  // 250,000 = 1/4 sec?
  // 80 = ~12500 Hz  

  // We only get 256 samples, and I'd like those to be spread out enough to
  // detect 25 Hz signals, which I think means spreading those 256 samples
  // out over a ~12Hz period, at least.
  // So I make the interval between samples to be:
  // 1000000 / 12 / 256 = 325.52

  // If we move up to 1024 samples, then it's be an 80 interval instead.

  timer1.begin(timer_callback, 325);
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
  if (analSamps < MAX_SAMPLES) {
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


// with 256 samples, we'll get 126 outputs, thus:
const short stride_size = 8;
short strides[stride_size] = {2, 4, 8, 12, 16, 24, 30, 30};

// For somewhat verbose output:
// const short stride_size = 21;
// short strides[stride_size] = {1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 8, 8, 8, 8,
                                 12, 12, 12, 12, 18};

// if bottom three registers are driving the RGB LEDs, and top three
// driving LED strands, then it might work nicely to do something like:
// const short stride_size = 6;
// short strides[stride_size] = {2, 3, 3, 16, 32, 40};


float accum;
short idx;
short stars;
char buf[80];
void display_output() {
  
  Serial.write(27);  // ESC
  Serial.print("[2J"); // clear screen
  
  Serial.print("Actual Samples: ");
  Serial.println(analSamps);  
  
  idx = 1; // Skip 0 as that's the DC component of the wave

  for (short i=0; i<stride_size; i++) {
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
