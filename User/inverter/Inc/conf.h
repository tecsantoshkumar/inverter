#ifndef _CONF_H_
#define _CONF_H_


#include <stdbool.h>

#include "types.h"

// Compiler settings
// #define HOST

// ADC Averaage
#define NUM_ADC_CHANNELS 6                               // Number of ADC channels
#define SAMPLES_PER_CH 128
#define ADC_BUFF_LEN (NUM_ADC_CHANNELS * SAMPLES_PER_CH) // Number of samples

// System Settings
#define DEFAULT_ZERO_CROSS_TICK_LIMIT 4
#define SYSTEM_FREQUENCY FREQUENCY_50HZ
#define SYSTEM_CAPACITY 1000
#define ACTUAL_CAPACITY 600
#define DC_BUS DC_BUS_12V
#define BATTERY_VOLTAGE BATTERY_VOLTAGE_12V
#define DEFAULT_BATTERY_TYPE BATTERY_TYPE_LEAD_ACID
#define DEFAULT_INVERTER_TYPE INVERTER_TYPE_NARROW
#define DEFAULT_BATTERY_CHG_CURRENT BATTERY_CHG_CURRENT_15A
#define DEFAULT_BATTERY_LOW_CUT_LEVEL BATTERY_LOW_CUT_LEVEL_3
#define SOLAR_ENABLED false
#define BUZZER_ENABLE true
#define ATC_ENABLE false
#define DEFAULT_MAINS_AVAILABLE_DELAY (3000 / DEFAULT_STATUS_POLLING_INTERVAL) // in sec
#define DEFAULT_MAINS_W_CHANGEOVER_DELAY 20                                    // in milliseconds //should be greater than 11 always
#define DEFAULT_MAINS_N_CHANGEOVER_DELAY 12                                    // in milliseconds //should be greater than 11 always
#define DEFAULT_STATUS_POLLING_INTERVAL 100                                    // in milliseconds
#define DEFAULT_STATUS_12MS_POLLING_INTERVAL 500                                //for only MCB trip in milliseconds
#define DEFAULT_BUZZER_FREQUENCY 2000
#define DEFAULT_LCD_FREQUENCY 3000

// Battery Low Cut Level 1 - 12 V
#define BATTERY_12V_LOW_SHUTDOWN_LEVEL_1 0
#define BATTERY_12V_LOW_WARNING_LEVEL_1 0
#define BATTERY_12V_LOW_RECOVER_LEVEL_1 0

// Battery Low Cut Level 2 - 12 V
#define BATTERY_12V_LOW_SHUTDOWN_LEVEL_2 0
#define BATTERY_12V_LOW_WARNING_LEVEL_2 0
#define BATTERY_12V_LOW_RECOVER_LEVEL_2 0

// Battery Low Cut Level 3 - 12 V
#define BATTERY_12V_LOW_SHUTDOWN_LEVEL_3 1070
#define BATTERY_12V_LOW_WARNING_LEVEL_3 1100
#define BATTERY_12V_LOW_RECOVER_LEVEL_3 1800 // Doesn't need in normal inverter only with MPPT LB recovery is required

// Battery Low Cut Level 4 - 12 V
#define BATTERY_12V_LOW_SHUTDOWN_LEVEL_4 0
#define BATTERY_12V_LOW_WARNING_LEVEL_4 0
#define BATTERY_12V_LOW_RECOVER_LEVEL_4 0

// Battery Low Cut Level 5 - 12 V
#define BATTERY_12V_LOW_SHUTDOWN_LEVEL_5 0
#define BATTERY_12V_LOW_WARNING_LEVEL_5 0
#define BATTERY_12V_LOW_RECOVER_LEVEL_5 0

#define BATTERY_12V_HIGH_LIMIT 1570
#define BATTERY_12V_HIGH_CUT_RECOVERY 1450

// Mains Voltage Limits - Narrow
#define MAINS_NARROW_LOW 1025
#define MAINS_NARROW_LOW_RECOVERY 1065
#define MAINS_NARROW_HIGH 1282
#define MAINS_NARROW_HIGH_RECOVERY 1240

// Mains Voltage Limits - Wide
#define MAINS_WIDE_LOW 786            // For Narrow/UPS Mode at 1028 ADC = 185 VAC     	// for Wide/WUPS at 786 ADC = 86 VAC
#define MAINS_WIDE_LOW_RECOVERY 850   // For Narrow/UPS Mode at 1057 ADC = 195 VAC	 	// for Wide/WUPS at 850 ADC = 110 VAC
#define MAINS_WIDE_HIGH 1342          // For Narrow/UPS Mode at 1255 ADC = 265 VAC		// for Wide/WUPS at 1324 ADC = 290 VAC
#define MAINS_WIDE_HIGH_RECOVERY 1300 // For Narrow/UPS Mode at 1215 ADC = 255 VAC 		// for Wide/WUPS at 1299 ADC = 280 VAC

// Temperature Sensor
#define TEMP_AMBIENT_TYPICAL 25
#define TEMP_AMBIENT_MIN -20
#define TEMP_AMBIENT_MAX 80
#define TEMP_AMBIENT_SENSOR_OPEN 100
#define TEMP_AMBIENT_SENSOR_SHORT -100

// Battery Charging Voltage Limits
#define BATTERY_VOLTAGE_BOOST_LIMIT 1440
#define BATTERY_VOLTAGE_MAX_LIMIT 1460
#define BATTERY_VOLTAGE_FLOAT_LIMIT 1370
// #define BATTERY_VOLTAGE_FLOAT_MIN_LIMIT 1360

// Load Limits
#define OVERLOAD_WARNING_PERCENTAGE 70
#define OVERLOAD_SHUTDOWN_PERCENTAGE 90

#define OVERLOAD_WARNING_LIMIT 350  //((OVERLOAD_WARNING_PERCENTAGE * ACTUAL_CAPACITY) / 100)
#define OVERLOAD_SHUTDOWN_LIMIT 600 //((OVERLOAD_SHUTDOWN_PERCENTAGE * ACTUAL_CAPACITY) / 100)

#define OVERLOAD_RETRY_LIMIT_1 110
#define OVERLOAD_RETRY_LIMIT_2 125
#define OVERLOAD_RETRY_LIMIT_3 150
#define OVERLOAD_RETRY_LIMIT_4 200

// Temperture Limits
#define TEMPERATURE_FAN_ENABLE 1600
#define TEMPERATURE_HIGH_CUT 1000
#define TEMPERATURE_HIGH_RECOVERY 690





#define FAN_ON()   HAL_GPIO_WritePin(FAN_DRIVE_GPIO_Port, FAN_DRIVE_Pin, GPIO_PIN_RESET)
#define FAN_OFF()  HAL_GPIO_WritePin(FAN_DRIVE_GPIO_Port, FAN_DRIVE_Pin, GPIO_PIN_SET)

#define MOVING_AVERAGE_WINDOW   128U   // because you were doing >>7
#define ADC_CH_OUTPUT_INDEX     4U     // ch 4 = output voltage

#define ADC_MAINS_OK_MIN_THR    65
#define ADC_MAINS_OK_MAX_THR    2700
#define ADC_MAINS_LOW_THR 		120
#define ADC_MAINS_CUT_THR       170

// Mains sense ~0 for MCB detection (ADC on mains channel)
#define ADC_MAINS_MCB_THR          20U     // mainsAvg < 20 �� "0V after MCB"

// Zero-cross "missing" threshold in ADC blocks
// If no ZC tick change for >7 blocks (~>10 ms), we consider ZC dead
#define ZC_MISSING_MAX_BLOCKS      7U

// How long "mains��0 & ZC alive" must persist to declare MCB trip
#define MCB_TRIP_ENTER_BLOCKS      7U     // > ZC_MISSING_MAX_BLOCKS

// Optional: how long normal condition must persist to auto-clear MCB trip
#define MCB_TRIP_EXIT_BLOCKS       20U

#define ADC_MAINS_CUT_THR_ENTER   (ADC_MAINS_CUT_THR)			// Macro Mains Low Cut Recovery in UPS/Narrow Mode
#define ADC_MAINS_CUT_THR_EXIT    (ADC_MAINS_CUT_THR + 23)		// Macro for Mains Low Cut in UPS/Narrow Mode
#define WUPS_ADC_MAINS_CUT_THR_EXIT	  (135U)					// Macro for Mains Low Cut Recovery in WUPS Mode
#define WUPS_ADC_MAINS_CUT_THR_ENTER  (130U)					// Macro for Mains Low Cut in WUPS Mode

#define MAINS_CUT_ENTER_BLOCKS    (2U)   // 2 blocks �� ~3 ms worst-case
#define MAINS_CUT_EXIT_BLOCKS     (12U)   // ~9 ms, not on fast path
#define WUPS_MAINS_CUT_ENTER_BLOCKS   (3U) 		//10U = 22ms in changeover time in WUPS Mode
#define WUPS_MAINS_CUT_EXIT_BLOCKS     (6U)   // ~9 ms, not on fast path

#define ADC_MAINS_HIGH_THR_ENTER  (2700U)

#define ADC_MAINS_HIGH_THR_EXIT   (2650U)  		// Macro for Mains High Cut Recovery in UPS/Narrow Mode
#define WUPS_ADC_MAINS_HIGH_THR_EXIT   (2800U)  // Macro for Mains High Cut Recovery in WUPS Mode

#define MAINS_HIGH_ENTER_BLOCKS   (2U)    // slower, non-critical
#define MAINS_HIGH_EXIT_BLOCKS    (12U)   // need it stable before clearing


#define MIN_MAINS_HEALTH_CHECK_COUNTER (40U)

#define ADC_RECOVERY_DEBOUNCING_COUNT   16

#define MAINS_DEBOUNCE_BLOCKS  (2U)
#define MAINS_CUT_DEBOUNCE_BLOCKS	(2U)

#define OUT_VOLT_RMS_BUF_SIZE   (40U)   // 40 samples �� 40 ms at 1 kHz (~2 cycles)

#define OUT_CAL_VOLTAGE      (214.0f)     // your known RMS volts
#define OUT_CAL_COUNTS       (350.0f)     // replace with your measured C214

#define OUT_VOLT_SCALE       (OUT_CAL_VOLTAGE / OUT_CAL_COUNTS)
#define OUT_VOLT_OFFSET      (0.0f)       // keep 0 unless you see a fixed offset error



#define OUT_CAL_EQ_A   (0.711f * OUT_VOLT_SCALE)
#define OUT_CAL_EQ_B   (0.711f * OUT_VOLT_OFFSET + 10.51f)
#define OUT_VOLT_PEAK_BUF_SIZE       (100U)  // get ADC sample
#define OUT_VOLT_SAMPLING_TIMER      (10U)  //  get ADC sample timer



//////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#define ADC_CHANNEL_COUNT 4
#define ADC_INDEX_BATTERY_VOLTAGE 0
#define ADC_INDEX_SOLAR_CURRENT 1
#define ADC_INDEX_BATTERY_CURRENT 1
#define ADC_INDEX_SOLAR_VOLTAGE 2
#define ADC_INDEX_TEMPERATURE 3
#define ADC_REFERENCE_VOLTAGE 3.3f
#define ADC_MOVING_AVERAGE_SIZE 64
#define ADC_DMA_SAMPLES       1024   /* Must be multiple of 4 */

#define USE_BATTERY_CURRENT_SENSE_IN_MEASUREMENTS 1

#define TASK_MEASURE_PERIOD_MS 100
#define TASK_MODULATE_PERIOD_MS 500
#define TASK_MODULATE_CHECK_PERIOD_MS 2000
#define TASK_MODULATE_FAULT_PERIOD_MS 10000


#define MPPT_SOLAR_CURRENT_MIN 0.0f
#define MPPT_SOLAR_CURRENT_MAX 20.0f
#define MPPT_SOLAR_POWER_DELTA 0.0f
#define MPPT_SOLAR_VOLTAGE_MIN 10.0f

#define MPPT_OUTPUT_CURRENT_MAX 50.0f
#define MPPT_INPUT_VOLTAGE_MAX 51.0f
#define MPPT_INPUT_CURRENT_MAX 20.0f

#define MPPT_BATTERY_VOLTAGE_MIN 10.0f
#define MPPT_BATTERY_VOLTAGE_MAX 14.6f
#define MPPT_BATTERY_CURRENT_MAX 50.0f

#endif
