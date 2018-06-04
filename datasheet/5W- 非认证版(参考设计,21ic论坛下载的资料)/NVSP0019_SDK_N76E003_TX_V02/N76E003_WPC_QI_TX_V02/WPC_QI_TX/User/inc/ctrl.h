#ifndef _CTRL_H
#define _CTRL_H

#define _ENABLE_FOD_
//#define _ENABLE_SLEEP_
#define _ENABLE_AC_BUZZ_

typedef enum
{
    State_Charge_Standby=0,
    State_Charge_Ping,	
    State_Charge_On,
    State_Charge_Full,
    State_Charge_OverTemper,
    State_Charge_FOD,
}QI_POWER_CHARGE_STATE_TYPEDEF;


#define BUZZ_SOUND_DELAY        75

#define TEMPERATURE_MAX         2200
#define TEMPERATURE_MIN         1900

extern QI_POWER_CHARGE_STATE_TYPEDEF QiPowerChargeState;

void SysPara_Delay_Count(void);
void Display(void);

#endif

