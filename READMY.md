
# 📋 Embedded Firmware Project - ADC, PWM, UART, GPIO

## ✅ Project Overview
This firmware demonstrates the integration of ADC, PWM, UART, and GPIO functionality on the CH32V10x microcontroller.

The application continuously reads analog data from 4 ADC channels, drives PWM output on a single channel, communicates debug information via UART, and toggles onboard LEDs.

---

## ⚡️ Hardware Connections

| Peripheral | Pin Configuration |
|------------|-------------------|
| **ADC**    | GPIOA Pin 0 (CH0), Pin 1 (CH1), Pin 2 (CH2), Pin 3 (CH3) |
| **PWM**    | GPIOA Pin 8 (TIM1_CH1 Output) |
| **UART**   | GPIOA Pin 9 (TX), GPIOA Pin 10 (RX) |
| **LEDs**   | GPIOB Pin 0, Pin 1 (Onboard LEDs) |

---

## 🚀 Features

- ✅ ADC with DMA mode: Continuously sample 4 channels.
- ✅ PWM Output: Fixed frequency at 39 KHz, configurable duty cycle via macro.
- ✅ UART Communication: Print system info and ADC values every second.
- ✅ GPIO Control: Toggle LED states every second.

---

## ⚙️ Macros & Configuration

```c
#define PWM_FREQUENCY_HZ    39000       // PWM frequency in Hz
#define PWM_PERIOD          1640        // Auto-reload register (ARR)
#define PWM_PRESCALER       1           // Timer prescaler
#define PWM_REPORT_DUTY     (PWM_PERIOD / 4)   // 25% duty cycle
```

---

## ⚡️ Sample Output on UART

```
System Clock: 72000000 Hz
PWM Frequency: 39000 Hz, Period: 1640, Prescaler: 1
PWM Duty: 25%
ADC Values: CH0=1234, CH1=1356, CH2=1400, CH3=1289
ADC Values: CH0=1245, CH1=1361, CH2=1398, CH3=1295
...
```


## ⚡️ Notes

- Ensure GPIOA-0 to GPIOA-3 are connected to analog sensors.
- GPIOA-8 outputs PWM signal.
- PA9 (TX) and PA10 (RX) are connected to UART for debug.
- PB0 and PB1 are connected to LEDs.

---

## 📄 License

```
(C) COPYRIGHT 2025 TEC.
Attention: This software (modified or not) and binary are only used for TEC.
```