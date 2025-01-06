#ifndef __LED_H__
#define __LED_H__

int32_t ledSetup();
void ledLoop();
void updateLed(uint8_t);
void ledOff();

#endif /* __LED_H__ */