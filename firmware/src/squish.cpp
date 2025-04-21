    #include <Arduino.h>
    #include <Wire.h>
    #include <Adafruit_LPS2X.h>
    #include <Adafruit_Sensor.h>
    #include "led.h"

    Adafruit_LPS22 lps;
    sensors_event_t pressure;
    sensors_event_t temp;
    unsigned long lastSquish = millis();
    float  avgPressure   = 0.0;

    #define pressureThreshold 0.12
    #define interruptPin 12
    #define NUM_READINGS 10


    float pressureReadings[NUM_READINGS];
    int readingIndex = 0;
    bool new_value_avalible = false;

    void IRAM_ATTR ISR_function() {
        new_value_avalible = true;
    }

    int32_t squishSetup(){
        
        if (!lps.begin_I2C(0x5D, &Wire)){
            Serial.println("Couldnt start pressure sensor");
            return 1;
        }
        lps.setDataRate(LPS22_RATE_50_HZ);
        // setup interrupt for new value
        lps.configureInterrupt(
            false,  // activeLow?  false => it will go HIGH on interrupt
            false,  // openDrain?  false => push‑pull output
            true,   // data_ready?
            false,  // pres_high?
            false,  // pres_low?
            false,  // fifo_full?
            false,  // fifo_watermark?
            false   // fifo_overflow?
          );
        lps.getEvent(&pressure, &temp);
        avgPressure = pressure.pressure;
        attachInterrupt(digitalPinToInterrupt(interruptPin), ISR_function, RISING);

        return 0;
    }


    void squishLoop(){
                
        if (!new_value_avalible ) return;   // Only proceed if there's a new value
        new_value_avalible = false;
        // Store the current pressure reading in the buffer
        pressureReadings[readingIndex] = pressure.pressure;
        lps.getEvent(&pressure, &temp);
        readingIndex = (readingIndex + 1) % NUM_READINGS;

        // Calculate the sum of the last 5 readings
        float sum = 0;
        for (const float pressureReading : pressureReadings) {
            sum += pressureReading;
        }

        // Calculate the average pressure
        avgPressure = sum / NUM_READINGS;

        float movingPressureThreshold = avgPressure + pressureThreshold;


 
        if(pressure.pressure > movingPressureThreshold && millis() > 1000){
            updateLed(getCurrentHue() + 30);
            lastSquish = millis();
            Serial.println("Squish detected");
            pressureReadings[readingIndex] = (pressureReadings[readingIndex]) - 0.12;
        }

        // Debugging: Print the current pressure and dynamic threshold
        //Serial.print("Current pressure:");
        //Serial.println(pressure.pressure);
        //Serial.print("Average Pressure:");
        //Serial.println(avgPressure);

    }

    bool isSquished(){
        return (lastSquish + 500 > millis());
    }
