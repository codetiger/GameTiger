#include "../../common.h"
#include "sx126x_hal.h"

void hal_gpio_init(uint8_t pin, uint8_t dir, bool value) {
    gpio_init(pin);
    gpio_set_dir(pin, dir);
    gpio_put(pin, value);
}

void hal_gpio_put(uint8_t pin, bool value) {
    gpio_put(pin, value);
}

bool hal_gpio_get(uint8_t pin) {
    return gpio_get(pin);
}

sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;

    while(hal_gpio_get(sx126x_hal->busy)) { }
    
    uint8_t buffer[255];
    memcpy(buffer, command, command_length);
    memcpy(buffer + command_length, data, data_length);

    hal_gpio_put(sx126x_hal->nss, 0);
    spi_write_blocking(sx126x_hal->spi, buffer, (uint32_t)(command_length + data_length));
    hal_gpio_put(sx126x_hal->nss, 1);

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;

    while(hal_gpio_get(sx126x_hal->busy)) { }

    hal_gpio_put(sx126x_hal->nss, 0);

    uint8_t buffer[255];
    memcpy(buffer, command, command_length);

    hal_gpio_put(sx126x_hal->nss, 0);
    spi_write_blocking(sx126x_hal->spi, buffer, command_length);
    spi_read_blocking(sx126x_hal->spi, 0, data, data_length);
    hal_gpio_put(sx126x_hal->nss, 1);

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_reset( const void* context ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;
    
    sleep_ms(20);
    hal_gpio_put(sx126x_hal->reset, 0);
    sleep_ms(50);
    hal_gpio_put(sx126x_hal->reset, 1);
    sleep_ms(50);

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_wakeup( const void* context ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;
    
    hal_gpio_put(sx126x_hal->nss, 0);

    const uint8_t wakeup_sequence[2] = {0xC0, 0x00};    // SX126X_GET_STATUS, SX126X_NOP
    spi_write_blocking(sx126x_hal->spi, wakeup_sequence, 2);

	hal_gpio_put(sx126x_hal->nss, 1);

    while (hal_gpio_get(sx126x_hal->busy)) { }
    
	return SX126X_HAL_STATUS_OK;
}