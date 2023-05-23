#include "common.h"

#ifndef _GAME_TIGER_LORA_H
#define _GAME_TIGER_LORA_H

#include "sx126x_driver/src/sx126x.h"
#include "sx126x_driver/src/sx126x_hal.h"

#define GPIO_PIN_SCK 18
#define GPIO_PIN_MOSI 19
#define GPIO_PIN_MISO 20

class Lora
{
private:
	sx126x_hal_t context;
public:

    Lora();
    ~Lora();

	void SendData(char* data, uint8_t length);
};

#endif