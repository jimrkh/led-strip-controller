/*
 * LED strip controller
 * 
 * LED strip controller with PIR sensor (passive infrared sensor)
 */

// PIR pim
#define PIR_PIN 7

// LED connected to digital pin 9
#define LED_PIN 9

#define FADE_STEP_VALUE 1
#define LOOP_DELAY 10
#define MIN_STEP_DELAY 60

#define HIGH_FADE_VALUE 255
#define THRESHOLD_FADE_VALUE 20
#define LOW_FADE_VALUE 0

// 2 minutes: 1000 * 60 * 2 = 120000
#define STAY_ON_FULL_PERFORMANCE 120000

// 90 minutes: 1000 * 60 * 90 = 5400000
#define STAY_ON_HALF_PERFORMANCE 5400000


int pirValue;
int fadeValue = LOW_FADE_VALUE;
long performanceDelay = STAY_ON_FULL_PERFORMANCE + STAY_ON_HALF_PERFORMANCE;


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  analogWrite(LED_PIN, fadeValue);
}


void loop() {
  pirValue = digitalRead(PIR_PIN);
  //LOW = no motion, HIGH = motion
  
  if (pirValue == HIGH && fadeValue < HIGH_FADE_VALUE) {
    // Fade IN
    fadeValue = fadeIn(fadeValue);
  }
  
  else if (pirValue == HIGH && fadeValue >= HIGH_FADE_VALUE) {
    // Performance delay reset to FULL
    performanceDelay = STAY_ON_FULL_PERFORMANCE + STAY_ON_HALF_PERFORMANCE;
    delay(LOOP_DELAY);
  }

  else if (pirValue == LOW && fadeValue >= HIGH_FADE_VALUE && performanceDelay > STAY_ON_HALF_PERFORMANCE) {
    // Decrease performance delay 1
    performanceDelay -= LOOP_DELAY;
    delay(LOOP_DELAY);
  }

  else if (pirValue == LOW && fadeValue > THRESHOLD_FADE_VALUE && performanceDelay <= STAY_ON_HALF_PERFORMANCE) {
    // Fade OUT 1
    fadeValue = fadeStep(fadeValue, FADE_STEP_VALUE * (-1));
  }

  else if (pirValue == LOW && fadeValue == THRESHOLD_FADE_VALUE && performanceDelay > 0) {
    // Decrease performance delay 2
    performanceDelay -= LOOP_DELAY;
    delay(LOOP_DELAY);
  }

  else if (pirValue == LOW && fadeValue <= THRESHOLD_FADE_VALUE && fadeValue > 0 && performanceDelay <= 0) {
    // Fade OUT 2
    fadeValue = fadeStep(fadeValue, FADE_STEP_VALUE * (-1));
  }

  else {
    // Waiting
    delay(LOOP_DELAY);
  }
}


int fadeStep(int currentFadeVal, int stepVal) {
  // Sets the value (range from 0 to 255):
  int fadeVal = currentFadeVal + stepVal;

  if (fadeVal < LOW_FADE_VALUE) {fadeVal = LOW_FADE_VALUE;}
  if (fadeVal > HIGH_FADE_VALUE) {fadeVal = HIGH_FADE_VALUE;}

  int diffVal = fadeVal - THRESHOLD_FADE_VALUE;
  if (diffVal < 0 && abs(diffVal) < FADE_STEP_VALUE) {
    fadeVal = THRESHOLD_FADE_VALUE;
  }

  // Write
  analogWrite(LED_PIN, fadeVal);

  // wait for some milliseconds to see the effect
  int delayValue;
  delayValue = (HIGH_FADE_VALUE + 1) - fadeVal;
  if (delayValue > MIN_STEP_DELAY) {delayValue = MIN_STEP_DELAY;}
  delay(delayValue);

  return fadeVal;
}


int fadeIn(int currentFadeVal) {
  // fade in to max value in increments of 1 points:
  analogWrite(LED_PIN, currentFadeVal);
  while (currentFadeVal < HIGH_FADE_VALUE) {
    currentFadeVal = fadeStep(currentFadeVal, FADE_STEP_VALUE);
  }
  return currentFadeVal;
}

