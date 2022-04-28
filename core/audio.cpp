#include "audio.h"

AudioController::AudioController() {
#ifdef FORMPU
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    config = pwm_get_default_config();
#endif
}

AudioController::~AudioController() {
    pwm_set_enabled(slice, false);
}

void AudioController::play() {
#ifdef FORMPU
    while (!songFreqs.empty()) {
        uint16_t freq = songFreqs.front();
        if(freq > 0) {
            float div = (float)clock_get_hz(clk_sys) / (freq * 10000);
            pwm_config_set_clkdiv(&config, div);
            pwm_config_set_wrap(&config, 10000); 
            pwm_init(slice, &config, true);
            pwm_set_gpio_level(BUZZER_PIN, dutyCycle);
        } else {
            pwm_set_gpio_level(BUZZER_PIN, 0);
        }
        songFreqs.pop();
        sleep_ms(this->noteDuration);
    }
    this->stop();
#endif
}

void AudioController::setNoteDuration(uint16_t duration) {
    this->noteDuration = duration;
}

void AudioController::sing(std::string song[], uint16_t num) {
    for (uint16_t i = 0; i < num; i++) {
        size_t index = std::distance(toneNames.begin(), std::find(toneNames.begin(), toneNames.end(), song[i]));
        uint16_t freq = toneFreqs[index];
        songFreqs.push(freq);
    }
    multicore_fifo_push_blocking(AUDIO_FLAG_VALUE);
}

void AudioController::beep(u_int16_t freq) {
    songFreqs.push(freq);
    multicore_fifo_push_blocking(AUDIO_FLAG_VALUE);
}

void AudioController::stop() {
    songFreqs = std::queue<uint16_t>();
    pwm_set_gpio_level(BUZZER_PIN, 0);
}
