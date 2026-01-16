#include "conf.h"
#include "device.h"
#include "hardware.h"

volatile Inverter dev;

void device_init(Inverter *d)
{
    d->charging_state = CHARGING_STATE_IDLE;
    d->bcc = DEFAULT_BATTERY_CHG_CURRENT;
    d->battery_type = DEFAULT_BATTERY_TYPE;
    d->inverter_type = DEFAULT_INVERTER_TYPE;
    d->dc_bus = DC_BUS;
    d->battery_low_cut_level = DEFAULT_BATTERY_LOW_CUT_LEVEL;
    d->LCD_SCREEN = 0;
    d->adc_mains_cut_thr_exit = WUPS_ADC_MAINS_CUT_THR_EXIT;					// Mains Low Cut in UPS/Narrow Mode
    d->adc_mains_cut_thr_enter = WUPS_ADC_MAINS_CUT_THR_ENTER;					// Mains Low Cut Recovery in UPS/Narrow Mode
    d->mains_cut_enter_blocks = WUPS_MAINS_CUT_ENTER_BLOCKS;					// Changeover from Mains to Backup time in UPS/Narrow Mode
    d->mains_cut_exit_block = WUPS_MAINS_CUT_EXIT_BLOCKS;						// Changeover from Backup to Mains time in UPS/Narrow Mode
    d->mains_high_threshold = 610;											// Mains High Cut in UPS/Narrow Mode
    d->adc_mains_high_thr_exit =  WUPS_ADC_MAINS_HIGH_THR_EXIT;					// Mains High Cut Recovery in UPS/Narrow Mode

}

void device_hardware_sync(Inverter *d)
{
    switch (d->dc_bus)
    {
    case DC_BUS_12V:

        switch (d->battery_low_cut_level)
        {
        case BATTERY_LOW_CUT_LEVEL_1: /* 10.5 V */
            d->BATTERY_VOLTAGE_LOW_SHUTDOWN = BATTERY_12V_LOW_SHUTDOWN_LEVEL_1;
            d->BATTERY_VOLTAGE_LOW_WARNING = BATTERY_12V_LOW_WARNING_LEVEL_1;
            d->BATTERY_VOLTAGE_LOW_RECOVERY = BATTERY_12V_LOW_RECOVER_LEVEL_1;
            d->BATTERY_VOLTAGE_HIGH_CUT = BATTERY_12V_HIGH_LIMIT;
            d->BATTERY_VOLTAGE_HIGH_CUT_RECOVERY = BATTERY_12V_HIGH_CUT_RECOVERY;
            break;

        case BATTERY_LOW_CUT_LEVEL_2: /* 10.8 V */
            // Set the battery low shutdown, warning, and recovery voltages for BATTERY_LOW_CUT_LEVEL_2
            d->BATTERY_VOLTAGE_LOW_SHUTDOWN = BATTERY_12V_LOW_SHUTDOWN_LEVEL_2;
            d->BATTERY_VOLTAGE_LOW_WARNING = BATTERY_12V_LOW_WARNING_LEVEL_2;
            d->BATTERY_VOLTAGE_LOW_RECOVERY = BATTERY_12V_LOW_RECOVER_LEVEL_2;
            d->BATTERY_VOLTAGE_HIGH_CUT = BATTERY_12V_HIGH_LIMIT;
            d->BATTERY_VOLTAGE_HIGH_CUT_RECOVERY = BATTERY_12V_HIGH_CUT_RECOVERY;
            break;

        case BATTERY_LOW_CUT_LEVEL_3: /* 11.0 V */
            // Set the battery low shutdown, warning, and recovery voltages for BATTERY_LOW_CUT_LEVEL_3
            d->BATTERY_VOLTAGE_LOW_SHUTDOWN = BATTERY_12V_LOW_SHUTDOWN_LEVEL_3;
            d->BATTERY_VOLTAGE_LOW_WARNING = BATTERY_12V_LOW_WARNING_LEVEL_3;
            d->BATTERY_VOLTAGE_LOW_RECOVERY = BATTERY_12V_LOW_RECOVER_LEVEL_3;
            d->BATTERY_VOLTAGE_HIGH_CUT = BATTERY_12V_HIGH_LIMIT;
            d->BATTERY_VOLTAGE_HIGH_CUT_RECOVERY = BATTERY_12V_HIGH_CUT_RECOVERY;
            break;

        case BATTERY_LOW_CUT_LEVEL_4: /* 11.2 V */
            // Set the battery low shutdown, warning, and recovery voltages for BATTERY_LOW_CUT_LEVEL_4
            d->BATTERY_VOLTAGE_LOW_SHUTDOWN = BATTERY_12V_LOW_SHUTDOWN_LEVEL_4;
            d->BATTERY_VOLTAGE_LOW_WARNING = BATTERY_12V_LOW_WARNING_LEVEL_4;
            d->BATTERY_VOLTAGE_LOW_RECOVERY = BATTERY_12V_LOW_RECOVER_LEVEL_4;
            d->BATTERY_VOLTAGE_HIGH_CUT = BATTERY_12V_HIGH_LIMIT;
            d->BATTERY_VOLTAGE_HIGH_CUT_RECOVERY = BATTERY_12V_HIGH_CUT_RECOVERY;
            break;

        case BATTERY_LOW_CUT_LEVEL_5: /* 11.4 V */
            // Set the battery low shutdown, warning, and recovery voltages for BATTERY_LOW_CUT_LEVEL_5
            d->BATTERY_VOLTAGE_LOW_SHUTDOWN = BATTERY_12V_LOW_SHUTDOWN_LEVEL_5;
            d->BATTERY_VOLTAGE_LOW_WARNING = BATTERY_12V_LOW_WARNING_LEVEL_5;
            d->BATTERY_VOLTAGE_LOW_RECOVERY = BATTERY_12V_LOW_RECOVER_LEVEL_5;
            d->BATTERY_VOLTAGE_HIGH_CUT = BATTERY_12V_HIGH_LIMIT;
            d->BATTERY_VOLTAGE_HIGH_CUT_RECOVERY = BATTERY_12V_HIGH_CUT_RECOVERY;
            break;
        }

        break;

    case DC_BUS_24V:
        break;

    case DC_BUS_48V:
        break;

    case DC_BUS_72V:
        break;

    case DC_BUS_96V:
        break;
    }

    switch (d->inverter_type)
    {
    case INVERTER_TYPE_NARROW:
        d->MAINS_VOLTAGE_LOW_CUT = MAINS_NARROW_LOW;
        d->MAINS_VOLTAGE_HIGH_CUT = MAINS_NARROW_HIGH;
        d->MAINS_VOLTAGE_LOW_RECOVERY = MAINS_NARROW_LOW_RECOVERY;
        d->MAINS_VOLTAGE_HIGH_RECOVERY = MAINS_NARROW_HIGH_RECOVERY;
        d->MAINS_CHANGEOVER_DELAY = DEFAULT_MAINS_N_CHANGEOVER_DELAY;
        break;

    case INVERTER_TYPE_WIDE:
        // Set the mains low, high, low recovery, and high recovery voltages for INVERTER_TYPE_WIDE
        d->MAINS_VOLTAGE_LOW_CUT = MAINS_WIDE_LOW;
        d->MAINS_VOLTAGE_HIGH_CUT = MAINS_WIDE_HIGH;
        d->MAINS_VOLTAGE_LOW_RECOVERY = MAINS_WIDE_LOW_RECOVERY;
        d->MAINS_VOLTAGE_HIGH_RECOVERY = MAINS_WIDE_HIGH_RECOVERY;
        d->MAINS_CHANGEOVER_DELAY = DEFAULT_MAINS_W_CHANGEOVER_DELAY;
        break;
    }
}
