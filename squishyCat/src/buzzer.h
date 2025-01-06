#ifndef __BUZZER_H__
#define __BUZZER_H__

int32_t buzzer_setup();
void buzzer_tone(uint16_t frequency, uint8_t duration);

#endif /* __BUZZER_H__ */