#include "ch32v10x_conf.h"
#include <stdbool.h>
#include <string.h>
#include "hardware.h"
#include "conf.h"
#include "debug.h"

#define ADC_DMA_SAMPLES 1024
uint16_t ADC_Buffer[ADC_DMA_SAMPLES] = {0};

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

static char uart_tx_buffer[80] = {0};
s16 Calibrattion_Val = 0;
  uint16_t idx;

volatile  uint16_t sys_ms = 0;
FlagStatus SecFlag = RESET;

static uint8_t uart_rx_temp_byte;


volatile uint8_t RxBuffer[50] = {0};    // Fixed size array
volatile uint16_t RxIndex = 0;


// void wyre_gpio_set_state(MPPT_GPIO_Pin_t pin, bool state)
// {
//     switch (pin)
//     {
//     case MPPT_GPIO_BCCU_ENABLE:
//         GPIO_WriteBit(GPIOB, BCCU_Pin, state);
//         break;

//     case MPPT_GPIO_SHUTDOWN:
//         GPIO_WriteBit(GPIOB, MPPT_SHUTDOWN_Pin, state);
//         break;

//     case MPPT_GPIO_FAN_DRIVE:
//         GPIO_WriteBit(GPIOB, FAN_DRIVE_Pin, state);
//         break;

//     case MPPT_GPIO_REVERSE_PROTECTION:
//         GPIO_WriteBit(GPIOB, RE_PV_Pin, state);
//         break;

//     default:
//         break;
//     }
// }

void wyre_hardware_init(void)
{   
    /*-----------------------------------------------------------
     * System Clock Configuration
     *-----------------------------------------------------------*/
    // Set system clock source to PLL and run at 80 MHz for stable operation
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    
    /*-----------------------------------------------------------
     * UART1 GPIO Configuration
     *-----------------------------------------------------------*/
    wyre_uart_init();

    printf("System Clock set to %lu Hz\n", SystemCoreClock);
    
    /*-----------------------------------------------------------
     * GPIO Configuration
     *-----------------------------------------------------------*/
    wyre_gpio_init();
    
    /*-----------------------------------------------------------
     * System Tick Timer Initialization
     *-----------------------------------------------------------*/
    wyre_SysTick_Init();

    /*-----------------------------------------------------------
     * Peripheral system timer Initialization
     *-----------------------------------------------------------*/
    wyre_timer_init();
    
    /*-----------------------------------------------------------
     * Peripheral ADC Initialization
     *-----------------------------------------------------------*/
    wyre_adc_init();

    printf("Return to main loop\n");
}

/*******************************************************************************
* @Function Name : wyre_gpio_init
* @Description   : Initialize GPIOB Pin 0 and Pin 1 as output (for LEDs)
* @Inputs        : None
* @Outputs       : None
* @Return Value  : None
*******************************************************************************/
void wyre_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    printf("GPIO Init done\n");
}
/*******************************************************************************
* @Function Name : wyre_timer_init
* @Description   : Initialize Timer1 to generate PWM on PA8
* @Inputs        : None
* @Outputs       : None
* @Return Value  : None
*******************************************************************************/
void wyre_timer_init(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure        = {0};
    TIM_OCInitTypeDef       TIM_OCInitStructure       = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    // Enable clocks for GPIOA and TIM1 peripheral
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);

    // Configure GPIOA Pin 8 (PA8) as Alternate Function Push-Pull (for TIM1_CH1 output)
    GPIO_InitStructure.GPIO_Pin  = MPPT_PWM_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure Timer Base parameters
    TIM_TimeBaseInitStructure.TIM_Period        = PWM_PERIOD;      // Auto-reload value (PWM resolution)
    TIM_TimeBaseInitStructure.TIM_Prescaler     = PWM_PRESCALER;   // Prescaler for desired frequency
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    // No clock division
    TIM_TimeBaseInitStructure.TIM_CounterMode    = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    // Configure PWM Output Mode
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;       // PWM Mode 1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = PWM_DUTY_CYCLE(DEFAULT_PWM_DUTY_CYCLE);  // Set default duty cycle
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    // Enable main PWM output (required for TIM1)
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    // Enable Preload for PWM duty cycle register (CCR1)
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // Enable Auto-Reload Preload (ARR register buffered)
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    // Start Timer
    TIM_Cmd(TIM1, ENABLE);
    
    printf("PWM initialized @ 39 kHz (80 MHz clock), PA8\n");
}

/*******************************************************************************
* @Function Name : PWM_SetDuty
* @Description   : Set duty cycle for PWM (0-100%)
* @Inputs        : duty_value - percentage (0–100)
* @Outputs       : None
* @Return Value  : applied duty %
*******************************************************************************/
uint8_t wyre_pwm_set_duty_cycle(uint8_t duty_value)
{
    if ((duty_value > 100 )|| (duty_value < 0)) {duty_value = 0;}

    uint32_t compare_val = PWM_DUTY_CYCLE(duty_value);
    
    TIM_SetCompare1(TIM1, compare_val);
    return duty_value;
}


/*******************************************************************************
 * Function Name  : wyre_adc_init
 * Description    : Initialize ADC1 in DMA mode for 4 channels (PA0-PA3)
 * Input          : None
 * Output         : None
 * Return         : None
 * *******************************************************************************/

void DMA_ADC_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit(DMA1_Channel1);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->RDATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 1024;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   /* Can be Circular if needed */
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void wyre_adc_init(void)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* ADC clock <= 14 MHz */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    /* PA0–PA3 as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;          /* IMPORTANT */
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   /* REQUIRED */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNEL_COUNT;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    /* Calibration */
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    Calibrattion_Val = Get_CalibrationValue(ADC1);

    /* Channel rank order */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5);

    DMA_ADC_Init();
}

/*******************************************************************************
* @Function Name : wyre_adc_update_moving_average
* @Description   : Update moving average for ADC channel
* @Inputs        : channel - ADC channel index
*                 new_value - new ADC reading
* @Outputs       : None
* @Return Value  : None
*******************************************************************************/
static void wyre_adc_update_moving_average(uint8_t channel, uint16_t new_value)
{
    // Subtract the old value from the sum
    // mppt.adc.sum[channel] -= mppt.adc.moving_avg_buffer[channel][mppt.adc.buffer_index[channel]];
    // // Add the new value to the buffer and sum
    // mppt.adc.moving_avg_buffer[channel][mppt.adc.buffer_index[channel]] = new_value;
    // mppt.adc.sum[channel] +=  new_value;
    // // Update the buffer index
    // mppt.adc.buffer_index[channel] = (mppt.adc.buffer_index[channel] + 1) % ADC_MOVING_AVERAGE_SIZE;
    // // Calculate the new averagewyre_adc_read_all
    
    // mppt.adc.averaged_values[channel] = mppt.adc.sum[channel] / ADC_MOVING_AVERAGE_SIZE;
    // mppt.adc.sum[0] = 0;
    // for(uint8_t i = 0; i < 64; i++)
    // {
    //     mppt.adc.sum[0] +=  wyre_adc_read_channel(channel);
    // }

    // // mppt.adc.averaged_values[0] = mppt.adc.sum[0] / ADC_MOVING_AVERAGE_SIZE;
    // mppt.adc.averaged_values[0] = mppt.adc.sum[0] / ADC_MOVING_AVERAGE_SIZE;
}

/*******************************************************************************
* @Function Name : SysTick_Handler
* @Description   : SysTick interrupt handler for 1 ms system tick
* @Inputs        : None
* @Outputs       : None
* @Return Value  : None
*******************************************************************************/
void SysTick_Handler(void)
{
    sys_ms++;
    /* 10 ms task */
    if ((sys_ms % 10U) == 0U)
    {
        for (uint8_t channel = 0; channel < ADC_CHANNEL_COUNT; channel++)
        {
            wyre_adc_update_moving_average(channel,ADC_Buffer[1020 + channel]);
        }
    }

    /* 1 second task */
    if (sys_ms >= 1000U)
    {
        sys_ms = 0;
        SecFlag = SET;
    }

    /* Clear SysTick interrupt (CH32 requirement) */
    SysTick->CNTL0 = 0;
    SysTick->CNTL1 = 0;
    SysTick->CNTL2 = 0;
    SysTick->CNTL3 = 0;

    SysTick->CNTH0 = 0;
    SysTick->CNTH1 = 0;
    SysTick->CNTH2 = 0;
    SysTick->CNTH3 = 0;
}

uint32_t wyre_get_tick(void)
{
   return sys_ms;
}

/*******************************************************************************
* @Function Name : wyre_SysTick_Init
* @Description   : Initialize SysTick timer for 1 ms interrupts
* @Inputs        : None
* @Outputs       : None
* @Return Value  : None
*******************************************************************************/
void wyre_SysTick_Init(void)
{
    uint64_t ticks;

    /* SysTick clock = HCLK / 8 */
     ticks = (uint64_t)(SystemCoreClock / 8 / 1000) - 1;


    /* Disable SysTick */
    SysTick->CTLR = 0x0000;

    /* Clear counter */
    SysTick->CNTL0 = 0;
    SysTick->CNTL1 = 0;
    SysTick->CNTL2 = 0;
    SysTick->CNTL3 = 0;

    SysTick->CNTH0 = 0;
    SysTick->CNTH1 = 0;
    SysTick->CNTH2 = 0;
    SysTick->CNTH3 = 0;

    /* Load compare value (64-bit) */
    SysTick->CMPLR0 = (uint8_t)(ticks & 0xFF);
    SysTick->CMPLR1 = (uint8_t)(ticks >> 8);
    SysTick->CMPLR2 = (uint8_t)(ticks >> 16);
    SysTick->CMPLR3 = (uint8_t)(ticks >> 24);

    SysTick->CMPHR0 = (uint8_t)(ticks >> 32);
    SysTick->CMPHR1 = (uint8_t)(ticks >> 40);
    SysTick->CMPHR2 = (uint8_t)(ticks >> 48);
    SysTick->CMPHR3 = (uint8_t)(ticks >> 56);

    /* Configure NVIC */
    NVIC_SetPriority(SysTicK_IRQn, 15);
    NVIC_EnableIRQ(SysTicK_IRQn);

    /* Enable SysTick */
    SysTick->CTLR = (1 << 0);   /* Enable counter + interrupt */
}


uint32_t millis(void) { return sys_ms; }

void delay_ms(uint32_t ms)
{
    uint32_t start = millis();
    while ((millis() - start) < ms) { /* optionally __WFI() */ }
}


void wyre_uart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable clocks for USART1 and GPIOA */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* Configure USART1 TX (PA9) as Alternate Function Push-Pull */
    GPIO_InitStructure.GPIO_Pin = UART_TX_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 RX (PA10) as Floating Input */
    GPIO_InitStructure.GPIO_Pin = UART_RX_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 parameters */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    /* Enable RX interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /* Configure NVIC for USART1 interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART1 peripheral */
    USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
* @Function Name : wyre_uart_transmit_it
* @Description   : Transmits data over USART1.
* @Input         : data - Data buffer to transmit.
*                  length - Number of bytes to transmit.
* @Output        : None
* @Return        : None
*******************************************************************************/
void wyre_uart_transmit_it(uint8_t *data, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, data[i]);
    }
}

/*******************************************************************************
* @Function Name : wyre_uart_receive_it
* @Description   : Receives data over USART1.
* @Input         : buffer - Buffer to store received data.
*                  length - Number of bytes to receive.
* @Output        : None
* @Return        : None
*******************************************************************************/
char wyre_uart_receive_it(uint8_t *data, uint16_t length)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return (char)USART_ReceiveData(USART1);
}

/*********************************************************************************
 * @Function Name  : USART1_IRQHandler
 * @Description    : This function handles USART1 interrupt request.
 * @Input          : None
 * @Output         : None
 * @Return         : None
 *********************************************************************************/
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t received = USART_ReceiveData(USART1) & 0xFF;

        /* Store received data into buffer */
        if (RxIndex < RX_BUFFER_SIZE)
        {
            RxBuffer[RxIndex++] = received;
        }

        /* Clear the RXNE interrupt flag */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
