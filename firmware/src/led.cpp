#include <Arduino.h>
#include <FastLED.h>
#include <TaskScheduler.h>
#include "led.h"
#include "buzzer.h"


#define LED_PIN     GPIO_NUM_3        // Pin where your data line is connected
#define NUM_LEDS    4       // Number of LEDs in your strip
#define BRIGHTNESS  180      // Brightness of the LEDs (0 - 255)
#define LED_TYPE    WS2812B  // Type of LED strip
#define COLOR_ORDER GRB      // Color order of the LED strip
#define FAST_UPDATE_INTERVAL 5 // Interval for faster updates temporarily
#define SLOW_UPDATE_INTERVAL 55 // Interval for slow continuous update


CRGB leds[NUM_LEDS];
uint8_t currentHue = 0;
uint8_t targetHue = 0;
bool isMovingToTargetHue = 0;

// Scheduler
Scheduler runner;
// Task for updating LEDs
void updateLEDsCallback();
Task updateLEDsTask(SLOW_UPDATE_INTERVAL, TASK_FOREVER, &updateLEDsCallback);

int32_t ledSetup(){
  // configure FASTLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  // Add task to the scheduler
  runner.init();
  runner.addTask(updateLEDsTask);
  updateLEDsTask.enable();
  return 0;
}

void ledLoop() {
  // Execute scheduler
  runner.execute();
}

void updateLed(uint8_t toUpdateHue){ 
  targetHue = toUpdateHue;
  isMovingToTargetHue = 1;
  fastColorChange(1);
}

void setRandomColor(){
  updateLed(targetHue + random(75, 120));
}

uint8_t getCurrentHue(){
  return currentHue;
}

void fastColorChange(bool startOrStop){
  if(startOrStop) updateLEDsTask.setInterval(FAST_UPDATE_INTERVAL);
  else updateLEDsTask.setInterval(SLOW_UPDATE_INTERVAL);
}

void ledOff()
{
    FastLED.clear();
    FastLED.show();
}

void updateLEDsCallback() {
  if (currentHue == targetHue){
    //increments the hue by one to cycle through colors
    targetHue++;
    currentHue++;
    if (isMovingToTargetHue)
    {
      fastColorChange(0);
      isMovingToTargetHue = 0;
    }
  }else{
    // Calculate the clockwise and counterclockwise distances
    uint8_t clockwiseDistance = targetHue - currentHue;
    uint8_t counterclockwiseDistance = currentHue - targetHue;
    if (clockwiseDistance <= counterclockwiseDistance) //Decide on which direction has the shorter path
        currentHue++; // Move clockwise
    else currentHue--; // Move counterclockwise
  }
  // Fill the leds with a new color
  for(uint8_t i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CHSV(currentHue, 255, 255);
  }
  FastLED.show(); // Send the updated pixel colors to the hardware
}