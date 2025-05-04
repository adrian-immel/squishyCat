#include "led.h"

#include "mesh.h"


CRGB leds[NUM_LEDS];
uint8_t currentHue = random(0, 255);
uint8_t targetHue = currentHue;
bool isMovingToTargetHue = false;
// Scheduler
Task updateLEDsTask(SLOW_UPDATE_INTERVAL, TASK_FOREVER, &updateLEDsCallback);


int32_t ledSetup(Scheduler &runner){
  // configure FASTLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  // Add task to the scheduler
  runner.addTask(updateLEDsTask);
  updateLEDsTask.enable();
  return 0;
}



void updateLed(const uint8_t toUpdateHue, const bool sendMeshMsg){
  targetHue = toUpdateHue;
  isMovingToTargetHue = 1;
  fastColorChange(1);
  if (sendMeshMsg) sendColorSetMessage(targetHue); //bool to stop a mesh message loop
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
      fastColorChange(false);
      isMovingToTargetHue = false;
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