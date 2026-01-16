#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>


typedef enum
{
  GPIO_PIN_RESET = 0U,
  GPIO_PIN_SET
} GPIO_PinState;
/* General */

typedef struct
{
    uint16_t voltage;
    uint16_t current;
    uint16_t frequency;
} AC_t;

/* Device */

typedef enum
{
    BATTERY_TYPE_LEAD_ACID,
    BATTERY_TYPE_SMF,
    BATTERY_TYPE_TUBULAR,
    BATTERY_TYPE_LITHIUM_ION
} BatteryType_t;

typedef enum
{
    BATTERY_LOW_CUT_LEVEL_1, /* 10.5 V */
    BATTERY_LOW_CUT_LEVEL_2, /* 10.8 V */
    BATTERY_LOW_CUT_LEVEL_3, /* 11.0 V */
    BATTERY_LOW_CUT_LEVEL_4, /* 11.2 V */
    BATTERY_LOW_CUT_LEVEL_5, /* 11.4 V */
} BatteryLowCut_t;

typedef enum
{
    FREQUENCY_50HZ = 50,
    FREQUENCY_60HZ = 60
} Frequency_t;

typedef enum
{
    DC_BUS_12V = 12,
    DC_BUS_24V = 24,
    DC_BUS_48V = 48,
    DC_BUS_72V = 72,
    DC_BUS_96V = 96
} DCBus_t;

typedef enum
{
    INVERTER_TYPE_NARROW,
    INVERTER_TYPE_WIDE
} InverterType_t;

typedef enum
{
    CHARGING_STATE_IDLE,
    CHARGING_STATE_BOOST,
    CHARGING_STATE_ABSORPTION,
    CHARGING_STATE_FLOAT
} ChargingState_t;

typedef enum
{
    MAINS_STATE_CUT = 0,
    MAINS_STATE_OK
} MainsState_t;

typedef enum
{
    BATTERY_CHG_CURRENT_1A = 1600,
    BATTERY_CHG_CURRENT_2A = 1600,
    BATTERY_CHG_CURRENT_5A = 1600,
    BATTERY_CHG_CURRENT_10A = 830,
    BATTERY_CHG_CURRENT_15A = 1360,
    BATTERY_CHG_CURRENT_20A = 1770,
    BATTERY_CHG_CURRENT_25A = 1600
} BatteryChgCurrent_t;

/* App */

typedef struct
{
    uint8_t sig;
} GenericEvent_t;

typedef struct
{
    GenericEvent_t super;
} UserEvent_t;

typedef enum
{
    IDLE,
    MAINS_MODE,
    BACKUP_MODE,
    FAULT_DETECT
    // MAINS_MODE_OKAY_CHARGING,
    // MAINS_MODE_LOW,
    // MAINS_MODE_HIGH,
    // MAINS_MODE_ZERO_CROSS_HANDLE,
    // MAINS_FUSE_BLOWN,
    // MAINS_OVER_CURRENT_WARNING,
    // MAINS_OVER_CURRENT_FAULT,
    // BACKUP_MODE_TURN_ON,
    // BACKUP_MODE_BATTERY_LOW,
    // BACKUP_MODE_BATTERY_LOW_SHUTDOWN,
    // BACKUP_MODE_BATTERY_HIGH,
    // BACKUP_MODE_OVERLOAD_WARNING,
    // BACKUP_MODE_OVERLOAD_FAULT,
    // BACKUP_MODE_SHORT_CIRCUIT_WARNING,
    // BACKUP_MODE_SHORT_CIRCUIT
} State_t;

typedef enum
{
    SHORT_TICK = 10,
    LONG_TICK,
    SHORT_CIRCUIT,
    ZERO_CROSS_EDGE,
    UPS_POWERED_ON,
    UPS_POWERED_OFF,

    /* Internal */
    ENTRY,
    EXIT,
} Signal_t;

typedef enum
{
    EVENT_HANDLED,
    EVENT_IGNORED,
    EVENT_TRANSITION
} EventStatus;

typedef struct
{
    bool volatile mains_available;
    bool volatile zero_cross_available;
    bool front_switch;
    bool charging_status;
    uint8_t buzzer_en;
    uint16_t temperature;
} InverterStatus;

typedef struct
{
    bool Batt_Low_Cut;
    bool Batt_High_Cut;
    // bool Overload_warning;
    bool Overload_shutdown;
    bool Short_circuit_detect;
    bool Short_circuit_shutdown;
    bool MCB_trip_shutdown;
    bool soft_start;
    bool High_temp;
} BackupFaultStatus;

typedef enum
{
	NONE,
	MAINS_STATE_HIGH,
	MAINS_STATE_LOW,
	MAINS_MCB_TRIP
} MainsFaultStatus;

typedef struct
{
    uint16_t volatile polling_tick;
    uint16_t volatile polling_tick_1ms;
    uint16_t overload_tick;
    uint16_t short_circuit_tick;
    uint16_t mains_ok_tick;
    uint16_t mains_fault_tick;
    uint16_t volatile zero_cross_tick;
    uint8_t overload_retry_tick;
    uint8_t short_circuit_retry_tick;
    uint16_t charge_tick;
    uint32_t last_buzz_update;
    uint32_t last_lcd_update;
} TickerStatus;

typedef struct
{
    // bool Front_switch;
    AC_t mains;
    AC_t battery;
    AC_t load;
    AC_t output;
    AC_t solar;
    DCBus_t dc_bus;
    ChargingState_t charging_state;
    MainsState_t mains_state;
    BatteryChgCurrent_t bcc;
    BatteryType_t battery_type;
    InverterType_t inverter_type;
    BatteryLowCut_t battery_low_cut_level;
    State_t active_state;
    BackupFaultStatus backup_fault_status;
    MainsFaultStatus mains_fault_status;
    TickerStatus ticker_status;
    InverterStatus inverter_status;
    uint16_t short_circuit_detect_counter;
    uint16_t adc_mains_cut_thr_enter;
    uint16_t adc_mains_cut_thr_exit;
    uint16_t mains_cut_enter_blocks;
    uint16_t mains_cut_exit_block;
    uint16_t mains_high_threshold;
    uint16_t adc_mains_high_thr_exit;
    bool pwm_start;
    uint8_t debug_variable;
    uint16_t WUPS_ADC_MAINS_CUT_THR_ENTER;
    uint16_t NUPS_ADC_MAINS_CUT_THR_ENTER;
    uint16_t BATTERY_VOLTAGE_LOW_SHUTDOWN;
    uint16_t BATTERY_VOLTAGE_LOW_WARNING;
    uint16_t BATTERY_VOLTAGE_LOW_RECOVERY;
    uint16_t BATTERY_VOLTAGE_HIGH_CUT;
    uint16_t BATTERY_VOLTAGE_HIGH_CUT_RECOVERY;
    uint16_t MAINS_VOLTAGE_LOW_CUT;
    uint16_t MAINS_VOLTAGE_HIGH_CUT;
    uint16_t MAINS_VOLTAGE_LOW_RECOVERY;
    uint16_t MAINS_VOLTAGE_HIGH_RECOVERY;
    uint16_t MAINS_CHANGEOVER_DELAY;
    uint16_t BUZZER_FREQUENCY;
    uint8_t LCD_SCREEN;
} Inverter;

#endif
