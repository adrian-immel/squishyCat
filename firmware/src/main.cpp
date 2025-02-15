#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>
//#include "painlessMesh.h"

#include "led.h"
#include "power.h"
#include "buzzer.h"
#include "tap.h"


void setup() {
  Serial.begin(115200);
  Wire.begin(48, 47);
  // setup code
  Serial.println("Buzzer init");
  buzzer_setup();
  Serial.println("Power init");
  power_setup();
  Serial.println("Led init");
  ledSetup();
  Serial.println("Tap init");
  tapSetup();
  Serial.println("Startup complete");

}

void loop() {
  
  ledLoop();
  powerOffButtonPressedLoop();
  tapLoop();
  }
