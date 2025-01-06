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


void setup() {
  Serial.begin(115200);
  // setup code
  Serial.println("Power init");
  power_setup();
  Serial.println("led init");
  ledSetup();
  Serial.println("Startup complete");

}

void loop() {
  
  ledLoop();
  powerOffButtonPressedLoop();
  // put your main code here, to run repeatedly:
}
