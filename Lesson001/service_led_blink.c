#include "led_blink.h"
#include "vsrtos.h"
#include "gpio.h"

static void led_blink_func_set_pin_state (uint8_t state) {
    led_set_state(state);
}

const led_blink_cfg_t led_blink_cfg = {
    .period_ms = MS_TO_TICK(1000),
    .f_set = led_blink_func_set_pin_state
};

void service_init_led_blink () {
    init_led_blink(&led_blink_cfg);
}
