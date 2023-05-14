#include "lora.h"

Lora::Lora() {
    printf("Lora driver loading... ");

    gpio_init(GPIO_PIN_CS);
    gpio_set_dir(GPIO_PIN_CS, GPIO_OUT);

    gpio_init(GPIO_PIN_RESET);
    gpio_set_dir(GPIO_PIN_RESET, GPIO_OUT);

    gpio_init(GPIO_PIN_MISO);
    gpio_set_dir(GPIO_PIN_MISO, GPIO_OUT);

    int br = spi_init(spi0, 16 * 1000 * 1000);
    printf("SX1262 baudrate: %d\n", br);

    gpio_set_function(GPIO_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(GPIO_PIN_BUSY);
    gpio_set_dir(GPIO_PIN_BUSY, GPIO_IN);

    gpio_init(GPIO_PIN_DIO1);
    gpio_set_dir(GPIO_PIN_DIO1, GPIO_IN);

    SetDeviceType(SX1262);
    Init();

    SetNetworkType(PRIVATE);

    ModulationParams_t modulationParams;
    modulationParams.PacketType = PACKET_TYPE_LORA;
    modulationParams.Params.LoRa.Bandwidth = LORA_BW_500;
    modulationParams.Params.LoRa.CodingRate = LORA_CR_4_8;
    modulationParams.Params.LoRa.SpreadingFactor = LORA_SF12;
    modulationParams.Params.LoRa.LowDatarateOptimize = 0;
    SetModulationParams(modulationParams);

    PacketParams_t packetParams;
    packetParams.PacketType = PACKET_TYPE_LORA;
    packetParams.Params.LoRa.HeaderType = LORA_PACKET_VARIABLE_LENGTH;
    packetParams.Params.LoRa.PreambleLength = 8;
    packetParams.Params.LoRa.CrcMode = LORA_CRC_ON;
    packetParams.Params.LoRa.InvertIQ = LORA_IQ_NORMAL;
    SetPacketParams(packetParams);
    SetTxParams(-5, RADIO_RAMP_200_US);
    SetRfFrequency(866);
    
    printf("Done\n");
}

Lora::~Lora() {
    
}

bool Lora::HalGpioRead(GpioPinFunction_t pin) {
    return gpio_get(pin);
}

void Lora::HalGpioWrite(GpioPinFunction_t pin, bool value) {
    gpio_put(pin, value);
}

void Lora::HalSpiTransfer(uint8_t *buffer_in, const uint8_t *buffer_out, uint16_t size) {
	gpio_put(GPIO_PIN_CS, 0);
    spi_write_blocking(spi0, buffer_out, size);
	uint num_bytes_read = spi_read_blocking(spi0, 0, buffer_in, size);
	gpio_put(GPIO_PIN_CS, 1);
}

void Lora::checkStatus() {
    RadioStatus_t status = GetStatus();
    printf("Lora Chip Status: %d\n", status.Value);
}