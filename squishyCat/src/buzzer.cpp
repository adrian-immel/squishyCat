#include <Arduino.h>
#include "buzzer.h"

#define BUZZER_PIN		GPIO_NUM_8
#define BUZZER_PWM_CH  	1
#define BUZZER_PWM_F   	2000

int32_t buzzer_setup()
{
    ledcSetup(BUZZER_PWM_CH, BUZZER_PWM_F, 8);
	ledcAttachPin(BUZZER_PIN, BUZZER_PWM_CH);
	ledcWrite(BUZZER_PWM_CH, 0);

    return 0;
}

void buzzer_tone(uint16_t frequency, uint8_t duration)
{
	ledcWriteTone(BUZZER_PWM_CH, frequency);
	delay(duration);
	ledcWriteTone(BUZZER_PWM_CH, 0);
}