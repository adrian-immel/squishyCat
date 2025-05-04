#ifndef LED_H_
#define LED_H_

#include <Arduino.h>
#include <FastLED.h>
#include <TaskSchedulerDeclarations.h>
#include "mesh.h"

#define LED_PIN     GPIO_NUM_3        // Pin where your data line is connected
#define NUM_LEDS    4       // Number of LEDs in your strip
#define BRIGHTNESS  195      // Brightness of the LEDs (0 - 255)
#define LED_TYPE    WS2812B  // Type of LED strip
#define COLOR_ORDER GRB      // Color order of the LED strip
#define FAST_UPDATE_INTERVAL 4 // Interval for faster updates temporarily
#define SLOW_UPDATE_INTERVAL 85 // Interval for slow continuous update

int32_t ledSetup(Scheduler &runner);
void ledLoop();
void MeshColorChange(uint8_t toUpdateHue);
void updateLed(uint8_t toUpdateHue);
void ledOff();
void setRandomColor();
void fastColorChange(bool);
uint8_t getCurrentHue();
// Task for updating LEDs
void updateLEDsCallback();

extern bool isMovingToTargetHue;


#endif /* LED_H_ */