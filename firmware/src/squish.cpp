#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>
#include "led.h"

Adafruit_LPS22 lps;
sensors_event_t pressure;
sensors_event_t temp;
bool isBeeingSquished = 0;

#define pressureThreshold 2
float dynamicPressureThreshold;

int32_t squishSetup(){
    
    if (!lps.begin_I2C(0x5D, &Wire)){
        Serial.println("Couldnt start pressure sensor");
        return 1;
    }
    lps.setDataRate(LPS22_RATE_75_HZ);
    lps.getEvent(&pressure, &temp);
    dynamicPressureThreshold = (pressure.pressure + pressureThreshold);
    return 0;
}


void squishLoop(){

    lps.getEvent(&pressure, &temp);
    if (isBeeingSquished && (pressure.pressure > dynamicPressureThreshold))
    {
        fastColorChange(0);
        isBeeingSquished = 0;

    }else if (!isBeeingSquished && (pressure.pressure <= dynamicPressureThreshold))
    {
        fastColorChange(1);
        isBeeingSquished = 1;
    }
    Serial.println(pressure.pressure);
}

bool isSquished(){
    return isBeeingSquished;
}