#ifndef BALANCE_MAIN_H
#define BALANCE_MAIN_H

#include "vsrtos.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "gpio.h"
#include "mc.h"

#include "vsrtos.h"
#include "vsrtos_cfg.h"

#include "led_blink.h"

void mc_init_gpio ();

#define UDP_TX_PORT 5270
#define UDP_RX_PORT 5280

_Noreturn void eth_thread (const void *obj);

extern const led_blink_cfg_t led_blink_cfg;

#endif // BALANCE_MAIN_H
