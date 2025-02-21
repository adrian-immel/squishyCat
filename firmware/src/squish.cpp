#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>
#include "led.h"

Adafruit_LPS22 lps;
sensors_event_t pressure;
sensors_event_t temp;
bool isBeeingSquished = 0;

#define PRESSURE_THRESHOLD_OFFSET 2
#define HYSTERESIS 1
#define SMOOTHING_FACTOR 0.1 // Alpha for exponential smoothing
#define TREND_FACTOR 0.05 // Adjust based on the rate of pressure change

float dynamicPressureThreshold;
float smoothedPressure = 0;
float trend = 0;
// Global variable to keep track of the previous smoothed pressure for trend calculation
float previousSmoothedPressure = 0;

int32_t squishSetup() {
  if (!lps.begin_I2C(0x5D, &Wire)) {
    Serial.println("Couldn't start pressure sensor");
    return 1;
  }
  lps.setDataRate(LPS22_RATE_75_HZ);
  lps.getEvent(&pressure, &temp);
  smoothedPressure = pressure.pressure; // Initialize with the first reading
  dynamicPressureThreshold = smoothedPressure + PRESSURE_THRESHOLD_OFFSET;
  return 0;
}

void squishLoop() {
  lps.getEvent(&pressure, &temp);
  float currentPressure = pressure.pressure;

  // Apply exponential smoothing to the pressure reading
  smoothedPressure = SMOOTHING_FACTOR * currentPressure + (1 - SMOOTHING_FACTOR) * smoothedPressure;

  // Calculate the rate of change (trend)
  trend = (smoothedPressure - previousSmoothedPressure) * TREND_FACTOR;
  previousSmoothedPressure = smoothedPressure;

  // Update dynamic pressure threshold with trend consideration
  dynamicPressureThreshold = smoothedPressure + PRESSURE_THRESHOLD_OFFSET + trend;

  if (isBeeingSquished) {
    // Use lower threshold minus hysteresis to reset
    if (currentPressure < dynamicPressureThreshold - HYSTERESIS) {
      fastColorChange(0);
      isBeeingSquished = 0;
    }
  } else {
    if (millis() > 2000 && currentPressure > dynamicPressureThreshold) {
      fastColorChange(1);
      isBeeingSquished = 1;
    }
  }

  // Debugging information
  Serial.print("Current Pressure: ");
  Serial.print(currentPressure);
  Serial.print(" | Smoothed Pressure: ");
  Serial.print(smoothedPressure);
  Serial.print(" | Dynamic Threshold: ");
  Serial.println(dynamicPressureThreshold);
}

bool isSquished() {
  return isBeeingSquished;
}

