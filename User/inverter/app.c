#include <stdio.h>
#include <string.h>
#include "hardware.h"
#include "ch32v10x.h"
#include  "conf.h"
#include "types.h"
#include "inverter.h"

extern Inverter dev;

extern uint8_t testFalg;

char tempBuffer[250];
extern FlagStatus SecFlag;
extern uint16_t ADC_Buffer[ADC_DMA_SAMPLES];


void wyre_setup()
{
    wyre_hardware_init();
}

void wyre_loop()
{
    static uint32_t last_measure_tick = 0;
    static uint32_t last_modulate_tick = 0;
    // static uint32_t last_uart_tick = 0;

    uint32_t current_tick       = wyre_get_tick();

    uint32_t measure_period_ms  = TASK_MEASURE_PERIOD_MS;
    uint32_t modulate_period_ms = TASK_MODULATE_PERIOD_MS;
    // uint32_t uart_period_ms     = 1000;

    if (measure_period_ms == 0 || (current_tick - last_measure_tick) >= measure_period_ms)
    {
        // task_measure(&mppt);
        last_measure_tick = current_tick;
    }

    if (modulate_period_ms == 0 ||(current_tick - last_modulate_tick) >= modulate_period_ms)
    {
        // task_modulate(&mppt);
        last_modulate_tick = current_tick;
    }

    // if (uart_period_ms == 0 || (current_tick - last_uart_tick) >= uart_period_ms)
    // {
    //     wyre_uart_send_pv_status();
    //     last_uart_tick = current_tick;
    // }

    if (SecFlag)
    {
        SecFlag = RESET;

        // Debug Log
        memset(tempBuffer,0,sizeof(tempBuffer));
        printf("Every Second\r\n");
    }
}
