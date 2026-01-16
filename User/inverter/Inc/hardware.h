#ifndef HARDWARE_H
#define HARDWARE_H

    #include <stdint.h>
    
    /********************************** Timer Related Macros Define **********************************/
    #define SYSTEM_CLOCK_HZ      72000000UL       // 80 MHz system clock
    #define PWM_FREQUENCY_HZ     39000UL          // Target PWM frequency: 39 kHz
    #define PWM_PERIOD          ((SYSTEM_CLOCK_HZ / PWM_FREQUENCY_HZ) - 1)          // ARR value (Resolution)
    #define PWM_PRESCALER        ((SYSTEM_CLOCK_HZ / (PWM_FREQUENCY_HZ * PWM_PERIOD)) - 1)
    #define PWM_DUTY_CYCLE(percent) ((uint16_t)(((percent) * (PWM_PERIOD)) / 100))  // Convert percentage to CCR value
    #define DEFAULT_PWM_DUTY_CYCLE    0 // Common duty cycle macros
    
    
    #define ADC_CH_COUNT    4
    #define ADC_SAMPLE_COUNT   (ADC_CH_COUNT * 25)  // Total buffer length for DMA (e.g., 16 samples per channel)
    #define DMA_ADC              1  // Set to 1 to enable DMA for ADC
    
    #define ADC_SAMPLE_FREQUENCY_HZ    1000    // Example: 1 kHz sampling rate

    #define ADC_SAMPLE_PERIOD_MS       (1000 / ADC_SAMPLE_FREQUENCY_HZ)  // Period in ms
    
    #define RX_BUFFER_SIZE 10  // Compile-time constant


    /************************** ADC Related Macros Define *****************************************/
    #define ADC1_DR_Address ((uint32_t)0x4001244C)  // Address of ADC1 data register
    #define ADC_GPIO_Port         GPIOA
    #define ADC_CLK              RCC_APB2Periph_GPIOA

    // Analog Inputs
    #define BATTERY_VOLTAGE_SENSE_Pin   GPIO_Pin_0
    #define SOLAR_CURRENT_SENSE_Pin     GPIO_Pin_1
    #define SOLAR_VOLTAGE_SENSE_Pin     GPIO_Pin_2
    #define TEMPERATURE_SENSE_Pin       GPIO_Pin_3

    // Control Outputs PORT B
    #define MPPT_SHUTDOWN_Pin           GPIO_Pin_1
    #define BCCU_Pin                    GPIO_Pin_2
    #define FAN_DRIVE_Pin               GPIO_Pin_3
    #define RE_PV_Pin                   GPIO_Pin_4

    #define UART_TX_Pin                 GPIO_Pin_9
    #define UART_RX_Pin                 GPIO_Pin_10

    // PWM
    #define MPPT_PWM_Pin                GPIO_Pin_8

    // LCD
    #define LCD_BACKLIGHT_Pin           GPIO_Pin_2
    #define LCD_DATA0_Pin               GPIO_Pin_3
    #define LCD_DATA1_Pin               GPIO_Pin_4
    #define LCD_DATA2_Pin               GPIO_Pin_5
    #define LCD_DATA3_Pin               GPIO_Pin_6
    #define LCD_RS_Pin                  GPIO_Pin_7
    #define LCD_EN_Pin                  GPIO_Pin_10


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