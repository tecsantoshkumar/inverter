#ifndef __setup_config_H
#define __setup_config_H
    /********************************** Timer Related Macros Define **********************************/
    #define SYSTEM_CLOCK_HZ      72000000UL       // 80 MHz system clock
    #define PWM_FREQUENCY_HZ     39000UL          // Target PWM frequency: 39 kHz
    #define PWM_PERIOD          ((SYSTEM_CLOCK_HZ / PWM_FREQUENCY_HZ) - 1)          // ARR value (Resolution)
    #define PWM_PRESCALER        ((SYSTEM_CLOCK_HZ / (PWM_FREQUENCY_HZ * PWM_PERIOD)) - 1)

    // Convert percentage to CCR value
    #define PWM_DUTY_CYCLE(percent) ((uint16_t)(((percent) * (PWM_PERIOD)) / 100))

    // Common duty cycle macros
    #define DEFAULT_PWM_DUTY_CYCLE    0

    /******************************** ADC Related Macros Define *****************************************/
    #define ADC1_DR_Address ((uint32_t)0x4001244C)  // Address of ADC1 data register

    #define ADC_GPIO_Port         GPIOA
    #define ADC_CLK              RCC_APB2Periph_GPIOA

    #define ADC_CH_COUNT    4
    #define ADC_SAMPLE_COUNT   (ADC_CH_COUNT * 25)  // Total buffer length for DMA (e.g., 16 samples per channel)
    #define DMA_ADC              1  // Set to 1 to enable DMA for ADC
    
    #define ADC_SAMPLE_FREQUENCY_HZ    1000    // Example: 1 kHz sampling rate

    #define ADC_SAMPLE_PERIOD_MS       (1000 / ADC_SAMPLE_FREQUENCY_HZ)  // Period in ms
    
    #define RX_BUFFER_SIZE 10  // Compile-time constant

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

#endif  // __setup_config_H
