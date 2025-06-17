#include <Arduino.h>
#include <Wire.h>

#include "led.h"
#include "power.h"
#include "buzzer.h"
#include "mesh.h"
#include "tap.h"
#include "squish.h"
#include <TaskScheduler.h>


Scheduler runner;


void setup() {
  Serial.begin(115200);
  Wire.begin(48, 47);
  // setup code
  Serial.println("Buzzer init");
  buzzer_setup();
  Serial.println("Power init");
  power_setup();
  randomSeed(analogRead(0));
  // init task Scheduler
  runner.init();
  Serial.println("Led init");
  ledSetup(runner);
  Serial.println("Tap init");
  tapSetup();
  Serial.println("Squish init");
  squishSetup();
  Serial.println("Mesh init");
  initMeshNetwork(runner);
  Serial.println("Startup complete");

}

void loop() {
  
  powerOffButtonPressedLoop();
  tapLoop();
  squishLoop();
  runner.execute();
  }
