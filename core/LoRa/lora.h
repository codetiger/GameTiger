#ifndef _GAME_TIGER_LORA_H
#define _GAME_TIGER_LORA_H

#include "sx126x.h"
#include "sx126x_hal.h"

#ifdef RP2040
#include "../common.h"
#endif

class Lora
{
private:
	sx126x_hal_t context;
    void receiveData();
public:

    Lora(int8_t power);
    ~Lora();

	void SendData(char* data, uint8_t length);
    void ProcessIrq();
    void SetToReceiveMode();

    void SetTxEnable();
    void SetRxEnable();

    void CheckDeviceErrors();
    void CheckDeviceStatus();
};

#endif