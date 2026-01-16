#include <stdio.h>

#include "hardware.h"
#include "conf.h"
#include "device.h"
#include "types.h"

#define OUT_BLOCKS_FOR_AVG (32U)

/* vars */
extern Inverter dev;
extern int16_t adc_avg[NUM_ADC_CHANNELS], cnt2;
extern volatile uint16_t moving_averages[];
extern uint8_t  fan_boost_active;
extern uint32_t fan_boost_end_ms;

// extern FaultStatus fault_status;
// int volatile tick_zero_cross = 0;
// bool volatile zero_cross_active;
extern bool get_out_voltage;
extern float out_volt;

volatile static uint8_t mains_check_debounce_counter ;
uint8_t g_mains_adc_avg_val;
static uint8_t mains_adc_avg_val = 0;
volatile bool changeover_occured = false;
static uint32_t out_sum_counts = 0;
static uint16_t out_sum_blocks = 0;
static uint16_t out_avg_counts = 0;
volatile uint16_t battery_dsg_va;
int32_t charger_duty = 0;
uint16_t Float_counter = 0;
volatile uint16_t load_percent;
int allowed_time = 0;


float DEFAULT_I_Kp = 0.01f, DEFAULT_V_Kp = 0.25f;
static float out_display = 0.0f;

void Inputs_UpdateFromADC(void)
{
    // dev.mains.voltage                = adc_avg[0]; // We are not using it anywhere, can it be removed ?
    // dev.battery.voltage = (BAT_VOLT_SCALE * adc_avg[1] + BAT_VOLT_OFFSET) * BAT_VOLT_MULTIPLIER;
    dev.battery.current = adc_avg[2];
    dev.load.current = adc_avg[3];
    dev.inverter_status.temperature = adc_avg[5];
//    dev.inverter_status.zero_cross_available = ZC_IsAlive(&dev);
/*    cnt++;
    if(cnt >= 12)
    {
    	cnt = 0;
    }*/
    /*-------------------- Get actual out voltage --------------------
  //  Get_OutValtageFromADC((uint16_t)adc_avg[4]);
    if(get_out_voltage)
    {
        const float alpha = 0.2f;  // 0<alpha<=1 ; smaller = smoother
        out_display = out_display + alpha * (out_volt - out_display);
        dev.output.voltage = out_display;
        dev.output.voltage = out_volt;
        get_out_voltage    = false;
    }*/

    /*  static uint32_t out_filt = 0U;  // filtered value in same units as adc_avg[4]
      uint32_t raw_out = adc_avg[4];

      if (out_filt == 0U)
      {
          // First run: initialize to current value to avoid long ramp-up
          out_filt = raw_out;
      }
      else
      {
          // Exponential moving average:
          // out_filt = out_filt + (raw_out - out_filt) / 2^OUT_VOLT_FILTER_SHIFT
          out_filt += ( (int32_t)raw_out - (int32_t)out_filt ) >> 4;
      }

      dev.output.voltage = (uint16_t)out_filt;*/
    /*dev.output.voltage              = moving_averages[ADC_CH_OUTPUT_INDEX];
    if(dev.mains_state == MAINS_STATE_OK )
    {
        dev.mains.voltage = dev.output.voltage;
    }
    else
    {
        dev.mains.voltage = 0;
    }
*/
}

static void UpdateInverterType(GPIO_PinState upsTypePin)
{
    if (upsTypePin == GPIO_PIN_SET)
    {
        dev.inverter_type = INVERTER_TYPE_WIDE;
        dev.adc_mains_cut_thr_enter = WUPS_ADC_MAINS_CUT_THR_ENTER;				// Mains Low Cut in WUPS Mode
        dev.adc_mains_cut_thr_exit = WUPS_ADC_MAINS_CUT_THR_EXIT;				// Mains Low Cut Recovery in WUPS Mode
        dev.mains_cut_enter_blocks = WUPS_MAINS_CUT_ENTER_BLOCKS;				// Changeover time from Mains to Backup in WUPS Mode (10U - 22ms)
        dev.mains_cut_exit_block = WUPS_MAINS_CUT_EXIT_BLOCKS;					// Changeover time from Backup to Mains in WUPS Mode
        dev.mains_high_threshold = 610;											// Mains High Cut in WUPS Mode
        dev.adc_mains_high_thr_exit =  WUPS_ADC_MAINS_HIGH_THR_EXIT;			// Mains High Cut Recovery in WUPS Mode

    }
    else
    {
        dev.inverter_type = INVERTER_TYPE_NARROW;
        if(dev.inverter_status.front_switch == true)
        {
        dev.adc_mains_cut_thr_exit = ADC_MAINS_CUT_THR_EXIT;					// Mains Low Cut Recovery in UPS/Narrow Mode
        dev.adc_mains_cut_thr_enter = ADC_MAINS_CUT_THR_ENTER;					// Mains Low Cut in UPS/Narrow Mode
        dev.mains_cut_enter_blocks = MAINS_CUT_ENTER_BLOCKS;					// Changeover from Mains to Backup time in UPS/Narrow Mode
        dev.mains_cut_exit_block = MAINS_CUT_EXIT_BLOCKS;						// Changeover from Backup to Mains time in UPS/Narrow Mode
        dev.mains_high_threshold = 575;											// Mains High Cut in UPS/Narrow Mode
        dev.adc_mains_high_thr_exit =  ADC_MAINS_HIGH_THR_EXIT;					// Mains High Cut Recovery in UPS/Narrow Mode
        }
        else
        {
        	//dev.inverter_type = INVERTER_TYPE_WIDE;
			dev.adc_mains_cut_thr_enter = WUPS_ADC_MAINS_CUT_THR_ENTER;				// Mains Low Cut in WUPS Mode
			dev.adc_mains_cut_thr_exit = WUPS_ADC_MAINS_CUT_THR_EXIT;				// Mains Low Cut Recovery in WUPS Mode
			dev.mains_cut_enter_blocks = WUPS_MAINS_CUT_ENTER_BLOCKS;				// Changeover time from Mains to Backup in WUPS Mode (10U - 22ms)
			dev.mains_cut_exit_block = WUPS_MAINS_CUT_EXIT_BLOCKS;					// Changeover time from Backup to Mains in WUPS Mode
			dev.mains_high_threshold = 610;											// Mains High Cut in WUPS Mode

        }

    }
}

static void UpdateBatteryType(GPIO_PinState battTypePin)
{
    if (battTypePin == GPIO_PIN_SET)
    {
        dev.battery_type = BATTERY_TYPE_LEAD_ACID;
    }
    else
    {
        dev.battery_type = BATTERY_TYPE_SMF;
    }
}

static void UpdateChargeCurrentSetting(GPIO_PinState chgCurrPin)
{
    if ((chgCurrPin == GPIO_PIN_SET) && (dev.bcc != BATTERY_CHG_CURRENT_15A))
    {
        dev.bcc = BATTERY_CHG_CURRENT_15A;
        dev.charging_state = CHARGING_STATE_IDLE;
    }
    else if ((chgCurrPin == GPIO_PIN_RESET) && (dev.bcc != BATTERY_CHG_CURRENT_20A))
    {
        dev.bcc = BATTERY_CHG_CURRENT_20A;
        dev.charging_state = CHARGING_STATE_IDLE;
    }
}

static void UpdateFrontPanelPowerSwitch(GPIO_PinState powerPin)
{
    static bool prevPowerState = true;
    bool currPowerState = (powerPin == GPIO_PIN_SET);

    if (currPowerState != prevPowerState)
    {
        prevPowerState = currPowerState;

        if (currPowerState)
        {
            dev.inverter_status.front_switch = false;
            // HAL_GPIO_WritePin(BUZZER_DRIVE_GPIO_Port, BUZZER_DRIVE_Pin, GPIO_PIN_RESET);
            Flag_clear(&dev);
        }
        else
        {
            dev.inverter_status.front_switch = true;
        }
    }
}

void App_SampleInputs(void)
{
    Inputs_UpdateFromGPIO();
    Inputs_UpdateFromADC();
}

void Inputs_UpdateFromGPIO(void)
{
    // GPIO_PinState upsTypePin = HAL_GPIO_ReadPin(UPS_TYPE_SELECTION_GPIO_Port, UPS_TYPE_SELECTION_Pin);
    // GPIO_PinState battTypePin = HAL_GPIO_ReadPin(BATT_TYPE_SELECTION_GPIO_Port, BATT_TYPE_SELECTION_Pin);
    // GPIO_PinState chgCurrPin = HAL_GPIO_ReadPin(CHG_CURRENT_SELECTION_GPIO_Port, CHG_CURRENT_SELECTION_Pin);
    // GPIO_PinState powerPin = HAL_GPIO_ReadPin(POWER_GPIO_Port, POWER_Pin);

    // UpdateInverterType(upsTypePin);
    // UpdateBatteryType(battTypePin);
    // UpdateChargeCurrentSetting(chgCurrPin);
    // UpdateFrontPanelPowerSwitch(powerPin);
}

void Buzzer_Cntrl(uint32_t *last_update, uint8_t *state, uint16_t freq) // State :  0 - Idle ,1 - disable, 2 - enable (continuous)  , 3 - ticker frequency
{
    //	dev->BUZZER_FREQUENCY = freq;
    //	    dev->inverter_status.buzzer_en = state;

    if (*state == 1)
    {
        // HAL_GPIO_WritePin(BUZZER_DRIVE_GPIO_Port, BUZZER_DRIVE_Pin, RESET);
        *state = 0;
    }
    else if (*state == 2)
    {
        // HAL_GPIO_WritePin(BUZZER_DRIVE_GPIO_Port, BUZZER_DRIVE_Pin, SET);
        *state = 0;
    }
    else if (*state == 3)
    {
        uint32_t curr_tim = HAL_GetTick();
        if (curr_tim - *last_update > freq)
        {
            // HAL_GPIO_TogglePin(BUZZER_DRIVE_GPIO_Port, BUZZER_DRIVE_Pin);
            *last_update = curr_tim;
        }
    }
}

void Buzzer_State(Inverter *const dev, uint8_t state, uint16_t freq) // State :  0 - Idle ,1 - disable, 2 - enable (continuous)  , 3 - ticker frequency
{                                                                    // State :  0 - Idle ,1 - disable, 2 - enable (continuous)  , 3 - ticker frequency
    dev->inverter_status.buzzer_en = state;
    dev->BUZZER_FREQUENCY = freq;
}

void Flag_clear(Inverter *const dev)
{
    battery_dsg_va = 0;
    load_percent = 0;
    dev->backup_fault_status.Batt_High_Cut = false;
    dev->backup_fault_status.Batt_Low_Cut = false;
    dev->backup_fault_status.Overload_shutdown = false;
    //	dev->backup_fault_status.Overload_warning = false;
    dev->backup_fault_status.Short_circuit_detect = false;
    dev->backup_fault_status.Short_circuit_shutdown = false;
    dev->backup_fault_status.MCB_trip_shutdown = false;
    dev->backup_fault_status.High_temp = false;
    dev->backup_fault_status.soft_start = true;
    dev->debug_variable = 0;

    dev->inverter_status.mains_available = false;
   // dev->inverter_status.charging_status = false;
    dev->inverter_status.buzzer_en = 1;

    dev->ticker_status.overload_retry_tick = 0;
    dev->ticker_status.overload_tick = 0;
    dev->ticker_status.short_circuit_retry_tick = 0;
    dev->ticker_status.short_circuit_tick = 0;
    dev->ticker_status.charge_tick = 0;

    dev->LCD_SCREEN = 0;

    dev->charging_state = CHARGING_STATE_IDLE;
}

void Temperature_Cntrl(Inverter *const dev)
{


	uint8_t fan_request = 0U;
	if (fan_boost_active)
	{
		// Handle wrap-around safely
		if ((int32_t)(HAL_GetTick() - fan_boost_end_ms) >= 0)
		{
			fan_boost_active = 0U;
		}
	}
	if (fan_boost_active)
	    {
	        fan_request = 1U;
	    }

	    if (fan_request)
	    {
	        // FAN_ON();
	    }
	    else
	    {
	        // FAN_OFF();
	    }


    if (dev->inverter_status.temperature < TEMPERATURE_FAN_ENABLE)
    {
        // if (!HAL_GPIO_ReadPin(FAN_DRIVE_GPIO_Port, FAN_DRIVE_Pin))
        // {
        //     HAL_GPIO_WritePin(FAN_DRIVE_GPIO_Port, FAN_DRIVE_Pin, SET);
        // }
        if (dev->inverter_status.temperature < TEMPERATURE_HIGH_CUT)
        {
            pwm_stop();
            dev->active_state = FAULT_DETECT;
            dev->backup_fault_status.High_temp = true;
            dev->LCD_SCREEN = 6;
            dev->inverter_status.temperature;
            // Buzzer_State(dev, 2, 1);
        }
    }
    // else if (HAL_GPIO_ReadPin(FAN_DRIVE_GPIO_Port, FAN_DRIVE_Pin) && (dev->active_state != MAINS_MODE))
    // {
    //     HAL_GPIO_WritePin(FAN_DRIVE_GPIO_Port, FAN_DRIVE_Pin, RESET);
    // }
}

void Backup_Fault_check(Inverter *const dev)
{
	Buzzer_State(dev, 1, 1);

	static uint8_t counter = 0;
	static uint8_t tick_counter = 0;
//	dev->inverter_status.zero_cross_available = ZC_IsAlive(dev);
	tick_counter++;
	if(tick_counter >= 12)
	{
		tick_counter = 0;

	if (dev->mains_state == MAINS_STATE_CUT && dev->inverter_status.zero_cross_available && dev->mains_fault_status == NONE )
   // if (dev->mains_state == MAINS_STATE_CUT && dev->inverter_status.zero_cross_available)
    {
    	counter++;

    	if(counter == 1)
    	{
    		mains_adc_avg_val += adc_avg[0];


    	}
    	else
    	{
    		if(mains_adc_avg_val < 17)
    		{
    			//dev->mains_fault_status = MAINS_MCB_TRIP;
    			g_mains_adc_avg_val = dev->inverter_status.zero_cross_available;
    		}
    		else{
    			dev->mains_fault_status = MAINS_STATE_LOW;
    			g_mains_adc_avg_val = dev->inverter_status.zero_cross_available;
    		}
  		 mains_adc_avg_val = 0;
  		 counter = 0;
    	}

    }

    else
    {
        if (dev->mains_fault_status != MAINS_STATE_HIGH)
        {
           // dev->mains_fault_status = NONE;
        }
           }
	}


//	 if (dev->mains_fault_status == MAINS_MCB_TRIP)
//		{
//			if(dev->inverter_status.zero_cross_available)
//			{
//				//dev->active_state = IDLE;
//				pwm_stop();
//			}
//			else{
//				dev->active_state = BACKUP_MODE;
//				pwm_start();
//			}
//		}
//		else{
//
//		}

    // Battery Fault check
    if (dev->battery.voltage < dev->BATTERY_VOLTAGE_LOW_WARNING)
    {
        LOG_PRINTF("[Backup mode]: Battery voltage low warning\n");
        //        lcd_clear();
        //        lcd_print(0, 0, "Battery Low");
        //        lcd_print(1, 3, "Warning");
               // Buzzer_Cntrl(dev, 2000, 3);

        Buzzer_State(dev, 3, 1000);

        if (dev->battery.voltage < dev->BATTERY_VOLTAGE_LOW_SHUTDOWN)
        {
            LOG_PRINTF("[Backup mode]: Battery voltage low shutdown\n");
            dev->active_state = FAULT_DETECT;
            dev->backup_fault_status.Batt_Low_Cut = true;
            Buzzer_State(dev, 2, 1);
                      //  Buzzer_Cntrl(dev, 1, 2);
            //            lcd_clear();
            //            lcd_print(0, 0, "Battery Low");
            //            lcd_print(1, 3, "Shutdown");
            pwm_stop();
        }
    }
    else if (dev->battery.voltage > dev->BATTERY_VOLTAGE_HIGH_CUT)
    {
        LOG_PRINTF("[Backup mode]: Battery voltage high cut\n");
        //        lcd_clear();
        //        lcd_print(0, 0, "Battery High");
        //        lcd_print(1, 3, "Shutdown");
        Buzzer_State(dev, 2, 1);
//        pwm_stop();
        dev->active_state = FAULT_DETECT;
        dev->backup_fault_status.Batt_High_Cut = true;
    }
    else if ((dev->active_state != FAULT_DETECT) && (dev->inverter_status.buzzer_en != 0))
    {
        //        Buzzer_State(dev, 1, 1);
    }

    // battery_dsg_va = 0;
    // load_percent = 0;

    battery_dsg_va = (int)(dev->output.voltage * dev->load.current) / 1000;
    // load_percent = (int)((float)(battery_dsg_va / OVERLOAD_SHUTDOWN_LIMIT) * 100);
    load_percent = (int)((battery_dsg_va * 100.0) / OVERLOAD_SHUTDOWN_LIMIT);

    if (load_percent > 100)
    {
        dev->LCD_SCREEN = 3;
        dev->ticker_status.overload_tick++;
        if (load_percent <= OVERLOAD_RETRY_LIMIT_1)
        {
            allowed_time = 180000 / DEFAULT_STATUS_POLLING_INTERVAL; // 100–110%
            Buzzer_State(dev, 3, 1000);
        }
        else if (load_percent <= OVERLOAD_RETRY_LIMIT_2)
        {
            allowed_time = 60000 / DEFAULT_STATUS_POLLING_INTERVAL; // 111–125%
            Buzzer_State(dev, 3, 2000);
        }
        else if (load_percent <= OVERLOAD_RETRY_LIMIT_3)
        {
            allowed_time = 20000 / DEFAULT_STATUS_POLLING_INTERVAL; // 126-150%
            Buzzer_State(dev, 3, 3000);
        }
        else if (load_percent <= OVERLOAD_RETRY_LIMIT_4)
        {
            allowed_time = 5000 / DEFAULT_STATUS_POLLING_INTERVAL; //  151-200%
            Buzzer_State(dev, 3, 4000);
        }
        // else if (load_percent == OVERLOAD_RETRY_LIMIT_5)
        // {
        //     allowed_time = 5; // 176-200%
        //     Buzzer_State(dev, 3, 5000);
        // }
        else
        {
            allowed_time = 0;
            Buzzer_State(dev, 1, 1);
        }

        // add more stages here if needed: 125%, 150%, etc.

        // Common shutdown logic
        if (dev->ticker_status.overload_tick > allowed_time && allowed_time != 0)
        {
            pwm_stop();
            dev->backup_fault_status.Overload_shutdown = true;
            dev->active_state = FAULT_DETECT;
            dev->ticker_status.overload_tick = 0;
            dev->LCD_SCREEN = 5;
            Buzzer_State(dev, 2, 1);
        }
    }
    /* if (battery_dsg_va > OVERLOAD_WARNING_LIMIT)
     {
         dev->ticker_status.overload_tick++;

         Buzzer_State(dev, 3, 1000);
         dev->LCD_SCREEN = 3;
         if (dev->ticker_status.overload_tick >= DEFAULT_MAINS_AVAILABLE_DELAY)
         { // 5 sec
             pwm_stop();
             dev->backup_fault_status.Overload_shutdown = true;
             dev->active_state = FAULT_DETECT;
             dev->ticker_status.overload_tick = 0;
             dev->LCD_SCREEN = 5;
             Buzzer_State(dev, 2, 1);
         }
     }*/
    else
    {
        dev->ticker_status.overload_tick = 0;
        //        Buzzer_State(dev, 1, 1);
    }
}

void Mains_check(Inverter *const dev)
{
    if (dev->active_state != MAINS_MODE)
    {
        if (dev->inverter_status.front_switch)
        {
            if ((dev->mains.voltage >= dev->MAINS_VOLTAGE_LOW_RECOVERY) &&
                (dev->mains.voltage <= dev->MAINS_VOLTAGE_HIGH_RECOVERY))
            {
                dev->inverter_status.mains_available = true;
                dev->ticker_status.mains_ok_tick++;
            }
            else
            {
                dev->inverter_status.mains_available = false;
                dev->ticker_status.mains_ok_tick = 0;
            }
        }
        else // in case of front switch off hv to start charging if above 80 volt
        {
            if ((dev->mains.voltage > 766) &&
                (dev->mains.voltage < MAINS_WIDE_HIGH_RECOVERY))
            {
                dev->inverter_status.mains_available = true;
                dev->ticker_status.mains_ok_tick++;
            }
            else
            {
                dev->inverter_status.mains_available = false;
                dev->ticker_status.mains_ok_tick = 0;
            }
        }

        if (dev->inverter_status.mains_available)
        {
            if (dev->backup_fault_status.MCB_trip_shutdown)
            {
                dev->backup_fault_status.MCB_trip_shutdown = false;
                Buzzer_State(dev, 1, 1);
                dev->ticker_status.mains_fault_tick = 0;
            }

            if ((dev->ticker_status.mains_ok_tick > DEFAULT_MAINS_AVAILABLE_DELAY) &&
                (dev->ticker_status.zero_cross_tick > 0))
            {
//                pwm_stop();
                dev->backup_fault_status.Batt_Low_Cut = false;
                dev->ticker_status.mains_ok_tick = 0;
                dev->ticker_status.zero_cross_tick = 0;
                dev->ticker_status.charge_tick = 0;
                dev->active_state = MAINS_MODE;
            }
        }
    }
}

/*void Mains_fault_check(Inverter *const dev)
{
    if (dev->active_state == MAINS_MODE)
    {
        dev->ticker_status.mains_ok_tick++;
        if (dev->ticker_status.mains_ok_tick >= dev->MAINS_CHANGEOVER_DELAY)
        {
            if (dev->ticker_status.zero_cross_tick < 1 ||
                (dev->mains.voltage < dev->MAINS_VOLTAGE_LOW_CUT) ||
                (dev->mains.voltage > dev->MAINS_VOLTAGE_HIGH_CUT))
            {
                if (dev->inverter_status.front_switch)
                {
                    dev->backup_fault_status.soft_start = false;
                    dev->inverter_status.mains_available = false;
                    dev->active_state = BACKUP_MODE;
                    dev->ticker_status.mains_ok_tick = 0;
                    dev->ticker_status.zero_cross_tick = 0;
                    charger_duty = 0;
                   // pwm_start();
                }
                else
                {
                    if ((dev->mains.voltage < 739) ||
                        (dev->mains.voltage > MAINS_WIDE_HIGH)) // on high what it does have to cross verify
                    {
                        charge_stop();
                        dev->active_state = IDLE;
                    }
                }
            }

            dev->ticker_status.mains_ok_tick = 0;
            dev->ticker_status.zero_cross_tick = 0;
        }
    }
    else if (((dev->mains.voltage > 600) || (dev->mains.voltage < 550)) && (dev->ticker_status.zero_cross_tick >= 200))
    {
        dev->ticker_status.zero_cross_tick = 100;
    }
    else if ((dev->mains.voltage < 600) && (dev->mains.voltage > 550) && (dev->ticker_status.zero_cross_tick >= 200) && (!dev->backup_fault_status.MCB_trip_shutdown))
    {
        dev->ticker_status.mains_fault_tick++;
        if (dev->ticker_status.mains_fault_tick >= 1000)
        {
            pwm_stop();
            Buzzer_State(dev, 2, 1);
            dev->backup_fault_status.MCB_trip_shutdown = true;
            dev->LCD_SCREEN = 5;
            dev->active_state = FAULT_DETECT;
            dev->ticker_status.mains_fault_tick = 0;
            dev->ticker_status.mains_ok_tick = 0;
            //            dev->ticker_status.zero_cross_tick = 0;
        }
    }
    else if (dev->backup_fault_status.MCB_trip_shutdown)
    {
        dev->ticker_status.mains_fault_tick++;
        if (dev->ticker_status.mains_fault_tick >= 12)
        {
            dev->ticker_status.mains_fault_tick = 0;
            dev->ticker_status.zero_cross_tick--;
            if (dev->ticker_status.zero_cross_tick <= 1)
            {
                Buzzer_State(dev, 1, 1);
                dev->backup_fault_status.MCB_trip_shutdown = false;
                dev->active_state = IDLE;
                dev->ticker_status.mains_fault_tick = 0;
                dev->ticker_status.mains_ok_tick = 0;
                dev->ticker_status.zero_cross_tick = 0;
            }
        }
    }
    else if (dev->ticker_status.mains_fault_tick > 0)
    {
        dev->ticker_status.mains_fault_tick = 0;
    }
}*/

static int32_t charger_duty_contrain(int32_t charger_duty, uint16_t mains_voltage)
{
    int32_t clamped_duty = charger_duty;

    if (clamped_duty < 0)
    {
        clamped_duty = 0;
        return clamped_duty;
    }

    switch (mains_voltage)
    {
        //    case 0 ... 879:
        //        clamped_duty = charger_duty > 2880 ? 2880 : charger_duty;	// 125 > mains > 0 - 90%
        //        break;

        //    case 0 ... 879:
        //        clamped_duty = charger_duty > 2720 ? 2720 : charger_duty; // 125 > mains > 0 - 85%
        //        break;

    case 0 ... 939:
        clamped_duty = charger_duty > 2560 ? 2560 : charger_duty; // 150 > mains > 125 - 80%
        break;

    case 940 ... 999:
        clamped_duty = charger_duty > 2400 ? 2400 : charger_duty; // 175 > mains > 150 - 75%
        break;

    case 1000 ... 1070:
        clamped_duty = charger_duty > 2240 ? 2240 : charger_duty; // 200 > mains > 175 - 70%
        break;

    default:
        clamped_duty = charger_duty > 2150 ? 2150 : charger_duty; // for mains > 200 - 65 %
        break;
    }
    return clamped_duty;
}

void Mains_Charging(Inverter *const dev)
{
    int error = 0;
    Buzzer_State(dev, 1, 1);

    switch (dev->charging_state)
    {

    case CHARGING_STATE_IDLE:

        if (dev->inverter_status.mains_available)
        {
            if (dev->ticker_status.charge_tick >= DEFAULT_MAINS_AVAILABLE_DELAY)
            {
                if (!dev->inverter_status.charging_status)
                {
                    dev->ticker_status.charge_tick = 0;
                    charger_duty = 500;
                    DEFAULT_I_Kp = 0.03f;
                    charge_start();
                }
                else
                {
                    if ((dev->battery.current >= dev->bcc) ||
                        (dev->battery.voltage >= BATTERY_VOLTAGE_BOOST_LIMIT))
                    {
                        DEFAULT_I_Kp = 0.25f;
                        dev->charging_state = CHARGING_STATE_BOOST;
                    }
                    else
                    {
                        error = dev->bcc - dev->battery.current;
                        charger_duty += (DEFAULT_I_Kp * error);

                        charger_duty = charger_duty_contrain(charger_duty, dev->output.voltage);

                        charge_update(charger_duty);
                    }
                }
            }
            else
            {
                dev->ticker_status.charge_tick++;
            }
        }

        break;

    case CHARGING_STATE_BOOST: // constant current

        if (dev->battery.voltage > BATTERY_VOLTAGE_BOOST_LIMIT)
        {
            dev->ticker_status.charge_tick++;
            if ((dev->ticker_status.charge_tick > (1000 / DEFAULT_STATUS_POLLING_INTERVAL)) || // 1 sec
                (dev->battery.voltage >= BATTERY_VOLTAGE_MAX_LIMIT))
            {
                dev->ticker_status.charge_tick = 0;
                Float_counter = 0;
                dev->charging_state = CHARGING_STATE_ABSORPTION;
            }
        }
        else if (dev->ticker_status.charge_tick > 0)
        {
            dev->ticker_status.charge_tick--;
        }

        error = dev->bcc - dev->battery.current;
        charger_duty += (DEFAULT_I_Kp * error);

        charger_duty = charger_duty_contrain(charger_duty, dev->output.voltage);

        charge_update(charger_duty);

        break;

    case CHARGING_STATE_ABSORPTION:

        if (dev->battery.current < 330) // Need to change to 2 Amp
        {
            dev->ticker_status.charge_tick++;
            if (dev->ticker_status.charge_tick > (10000 / DEFAULT_STATUS_POLLING_INTERVAL)) // 10 sec
            {
                if (Float_counter++ > (180000 / DEFAULT_STATUS_POLLING_INTERVAL)) // 180 sec - 1800(30 min)
                {
                    Float_counter = 0;
                    charger_duty = 0;
                    dev->ticker_status.charge_tick = 0;
                    dev->charging_state = CHARGING_STATE_FLOAT;
                }
                else
                {
                    dev->ticker_status.charge_tick = 0;
                }
            }
        }
        else if (dev->ticker_status.charge_tick > 0)
        {
            dev->ticker_status.charge_tick--;
        }

        if (dev->battery.current > dev->bcc)
            charger_duty -= 10;
        else
        {
            error = BATTERY_VOLTAGE_BOOST_LIMIT - dev->battery.voltage;
            charger_duty += (DEFAULT_V_Kp * error);
        }

        charger_duty = charger_duty_contrain(charger_duty, dev->output.voltage);

        charge_update(charger_duty);

        break;

    case CHARGING_STATE_FLOAT:

        error = BATTERY_VOLTAGE_FLOAT_LIMIT - dev->battery.voltage;
        charger_duty += (DEFAULT_V_Kp * error);

        // if (charger_duty < 0)
        //     charger_duty = 0;
        // else if (charger_duty > 1000)
        //     charger_duty = 1000;
        charger_duty = charger_duty_contrain(charger_duty, dev->output.voltage);
        charge_update(charger_duty);

        if (dev->inverter_status.temperature > 1700)
        {
            // HAL_GPIO_WritePin(FAN_DRIVE_GPIO_Port, FAN_DRIVE_Pin, GPIO_PIN_RESET);
        }

        break;
    }
}

void sm_handler_Inverter(Inverter *const dev)
{
    //	static int tick = 0, tick_mains_ok = 0;
    // static int volatile tick_zero_cross = 0;
    switch (dev->active_state)
    {
    case IDLE:
        battery_dsg_va = 0;
        load_percent = 0;
        if (dev->inverter_status.front_switch && dev->mains_fault_status != MAINS_MCB_TRIP)
        {
            pwm_start();
            dev->active_state = BACKUP_MODE;
       //     dev->LCD_SCREEN = 1;

            /*charger_duty                        = 0;
            dev->ticker_status.charge_tick       = 0;
            dev->charging_state                  = CHARGING_STATE_IDLE;
            dev->inverter_status.mains_available  = false;*/
            //            lcd_clear();
            //            lcd_print(0, 0, "Backup Mode:Okay");
            //            lcd_print(1, 0, "Batt V: ");
            //             dev->active_state = BACKUP_MODE;
        }
        if (dev->inverter_status.front_switch == false && dev->mains_state == MAINS_STATE_OK && dev->mains_fault_status != MAINS_STATE_HIGH)
        {
//            dev->charging_state = CHARGING_STATE_IDLE;
        	dev->inverter_status.charging_status = false;
        	dev->active_state = MAINS_MODE;
     //   	dev->LCD_SCREEN = 0;
        	//Mains_Charging(dev);
        }
        else
        {
           /* charger_duty                        = 0;
            dev->ticker_status.charge_tick       = 0;
            dev->charging_state                  = CHARGING_STATE_IDLE;
            dev->inverter_status.mains_available  = false;
            dev->inverter_status.charging_status = false;*/

        }
       /* if(dev->mains_fault_status == MAINS_MCB_TRIP)
        {
        	if(dev->inverter_status.zero_cross_available == false)
        	{
        		 pwm_start();
        		 g_mains_adc_avg_val = 5;
        		 dev->active_state = BACKUP_MODE;
        		 dev->mains_fault_status = NONE;
        	}
        }*/

        break;

    case BACKUP_MODE:
    {
    	cnt2 = 0;
        if (!dev->inverter_status.front_switch)
        {
            pwm_stop();
            dev->active_state = IDLE;
            battery_dsg_va = 0;
            //            lcd_clear();
            //            lcd_print(0, 0, "Backup Mode:IDLE");
            //            lcd_print(1, 0, "Batt V: ");
            //            lcd_print(1, 9, dev->battery.voltage);

            break;
        }
        if(dev->mains_state == MAINS_STATE_OK && dev->mains_fault_status != MAINS_STATE_HIGH )
		{
			mains_check_debounce_counter++;
			// if(mains_check_debounce_counter >= MAINS_CHECK_DEBOUNCE_COUNTER)
			// {
			// 	mains_check_debounce_counter = 0;
			// 	dev->active_state = MAINS_MODE;
			// 	pwm_stop();
			// }
		}
        Backup_Fault_check(dev);
    }
    break;

    case MAINS_MODE:

         Mains_Charging(dev);
         dev->LCD_SCREEN = 0;
        // dev->inverter_status.zero_cross_available = ZC_IsAlive(dev);
        if (dev->output.voltage > dev->mains_high_threshold && (dev->inverter_status.zero_cross_available))
        {



        	dev->mains_fault_status = MAINS_STATE_HIGH;
			charger_duty                       = 0;
			dev->ticker_status.charge_tick      = 0;
			dev->inverter_status.mains_available = false;
			dev->charging_state                 = CHARGING_STATE_IDLE;
			if(dev->inverter_status.front_switch == true)
			{
            dev->active_state = BACKUP_MODE;
            dev->backup_fault_status.soft_start = false;
            pwm_start();
			}
			else{
				 dev->active_state = IDLE;
			}
        }
        else
        {
            dev->mains_fault_status = NONE;
        }
        break;

    case FAULT_DETECT:

        if(dev->mains_fault_status == MAINS_STATE_CUT && !dev->backup_fault_status.Batt_Low_Cut && !dev->backup_fault_status.Overload_shutdown && !dev->backup_fault_status.High_temp && !dev->backup_fault_status.Short_circuit_shutdown)
        {
        	if(dev->inverter_status.zero_cross_available == false)
        	{
        		pwm_start();
        		dev->active_state = BACKUP_MODE;
        	}
        }
        if(dev->mains_fault_status == NONE && dev->mains_state == MAINS_STATE_OK)
        {
        	dev->active_state = MAINS_MODE;
        }
        if ((!dev->inverter_status.front_switch) && (!dev->inverter_status.mains_available) &&
            (!dev->backup_fault_status.MCB_trip_shutdown))
        {
            pwm_stop();
            dev->active_state = IDLE;
            //            lcd_clear();
            //            lcd_print(0, 0, "Mains MCB");
            //            lcd_print(1, 4, "Down");
        }
 /*       else if ((dev->backup_fault_status.Batt_Low_Cut) &&
                 (dev->battery.voltage > dev->BATTERY_VOLTAGE_LOW_RECOVERY))
        {
            dev->active_state = IDLE;
            dev->backup_fault_status.Batt_Low_Cut = false;
            //                Buzzer_Cntrl(dev, 1, 1);
            //                Buzzer_State(dev, 1, 1);
        }*/
        else
        {
            if ((dev->backup_fault_status.Batt_High_Cut) &&
                (dev->battery.voltage < dev->BATTERY_VOLTAGE_HIGH_CUT_RECOVERY))
            {
                dev->active_state = IDLE;
                dev->backup_fault_status.Batt_High_Cut = false;
                                Buzzer_Cntrl(dev, 1, 1);
                //                Buzzer_State(dev, 1, 1);
            }

            if ((dev->backup_fault_status.Overload_shutdown) &&
                (dev->ticker_status.overload_retry_tick < 4))
            {
                dev->ticker_status.overload_tick++;
                //                lcd_clear();
                //                lcd_print(0, 0, "Overload 100%");
                if (dev->ticker_status.overload_tick > DEFAULT_MAINS_AVAILABLE_DELAY)
                {
                    dev->ticker_status.overload_retry_tick++;
                    dev->backup_fault_status.Overload_shutdown = false;
                    dev->ticker_status.overload_tick = 0;
                    dev->active_state = IDLE;
                    //                    lcd_clear();
                    //                    lcd_print(0, 3, "Overload");
                    //                    lcd_print(1, 3, "Shutdown");
                }
            }

            if ((dev->backup_fault_status.Short_circuit_shutdown) &&
                (dev->ticker_status.short_circuit_retry_tick > 1))
            {
                dev->ticker_status.short_circuit_tick++;
                //                Buzzer_State(dev, 1, 1);

                //                lcd_clear();
                //                lcd_print(0, 0, "Short Circuit");
                if (dev->ticker_status.short_circuit_tick > DEFAULT_MAINS_AVAILABLE_DELAY)
                {
                    dev->ticker_status.short_circuit_retry_tick++;
                    dev->backup_fault_status.Short_circuit_detect = false;
                    dev->backup_fault_status.Short_circuit_shutdown = false;
                    dev->ticker_status.short_circuit_tick = 0;
                    dev->active_state = IDLE;
                    dev->LCD_SCREEN = 4;

                    //                    lcd_clear();
                    //                    lcd_print(0, 0, "Short Circuit");
                    //                    lcd_print(1, 3, "Shutdown");
                }
            }

            if ((dev->inverter_status.temperature > TEMPERATURE_HIGH_RECOVERY) && (dev->backup_fault_status.High_temp))
            {
                dev->active_state = IDLE;
                dev->backup_fault_status.High_temp = false;
            }
        }

        break;

    default:
        break;
    }
}

void inverter_update_lcd(Inverter *dev)
{
    uint32_t current_time = HAL_GetTick();

    if (current_time - dev->ticker_status.last_lcd_update < DEFAULT_LCD_FREQUENCY)
    {
        return;
    }

    dev->ticker_status.last_lcd_update = current_time;
    char buff[6];
    lcd_clear();
    switch (dev->LCD_SCREEN)
    {
    case 0:
        if (dev->inverter_status.mains_available)
        {
            lcd_print(0, 3, "Mains ON");
        }
        else
        {
            lcd_print(0, 3, "Mains OFF");
        }
        break;

    case 1:
        if (dev->inverter_status.front_switch)
        {
            if (dev->battery.voltage < dev->BATTERY_VOLTAGE_LOW_WARNING)
            {
                lcd_print(0, 2, "BATTERY LOW");
                lcd_print(1, 4, "WARNING");
                return;
            }
            else if (dev->backup_fault_status.Batt_Low_Cut)
            {
                lcd_print(0, 2, "BATTERY LOW");
                lcd_print(1, 4, "SHUTDOWN");
                return;
            }
            else if (dev->backup_fault_status.Batt_High_Cut)
            {
                lcd_print(0, 0, "BATTERY HIGH");
                lcd_print(1, 3, "SHUTDOWN");
                return;
            }
            else if (dev->active_state == IDLE)
            {
            	lcd_clear();
            	lcd_print(0, 4, "TURN ON");
            	lcd_print(1, 2, "FRONT SWITCH");
            }
            else
            {
                lcd_print(0, 0, "Backup Mode: ON");
                lcd_print(1, 0, "Batt V: ");
                buff[0] = (dev->battery.voltage / 1000) + '0';
                buff[1] = ((dev->battery.voltage / 100) % 10) + '0';
                buff[2] = '.';
                buff[3] = ((dev->battery.voltage / 10) % 10) + '0';
                buff[4] = (dev->battery.voltage % 10) + '0';
                buff[5] = '\0';
                lcd_print(1, 9, buff);
            }
        }
        else
        {
            lcd_print(0, 0, "Backup Mode: OFF");
        }
        break;

    case 2: //  SYSTEM OVERLOAD
    	if (dev->active_state == IDLE)
			{
				lcd_print(0, 4, "TURN ON");
				lcd_print(1, 2, "FRONT SWITCH");
			}
    	else
    	{
        lcd_print(0, 0, "OUTPUT LOAD: ");
        buff[0] = ((load_percent / 100) % 10) + '0';
        buff[1] = ((load_percent / 10) % 10) + '0';
        buff[2] = (load_percent % 10) + '0';
        buff[3] = '\0';
    	}
        lcd_print(0, 12, buff);
        lcd_print(0, 15, "%");

        break;

    case 3: // SYSTEM OVERLOAD
        lcd_print(0, 3, "SYSTEM");
        lcd_print(1, 3, "OVERLOAD");
        if (load_percent > 100)
        {
            return;
        }
        break;

    case 4: // SHORT CIRCUIT
        lcd_print(0, 0, "SHORT CIRCUIT");
        if (dev->backup_fault_status.Short_circuit_shutdown)
        {
            return;
        }
        break;

    case 5: // OVERLOAD SHUTDOWN
        lcd_print(0, 0, "OVERLOAD");
        lcd_print(1, 0, "SHUTDOWN");
        if (dev->backup_fault_status.Overload_shutdown)
        {
            return;
        }
        break;

    case 6: // HIGH TEMPERATURE SHUTDOWN
        lcd_print(0, 0, "HIGH TEMPERATURE");
        lcd_print(1, 0, "SHUTDOWN");
        if (dev->backup_fault_status.High_temp)
        {
            return;
        }
        break;

    case 7: // MCB Trip
        lcd_print(0, 0, "MCB TRIP");
        if (dev->backup_fault_status.MCB_trip_shutdown)
        {
            return;
        }
        break;

        //    case 8:
        //        if (dev->inverter_status.front_switch)
        //        {
        //            lcd_print(0, 0, "Output Load");
        //            lcd_print(1, 0, "Batt V: ");
        //            buff[0] = (dev->battery.voltage / 1000) + '0';
        //            buff[1] = ((dev->battery.voltage / 100) % 10) + '0';
        //            buff[2] = '.';
        //            buff[3] = ((dev->battery.voltage / 10) % 10) + '0';
        //            buff[4] = (dev->battery.voltage % 10) + '0';
        //            buff[5] = '\0';
        //            lcd_print(1, 9, buff);
        //        }
        //        break;s
    }
    dev->LCD_SCREEN++;
    if (dev->LCD_SCREEN > 2)
    {
        dev->LCD_SCREEN = 0;
    }
}

void sm_init(Inverter *dev)
{
    // GenericEvent_t ee;
    // ee.sig = ENTRY;
    dev->active_state = IDLE;
    // sm_handler_top(dev, &ee);
}

int app_init()
{
    LOG_PRINTF("Initializing application...\n");
        lcd_clear();
        lcd_print(0, 0, "Welcome");
        lcd_print(1, 0, "Batt V: ");
//        lcd_print(1, 9, dev->battery.voltage);
    Flag_clear(&dev);
    device_init(&dev);
    sm_init(&dev);

    return 0;
}

int app_process()
{
    device_hardware_sync(&dev);
    // Mains_check(&dev);
    dev.inverter_status.zero_cross_available = ZC_IsAlive(&dev);

    sm_handler_Inverter(&dev);
    Temperature_Cntrl(&dev);
    inverter_update_lcd(&dev);
    // /* fetch elements from the event queue and send to event dispatcher */8
    // if (!isEmpty(getEventQueueInstance()))
    // {
    //  GenericEvent_t ee;
    //  dequeue(getEventQueueInstance(), &ee.sig);
    //  event_dispatcher(&dev, &ee);
    // }

    return 0;
}

#ifdef HOST
int main(void)
{
    app_init();
    app_process();
}
#endif
