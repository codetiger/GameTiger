#include "lora.h"

#ifdef RP2040
#define LogPrintf printf
#else
#include <Arduino.h>
void LogPrintf(const char *format, ...) {
  char buf[256];
  va_list ap;
  va_start(ap, F(format));
  vsnprintf(buf, sizeof(buf), format, ap); 
  va_end(ap);
  Serial.print(buf);
};
#endif


Lora::Lora() {
    LogPrintf("[Lora] Loading driver...\n");

    #ifdef RP2040
    context.busy = 6;
    context.reset = 0;
    context.nss = 1;
    context.irq = 25;
    context.sck = 18;
    context.mosi = 19;
    context.miso = 20;
	context.txen = -1;
	context.rxen = -1;
    #else
    context.busy = 0;
    context.reset = 1;
    context.nss = 7;
    context.irq = 2;
    context.sck = 8;
    context.mosi = 10;
    context.miso = 9;
	context.txen = 4;
	context.rxen = 5;
    #endif
	context.baudrate = 10*1000*1000;

    hal_gpio_init(context.nss, GPIO_OUT, 1);
    hal_gpio_init(context.reset, GPIO_OUT, 1);
	if(context.txen != -1)
	    hal_gpio_init(context.txen, GPIO_OUT, 1);
	if(context.rxen != -1)
	    hal_gpio_init(context.rxen, GPIO_OUT, 0);

    #ifdef RP2040
    context.spi = 0;
    int br = spi_init(context.spi ? spi1 : spi0, context.baudrate);
    printf("[Lora] SX1262 baudrate: %d\n", br);

    spi_set_format(context.spi ? spi1 : spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(context.sck, GPIO_FUNC_SPI);
    gpio_set_function(context.mosi, GPIO_FUNC_SPI);
    gpio_set_function(context.miso, GPIO_FUNC_SPI);
    #endif

    hal_gpio_init(context.busy, GPIO_IN, 0);
    hal_gpio_init(context.irq, GPIO_IN, 0);

	while (hal_gpio_get(context.busy));

	sx126x_status_t status;
	sx126x_chip_status_t radio_status;
	if(sx126x_get_status(&context, &radio_status) == SX126X_STATUS_OK) {
		if(radio_status.chip_mode == SX126X_CHIP_MODE_STBY_RC) {
			LogPrintf("[Lora] RadioChip already in STBY_RC\n");
		} else {
			status = sx126x_set_standby(&context, SX126X_STANDBY_CFG_RC);
			if(status != SX126X_STATUS_OK)
				LogPrintf("[Lora] Error in set_standby: %d\n", status);
		}
	} else {
		LogPrintf("[Lora] Error in get_status: %d\n", status);
	}

	status = sx126x_set_reg_mode(&context, SX126X_REG_MODE_DCDC);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_reg_mode: %d\n", status);

    sx126x_pa_cfg_params_t pa_params = {
		.pa_duty_cycle = 0x04,
		.hp_max = 0x07,
		.device_sel = 0x00,
		.pa_lut = 0x01
	};
    status = sx126x_set_pa_cfg(&context, &pa_params);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_pa_cfg: %d\n", status);

	status = sx126x_set_dio3_as_tcxo_ctrl(&context, SX126X_TCXO_CTRL_3_3V, 0x64);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_dio3_as_tcxo_ctrl: %d\n", status);

	status = sx126x_cal(&context, SX126X_CAL_ALL);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in cal: %d\n", status);

	status = sx126x_cal_img(&context, 0xD7, 0xD8);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in cal_img: %d\n", status);

	status = sx126x_set_dio2_as_rf_sw_ctrl(&context, true);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_dio2_as_rf_sw_ctrl: %d\n", status);

    status = sx126x_set_pkt_type(&context, SX126X_PKT_TYPE_LORA);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_pkt_type: %d\n", status);

    status = sx126x_set_rf_freq(&context, 868 * 1000 * 1000);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_rf_freq: %d\n", status);

    sx126x_mod_params_lora_t lora_mod_params = {
		.sf = SX126X_LORA_SF7,
		.bw = SX126X_LORA_BW_125,
		.cr = SX126X_LORA_CR_4_5,
		.ldro = 0x00
	  };
    status = sx126x_set_lora_mod_params(&context, &lora_mod_params);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_lora_mod_params: %d\n", status);

	status = sx126x_set_buffer_base_address(&context, 0x00, 0x00);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_buffer_base_address: %d\n", status);

    sx126x_pkt_params_lora_t lora_pkt_params = {
		.preamble_len_in_symb = 8,
		.header_type = SX126X_LORA_PKT_EXPLICIT,
		.pld_len_in_bytes = 0xFF,
		.crc_is_on = true,
		.invert_iq_is_on = false
	};
	status = sx126x_set_lora_pkt_params(&context, &lora_pkt_params);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_lora_pkt_params: %d\n", status);

	static const uint16_t lora_irq_mask_dio1 = SX126X_IRQ_TX_DONE | SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT | SX126X_IRQ_CRC_ERROR;
	status = sx126x_set_dio_irq_params(&context, lora_irq_mask_dio1, lora_irq_mask_dio1, 0, 0);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_dio_irq_params: %d\n", status);

	status = sx126x_cfg_rx_boosted(&context, true);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in cfg_rx_boosted: %d\n", status);

	status = sx126x_set_lora_sync_word(&context, 0x12);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in clear_irq_status: %d\n", status);

	CheckDeviceErrors();
	LogPrintf("[Lora] Done\n");
}

Lora::~Lora() {
    
}

void Lora::CheckDeviceStatus() {
	sx126x_chip_status_t radio_status;
	if(sx126x_get_status(&context, &radio_status) == SX126X_STATUS_OK) {
		LogPrintf("[Lora] RadioChip Status: %d, %d\n", radio_status.chip_mode, radio_status.cmd_status);
	}

	sx126x_rx_buffer_status_t rx_buf_stat;
	if(sx126x_get_rx_buffer_status(&context, &rx_buf_stat) == SX126X_STATUS_OK) {
		LogPrintf("[Lora] RX Buffer Len: %d\n", rx_buf_stat.pld_len_in_bytes);
	}
}

void Lora::CheckDeviceErrors() {
	sx126x_errors_mask_t errors;
	sx126x_get_device_errors(&context, &errors);
	if(errors != 0)
		LogPrintf("[Lora] Error %d\n", errors);
	sx126x_clear_device_errors(&context);
}

void Lora::SetTxEnable() {
	if ((context.txen != -1) && (context.rxen != -1)) {
		hal_gpio_put(context.txen, 1);
		hal_gpio_put(context.rxen, 0);
  	} else {
      LogPrintf("[Lora] Error TX Enable\n");
    }
}

void Lora::SetRxEnable() {
	if ((context.txen != -1) && (context.rxen != -1)) {
		hal_gpio_put(context.rxen, 1);
		hal_gpio_put(context.txen, 0);
  	} else {
      LogPrintf("[Lora] Error RX Enable\n");
    }
}

void Lora::SendData(int8_t power, char* data, uint8_t length) {
	sx126x_set_standby(&context, SX126X_STANDBY_CFG_RC);
	sx126x_status_t status = sx126x_set_buffer_base_address(&context, 0x00, 0x00);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_buffer_base_address: %d\n", status);

    sx126x_pkt_params_lora_t lora_pkt_params = {
		.preamble_len_in_symb = 8,
		.header_type = SX126X_LORA_PKT_EXPLICIT,
		.pld_len_in_bytes = length,
		.crc_is_on = true,
		.invert_iq_is_on = false
	};
	status = sx126x_set_lora_pkt_params(&context, &lora_pkt_params);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_lora_pkt_params: %d\n", status);

	status = sx126x_write_buffer(&context, 0, (uint8_t*)data, length);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in write_buffer: %d\n", status);

	status = sx126x_set_tx_params(&context, power, SX126X_RAMP_200_US);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_tx_params: %d\n", status);

	status = sx126x_set_tx(&context, SX126X_MAX_TIMEOUT_IN_MS);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error in set_tx: %d\n", status);

	CheckDeviceErrors();
}

void Lora::ProcessIrq() {
	sx126x_irq_mask_t irq_mask;
	sx126x_status_t status = sx126x_get_irq_status(&context, &irq_mask);
	if(status == 0) {
		sx126x_clear_irq_status(&context, irq_mask);

		if (irq_mask & SX126X_IRQ_TX_DONE) {
			LogPrintf("[Lora] IRQ TX_DONE\n");
		}

		if (irq_mask & SX126X_IRQ_RX_DONE) {
			LogPrintf("[Lora] IRQ RX_DONE\n");
			receiveData();
		}

		if (irq_mask & SX126X_IRQ_PREAMBLE_DETECTED) {
			LogPrintf("[Lora] IRQ PREAMBLE_DETECTED\n");
		}

		if (irq_mask & SX126X_IRQ_SYNC_WORD_VALID) {
			LogPrintf("[Lora] IRQ SYNC_WORD_VALID\n");
		}

		if (irq_mask & SX126X_IRQ_HEADER_VALID) {
			LogPrintf("[Lora] IRQ HEADER_VALID\n");
		}

		if (irq_mask & SX126X_IRQ_HEADER_ERROR) {
			LogPrintf("[Lora] IRQ HEADER_ERROR\n");
		}

		if (irq_mask & SX126X_IRQ_CRC_ERROR) {
			LogPrintf("[Lora] IRQ CRC_ERROR\n");
		}

		if (irq_mask & SX126X_IRQ_CAD_DONE) {
			LogPrintf("[Lora] IRQ CAD_DONE\n");
		}

		if (irq_mask & SX126X_IRQ_CAD_DETECTED) {
			LogPrintf("[Lora] IRQ CAD_DETECTED\n");
		}

		if (irq_mask & SX126X_IRQ_TIMEOUT) {
			LogPrintf("[Lora] IRQ TIMEOUT\n");
		}
	} else {
		LogPrintf("[Lora] IRQ Error %d\n", status);
	}
}

void Lora::SetToReceiveMode() {
	sx126x_status_t status = sx126x_set_rx(&context, 60 * 1000);
	if(status != SX126X_STATUS_OK)
		LogPrintf("[Lora] Error set rx: %d\n", status);
	CheckDeviceErrors();
}

void Lora::receiveData() {
	sx126x_set_standby(&context, SX126X_STANDBY_CFG_RC);

	sx126x_pkt_status_lora_t last_rcvd_pkt_status;

	sx126x_get_lora_pkt_status(&context, &last_rcvd_pkt_status);
	LogPrintf("[Lora] Process_RX: RF status: RSSI: %d, SNR: %d, RSCP: %d\n", last_rcvd_pkt_status.rssi_pkt_in_dbm, last_rcvd_pkt_status.snr_pkt_in_db, last_rcvd_pkt_status.signal_rssi_pkt_in_dbm);

	sx126x_rx_buffer_status_t rx_buf_stat;
	sx126x_get_rx_buffer_status(&context, &rx_buf_stat);
	if(rx_buf_stat.pld_len_in_bytes > 0) {
		uint8_t buf[256];
		sx126x_read_buffer(&context, rx_buf_stat.buffer_start_pointer, buf, rx_buf_stat.pld_len_in_bytes);
		if(rx_buf_stat.pld_len_in_bytes > 0)
			LogPrintf("[Lora] Received Data: %s\n", (char *)buf);
	}

	SetToReceiveMode();
}
