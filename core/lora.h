#include "common.h"
#include "display.h"
#include "screen.h"
#include "SX126x.h"

#ifndef _GAME_TIGER_LORA_H
#define _GAME_TIGER_LORA_H

// #define ADV_DEBUG

class Lora : public SX126x
{
private:

public:

    Lora();
    ~Lora();

	bool HalGpioRead(GpioPinFunction_t pin);
	void HalGpioWrite(GpioPinFunction_t pin, bool value);
	void HalSpiTransfer(uint8_t *buffer_in, const uint8_t *buffer_out, uint16_t size);

	void checkStatus();
};

#endif