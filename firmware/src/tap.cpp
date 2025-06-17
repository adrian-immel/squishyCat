#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include "led.h"
#include "buzzer.h"
#include "squish.h"

// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICKTHRESHHOLD 35
#define DEBOUNCEDELAY 300

Adafruit_LIS3DH lis = Adafruit_LIS3DH();
unsigned long lastDebounceTime = millis();

int32_t tapSetup(){
    
    if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
        Serial.println("Couldnt start tap sensor");
        return 1;
    }
    lis.setRange(LIS3DH_RANGE_8_G);
    // 0 = turn off click detection & interrupt
    // 1 = single click only interrupt output
    // 2 = double click only interrupt output, detect single click
    // Adjust threshhold, higher numbers are less sensitive
    lis.setClick(1, CLICKTHRESHHOLD);
    return 0;
}

void tapLoop(){
    uint8_t click = lis.getClick();
    if ((click & 0x10) && ((millis() - lastDebounceTime) > DEBOUNCEDELAY) && !isSquished() && millis() > 700){
        setRandomColor();
        lastDebounceTime = millis();
        Serial.println("Tap detected");

    }

}