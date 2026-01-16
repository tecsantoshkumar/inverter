/*
 * buzzer.h
 *
 *  Created on: Nov 14, 2025
 *      Author: pulkitp
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "main.h"

typedef enum
{
    BUZZER_STATE_IDLE   = 0U,
    BUZZER_STATE_OFF    = 1U,   // one-shot: turn OFF then go back to IDLE
    BUZZER_STATE_ON     = 2U,   // one-shot: turn ON then go back to IDLE
    BUZZER_STATE_TICKER = 3U    // periodic toggle using periodMs
} BuzzerState_t;


/**
 * @brief Periodic buzzer service function.
 *
 * @param lastUpdateMs Pointer to timestamp (ms) of last toggle (HAL_GetTick-based).
 * @param pState       Pointer to buzzer state. Caller sets this to ON/OFF/TICKER.
 *                     Function may modify it (e.g. returns to IDLE after one-shot).
 * @param periodMs     Ticker period in milliseconds (used only for BUZZER_STATE_TICKER).
 */
void Buzzer_Control(uint32_t *lastUpdateMs, BuzzerState_t *pState, uint32_t periodMs);


void Buzzer_StartupBeep(void);

#endif /* INC_BUZZER_H_ */
