#include "lora.h"

Lora::Lora() {
    printf("[Lora] Loading driver...\n");

    context.busy = 6;
    context.reset = 0;
    context.nss = 1;
    context.irq = 25;
    context.spi = spi0;

    hal_gpio_init(context.nss, GPIO_OUT, 1);
    hal_gpio_init(context.reset, GPIO_OUT, 0);
    hal_gpio_init(context.reset, GPIO_OUT, 0);

    #ifdef FORMPU
    int br = spi_init(spi0, 16 * 1000 * 1000);
    printf("[Lora] SX1262 baudrate: %d\n", br);
    #endif

    gpio_set_function(GPIO_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_PIN_MOSI, GPIO_FUNC_SPI);

    hal_gpio_init(context.busy, GPIO_IN, 0);
    hal_gpio_init(context.irq, GPIO_IN, 0);

    sx126x_reset(&context);
    sx126x_wakeup(&context);

    sx126x_set_reg_mode(&context, SX126X_REG_MODE_DCDC);
    sx126x_set_standby(&context, SX126X_STANDBY_CFG_RC);
    sx126x_set_pkt_type(&context, SX126X_PKT_TYPE_LORA);
    sx126x_set_dio3_as_tcxo_ctrl(&context, SX126X_TCXO_CTRL_1_8V, 32);

    sx126x_cal(&context, SX126X_CAL_ALL);
    sx126x_set_standby(&context, SX126X_STANDBY_CFG_XOSC);

    sx126x_pa_cfg_params_t pa_params = {
		.pa_duty_cycle = 0x04,
		.hp_max = 0x07,
		.device_sel = 0x00,
		.pa_lut = 0x01
	};
    sx126x_set_pa_cfg(&context, &pa_params);
    sx126x_set_ocp_value(&context, SX126X_OCP_PARAM_VALUE_140_MA);
    sx126x_cfg_tx_clamp(&context);
    sx126x_set_tx_params(&context, 0, SX126X_RAMP_200_US);
    sx126x_set_rf_freq(&context, 868 * 1000 * 1000);
    sx126x_cal_img(&context, 0xD7, 0xDB);

    sx126x_set_buffer_base_address(&context, 0x00, 0x00);
    sx126x_set_pkt_type(&context, SX126X_PKT_TYPE_LORA);
    sx126x_set_rx_tx_fallback_mode(&context, SX126X_FALLBACK_FS);

    sx126x_mod_params_lora_t lora_mod_params = {
		.sf = SX126X_LORA_SF12,
		.bw = SX126X_LORA_BW_062,
		.cr = SX126X_LORA_CR_4_5,
		.ldro = 1
	};
    sx126x_set_lora_mod_params(&context, &lora_mod_params);

    sx126x_pkt_params_lora_t lora_pkt_params = {
		.preamble_len_in_symb = 64,
		.header_type = SX126X_LORA_PKT_EXPLICIT,
		.pld_len_in_bytes = 32,
		.crc_is_on = 1,
		.invert_iq_is_on = 0
	};
	sx126x_set_lora_pkt_params(&context, &lora_pkt_params);

    static const uint16_t lora_irq_mask_dio1 = SX126X_IRQ_TX_DONE | SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT | SX126X_IRQ_CRC_ERROR;
	static const uint16_t lora_irq_mask_dio2 = SX126X_IRQ_NONE;

	sx126x_set_dio_irq_params(&context, lora_irq_mask_dio1 | lora_irq_mask_dio2, lora_irq_mask_dio1, lora_irq_mask_dio2, 0);

	sx126x_clear_irq_status(&context, SX126X_IRQ_ALL);
	sx126x_cfg_rx_boosted(&context, true);
	// sx126x_set_tx_infinite_preamble(&context);
    sx126x_set_rx(&context, SX126X_MAX_TIMEOUT_IN_MS);

    printf("[Lora] Done\n");
}

Lora::~Lora() {
    
}

void Lora::SendData(char* data, uint8_t length) {
	sx126x_pkt_params_lora_t lora_pkt_params = {
		.preamble_len_in_symb = 16,
		.header_type = SX126X_LORA_PKT_EXPLICIT,
		.pld_len_in_bytes = length,
		.crc_is_on = 0,
		.invert_iq_is_on = 0
	};

	printf("[Lora] Sending packet: len=%zu\n", length);

	sx126x_set_lora_pkt_params(&context, &lora_pkt_params);
	sx126x_write_buffer(&context, 0, (uint8_t*)data, length);
	sx126x_set_tx(&context, SX126X_MAX_TIMEOUT_IN_MS);
    printf("[Lora] Done Sending\n");
}
