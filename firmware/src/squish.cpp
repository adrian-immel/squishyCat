    #include <Arduino.h>
    #include <Wire.h>
    #include <Adafruit_LPS2X.h>
    #include <Adafruit_Sensor.h>
    #include "led.h"

    Adafruit_LPS22 lps;
    sensors_event_t pressure;
    sensors_event_t temp;
    unsigned long lastsquish = millis();
    bool   firstRead     = true;
    float  avgPressure   = 0;

    #define pressureThreshold 0.15
    #define interuptPin 12

    bool newValueAvalible = false;

    void IRAM_ATTR ISR_function() {
        newValueAvalible = true;
    }

    int32_t squishSetup(){
        
        if (!lps.begin_I2C(0x5D, &Wire)){
            Serial.println("Couldnt start pressure sensor");
            return 1;
        }
        lps.setDataRate(LPS22_RATE_50_HZ);
        delay(25); //wait for first reading
        lps.getEvent(&pressure, &temp);
        avgPressure = pressure.pressure; // first sample just seeds it
        
        // setup interupt for new value
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
        attachInterrupt(digitalPinToInterrupt(interuptPin), ISR_function, RISING);

        return 0;
    }


    void squishLoop(){
                
        if (!newValueAvalible ) return;   // Only proceed if there's a new value 
        newValueAvalible = false;

        lps.getEvent(&pressure, &temp);

        float movingpressureThreshold = avgPressure + pressureThreshold;
        
 
        if(pressure.pressure > movingpressureThreshold && millis() > 1000){
            updateLed(getCurrentHue() + 70);
            lastsquish = millis();
            Serial.println("Squish detected");
            // (9/10)*old + (1/10)*new
            avgPressure = (avgPressure * 9 + pressure.pressure) / 10.0;
        }else{
            // (5/6)*old + (1/6)*new
            avgPressure = (avgPressure * 5 + pressure.pressure) / 6.0;

        }

        // Debugging: Print the current pressure and dynamic threshold
        //Serial.print("Current pressure:");
        //Serial.println(pressure.pressure);
        //Serial.print("Average Pressure:");
        //Serial.println(avgPressure);

    }

    bool isSquished(){
        return (lastsquish + 500 > millis());
    }
