#ifndef HARDWARE_H
#define HARDWARE_H

    #include <stdint.h>
    #include "setup_config.h"
    
    void ADC_GetAverage(uint16_t avg_out[ADC_CH_COUNT]);
    // void wyre_gpio_set_state(MPPT_GPIO_Pin_t pin, bool state);
    void wyre_hardware_init(void);
    void wyre_adc_init(void);
    uint16_t* wyre_adc_buffer(void);
    void wyre_timer_init(void);
    uint8_t wyre_pwm_set_duty_cycle(uint8_t duty_value);
    uint32_t wyre_get_tick(void);
    void wyre_SysTick_Init(void);
    void wyre_uart_transmit_it(uint8_t *data, uint16_t length);
    char wyre_uart_receive_it(uint8_t *data, uint16_t length);
    void wyre_uart_start_receive(void);
    void wyre_uart_init(void);
    void wyre_gpio_init(void);
    void wyre_setup(void);
    void wyre_loop();
    void wyre_adc_init_multi_dma(void);
    void wyre_uart_send_pv_status(void);
    void wyre_adc_read_all(void);
    void wyre_print(const char *str);
#endif