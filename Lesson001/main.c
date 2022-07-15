#include "main.h"

static bool init_mc () {
    mc_init_gpio();

    if (!init_crg(CPU_FREQ)) {
        return false;
    }

    return true;
}

_Noreturn static void idle_thread (__attribute__((unused)) const void *obj) {
    while (true) {}
}

#define THREAD_NUMBER 3
static vsrtos_ram_t vsrtos_ram = {0};
static thread_ram_t thread_ram[THREAD_NUMBER] = {0};

static uint32_t idle_thread_heap[50];
static uint32_t led_blink_thread_heap[150];
static uint32_t eth_thread_heap[1000];

static const thread_cfg_t thread_cfg[THREAD_NUMBER] = {
    {
        .func = idle_thread,
        .obj = NULL,
        SET_THREAD_HEAP(idle_thread_heap)
        .ram = &thread_ram[0],
    },
    {
        .func = led_blink_thread,
        .obj = &led_blink_cfg,
        SET_THREAD_HEAP(led_blink_thread_heap)
        .ram = &thread_ram[1],
    },
    {
        .func = eth_thread,
        .obj = NULL,
        SET_THREAD_HEAP(eth_thread_heap)
        .ram = &thread_ram[2],
    }
};

const vsrtos_cfg_t vsrtos_cfg = {
    .ram = &vsrtos_ram,
    SET_THREADS(thread_cfg)
};

int main () {
    init_system();

    if (!init_mc()) {
        mc_reset();
    }

    vsrtos_start_scheduler(&vsrtos_cfg);
    mc_reset();
    return 0;
}
