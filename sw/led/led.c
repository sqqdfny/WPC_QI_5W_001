

#include "led.h"
#include "qi.h"

#define LED_RED_ON()    (P13 = 0)
#define LED_RED_OFF()   (P13 = 1)
#define GET_LED_RED()   (!P13)

#define LED_BLUE_ON()   (P14 = 0)
#define LED_BLUE_OFF()  (P14 = 1)
#define GET_LED_BLUE()  (!P14)

static u8 xdata _led_dly_tick = 0;
//====================================================================
static void LedChargeStanby(void)
{
	LED_RED_OFF();
	LED_BLUE_OFF();
}

static void LedChargePing(void)
{
	if(_led_dly_tick) return;
	
	LED_BLUE_OFF();
	if(GET_LED_RED())
	{
		LED_RED_OFF();
		_led_dly_tick = 190;
	}
	else
	{
		LED_RED_ON();
		_led_dly_tick = 10;
	}
}
static void LedChargeOn(void)
{
	if(_led_dly_tick) return;
	
	LED_RED_OFF();
	if(GET_LED_BLUE())
	{
		LED_BLUE_OFF();
		_led_dly_tick = 90;
	}
	else
	{
		LED_BLUE_ON();
		_led_dly_tick = 10;
	}
}
static void LedChargeFull(void)
{
	LED_RED_OFF();
	LED_BLUE_ON();
}
static void LedChargeErr(void)
{
	if(_led_dly_tick) return;
	
	LED_BLUE_OFF();
	if(GET_LED_RED())
	{
		LED_RED_OFF();
		_led_dly_tick = 25;
	}
	else
	{
		LED_RED_ON();
		_led_dly_tick = 25;
	}
}
void LedFunction(void)
{
	if(_led_dly_tick) 
	{
		_led_dly_tick --;
	}
	
	switch(GetCurChargeState())
	{
		case State_Charge_Stanby:
			LedChargeStanby();
			break;
    case State_Charge_Ping:	
		case State_Charge_Config:
			LedChargePing();
			break;
    case State_Charge_On:
			LedChargeOn();
			break;
    case State_Charge_Full:
			LedChargeFull();
			break;
	  case State_Charge_RePing:
			break;
    case State_Charge_OverTemper:
		case State_Charge_Err:
			LedChargeErr();
			break;
		default:
			LedChargeErr();
			break;
	}
}

//====================================================================
void LedInit(void)
{
	LED_RED_ON();
	LED_BLUE_ON();
	clr_P1M1_3;set_P1M2_3;
	clr_P1M1_4;set_P1M2_4;
}
//====================================================================
//end files
