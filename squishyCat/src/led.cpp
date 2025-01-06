#include <Arduino.h>
#include <FastLED.h>
#include <TaskScheduler.h>
#include "led.h"
#include "buzzer.h"


#define LED_PIN     GPIO_NUM_3        // Pin where your data line is connected
#define NUM_LEDS    4       // Number of LEDs in your strip
#define BRIGHTNESS  170      // Brightness of the LEDs (0 - 255)
#define LED_TYPE    WS2812B  // Type of LED strip
#define COLOR_ORDER GRB      // Color order of the LED strip
#define FAST_UPDATE_INTERVAL 15 // Interval for faster updates temporarily
#define SLOW_UPDATE_INTERVAL 45 // Interval for slow continuous update


CRGB leds[NUM_LEDS];
uint8_t currentHue = 0;
uint8_t targetHue = 0;
bool isUpdatingHue = false;

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
  isUpdatingHue = true;
  updateLEDsTask.setInterval(FAST_UPDATE_INTERVAL);
}

void ledOff()
{
    FastLED.clear();
    FastLED.show();
}

void updateLEDsCallback() {
  if (currentHue == targetHue)
  {
    //increments the hue by one to cycle through colors
    targetHue++;
    currentHue++;
    if (isUpdatingHue)
    {
      updateLEDsTask.setInterval(SLOW_UPDATE_INTERVAL);
      isUpdatingHue = false;
    }
  }else
  {
    currentHue++;
  }
  // Fill the leds with a new color
  for(uint8_t i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CHSV(currentHue, 255, 255);
  }
  

  FastLED.show(); // Send the updated pixel colors to the hardware
}