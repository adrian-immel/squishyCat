#include <Arduino.h>
#include "power.h"
#include "led.h"
#include "buzzer.h"


#define POWER_BTN   GPIO_NUM_10
#define POWER_ON    GPIO_NUM_9
#define ADC_PWR_EN  GPIO_NUM_7
#define ADC_BAT_V   GPIO_NUM_6
#define CHG_STAT    GPIO_NUM_11

int32_t power_setup()
{   
    delay(10);
    pinMode(POWER_BTN, INPUT);
    pinMode(POWER_ON, OUTPUT);
    if (digitalRead(POWER_BTN))
        digitalWrite(POWER_ON, HIGH);
    buzzer_setup();
    buzzer_tone(440, 50);
    buzzer_tone(880, 75);

    return 0;
}

void power_off()
{
    ledOff();
    buzzer_tone(880, 50);
    buzzer_tone(440, 50);
    buzzer_tone(220, 50);
    Serial.println("Shutdown done!");
    digitalWrite(POWER_ON, LOW);
    esp_deep_sleep_start(); //shut of the esp

}

void powerOffButtonPressedLoop (){
    Serial.println("Shutdown Initalized");
    unsigned long buttonDownTime = millis();
    while (digitalRead(POWER_BTN))
    {
        if (millis() - buttonDownTime >= 1500){
            power_off();
        }
        delay(10);
    }

}