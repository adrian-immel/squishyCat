#include "led.h"

CRGB leds[NUM_LEDS];
uint8_t currentHue = random(0, 255);
uint8_t targetHue = currentHue;
// Scheduler
Task updateLEDsTask(SLOW_UPDATE_INTERVAL, TASK_FOREVER, &updateLEDsCallback);
bool isMovingToTargetHue = false;
bool sendMeshMsg = false;

int32_t ledSetup(Scheduler &runner){
  // configure FASTLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  // Add task to the scheduler
  runner.addTask(updateLEDsTask);
  updateLEDsTask.enable();
  return 0;
}



void updateLed(const uint8_t toUpdateHue){
  targetHue = toUpdateHue;
  sendColorSetMessage(targetHue);
  isMovingToTargetHue = true;
  fastColorChange(true);
}


void setRandomColor(){
  updateLed(targetHue + random(75, 120));
}

uint8_t getCurrentHue(){
  return currentHue;
}

uint8_t getTargetHue(){
  return targetHue;
}

void fastColorChange(const bool startOrStop){
  if(startOrStop) updateLEDsTask.setInterval(FAST_UPDATE_INTERVAL);
  else updateLEDsTask.setInterval(SLOW_UPDATE_INTERVAL);
}
void MeshColorChange(const uint8_t toUpdateHue)
{
  //if the difference is less than 10, don't change the color
  if (toUpdateHue - targetHue < 10) return;
  targetHue = toUpdateHue;
  isMovingToTargetHue = true;
  sendMeshMsg = false;
  fastColorChange(true);
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
      if (sendMeshMsg) //sendColorSetMessage(targetHue);
      sendMeshMsg = true;
    }
  }else{
    // Calculate the clockwise and counterclockwise distances
    const uint8_t clockwiseDistance = targetHue - currentHue;
    const uint8_t counterclockwiseDistance = currentHue - targetHue;
    if (clockwiseDistance <= counterclockwiseDistance) //Decide on which direction has the shorter path
        currentHue++; // Move clockwise
    else currentHue--; // Move counterclockwise
  }
  // Fill the leds with a new color
  for(auto & led : leds) {
    led = CHSV(currentHue, 255, 255);
  }
  FastLED.show(); // Send the updated pixel colors to the hardware
}