#include "sx126x_hal.h"
#ifndef RP2040
#include <Arduino.h>
#include <SPI.h>

#define sleep_ms delay
#define sleep_us delayMicroseconds
#else
#include "../common.h"
#endif

void hal_gpio_init(uint8_t pin, uint8_t dir, bool value) {
    #ifdef RP2040
    gpio_init(pin);
    gpio_set_dir(pin, dir);
    gpio_put(pin, value);
    #else
    pinMode(pin, dir);
    digitalWrite(pin, value);
    #endif
}

void hal_gpio_put(uint8_t pin, bool value) {
    #ifdef RP2040
    gpio_put(pin, value);
    #else
    digitalWrite(pin, value);
    #endif
}

bool hal_gpio_get(uint8_t pin) {
    #ifdef RP2040
    return gpio_get(pin);
    #else
    return digitalRead(pin);
    #endif
}

sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;

    while(hal_gpio_get(sx126x_hal->busy));
    
    uint8_t buffer[255];
    memcpy(buffer, command, command_length);
    memcpy(buffer + command_length, data, data_length);

    #ifdef RP2040
    hal_gpio_put(sx126x_hal->nss, 0);
    spi_write_blocking(sx126x_hal->spi ? spi1 : spi0, buffer, (uint32_t)(command_length + data_length));
    hal_gpio_put(sx126x_hal->nss, 1);
    #else
    SPI.beginTransaction(SPISettings(sx126x_hal->baudrate, MSBFIRST, SPI_MODE0));
    hal_gpio_put(sx126x_hal->nss, 0);
    SPI.transfer((void*)buffer, (uint32_t)(command_length + data_length));
    hal_gpio_put(sx126x_hal->nss, 1);
    SPI.endTransaction();
    #endif

    while(hal_gpio_get(sx126x_hal->busy));

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;

    while(hal_gpio_get(sx126x_hal->busy));

    #ifdef RP2040
    hal_gpio_put(sx126x_hal->nss, 0);
    spi_write_blocking(sx126x_hal->spi ? spi1 : spi0, command, command_length);
    spi_read_blocking(sx126x_hal->spi ? spi1 : spi0, 0, data, data_length);
    hal_gpio_put(sx126x_hal->nss, 1);
    #else
    SPI.beginTransaction(SPISettings(sx126x_hal->baudrate, MSBFIRST, SPI_MODE0));
    hal_gpio_put(sx126x_hal->nss, 0);

    SPI.transfer((void*)command, command_length);
    for( uint16_t i = 0; i < data_length; i++ )
        data[i] = SPI.transfer(0x0);

    hal_gpio_put(sx126x_hal->nss, 1);
    SPI.endTransaction();
    #endif

    while(hal_gpio_get(sx126x_hal->busy));

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_reset( const void* context ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;
    
    hal_gpio_put(sx126x_hal->reset, 1);
    sleep_us(150);
    hal_gpio_put(sx126x_hal->reset, 0);
    sleep_us(150);
    hal_gpio_put(sx126x_hal->reset, 1);

    while (hal_gpio_get(sx126x_hal->busy));

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_wakeup( const void* context ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;
    
    hal_gpio_put(sx126x_hal->nss, 0);

    const uint8_t wakeup_sequence[2] = {0xC0, 0x00};    // SX126X_GET_STATUS, SX126X_NOP
    #ifdef RP2040
    spi_write_blocking(sx126x_hal->spi ? spi1 : spi0, wakeup_sequence, 2);
    #else
    SPI.transfer(0xC0);
    SPI.transfer(0x00);
    #endif

	hal_gpio_put(sx126x_hal->nss, 1);

    while (hal_gpio_get(sx126x_hal->busy));
    
	return SX126X_HAL_STATUS_OK;
}