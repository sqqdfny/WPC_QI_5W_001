#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "ctrl.h"
#include "hardware.h"
#include "qi.h"

#define LED_RED_GPIO  P13 
#define LED_BLUE_GPIO  P14
#define BUZZER_GPIO   P30

uint16_t    LedFlashDelayCnt;
uint8_t LedStatus = 0;
uint8_t LedDisplayCnt = 0;

QI_POWER_CHARGE_STATE_TYPEDEF QiPowerChargeState = State_Charge_Standby;

#ifdef MARKFU
void SysPara_Delay_Count(void)
{
    static uint16_t led_status_cnt = 0;
    static uint8_t led_cnt = 0;
	
     if(LedFlashDelayCnt<1000)
        LedFlashDelayCnt++;
    else
        LedFlashDelayCnt=0; 
    if(led_status_cnt < 300)
        led_status_cnt++;
    else
    {
        led_status_cnt = 0;
        LedStatus++;
	 led_cnt = 0;	
    }
    switch(LedStatus)
    {
        case 0:
            P04 = 1;
            break;
        case 1:
            led_cnt++;
            if(led_cnt>2)
            {
                P04 = 0;
		  led_cnt = 0;
            }
	     else
                P04 = 1;
            break;	
        case 2:
            led_cnt++;
            if(led_cnt>1)
            {
                P04 = 0;
		  led_cnt = 0;
            }
	     else
                P04 = 1;			
            break;
        //case 3:
	     //P04 ^= 1;	
            //break;
        case 3:
            led_cnt++;
            if(led_cnt>1)
            {
                P04 = 1;
		  led_cnt = 0;
            }
	     else
                P04 = 0;				
            break;
        case 4: 
            led_cnt++;
            if(led_cnt>2)
            {
                P04 = 1;
		  led_cnt = 0;
            }
	     else
                P04 = 0;
            break;
        case 5: 
            led_cnt++;
            if(led_cnt>3)
            {
                P04 = 1;
		  led_cnt = 0;
            }
	     else
                P04 = 0;
            break;	
        case 6: 
            led_cnt++; 
            if(led_cnt>4)
            {
                P04 = 1;
		  led_cnt = 0;
            }
	     else
                P04 = 0;
            break;
        case 7: 
            led_cnt++; 
            if(led_cnt>5)
            {
                P04 = 1;
		  led_cnt = 0;
            }
	     else
                P04 = 0;
            break;	
        case 8: 
            led_cnt++; 
            if(led_cnt>7)
            {
                P04 = 1;
		  led_cnt = 0;
            }
	     else
                P04 = 0;
            break;	
        case 9: 
            led_cnt++; 
            if(led_cnt>10)
            {
                P04 = 1;
		  led_cnt = 0;
            }
	     else
                P04 = 0;
            break;			
        case 10:
            P04 = 0;
            break;			
        default:
	     if(LedDisplayCnt < 3)
	     {
		 LedDisplayCnt++;
		 LedStatus = 0;
	     }
	     else if(QiPowerChargeState == State_Charge_On)
		 LedStatus = 0;
	     else
		 LedStatus = 10;
		 	
            break;
    }
}
#else
void SysPara_Delay_Count(void)
{
     if(LedFlashDelayCnt<1000)
        LedFlashDelayCnt++; 
    else
        LedFlashDelayCnt=0; 
}
#endif

/*----------------------------------------------------------------------------
 *       Led control : the normal charge
 *---------------------------------------------------------------------------*/
void Led_Charge_On(void)
{
    LED_BLUE_GPIO = 0;
    LED_RED_GPIO = 1;
}

/*----------------------------------------------------------------------------
 *       Led control : System standby
 *---------------------------------------------------------------------------*/
void Led_Standby(void)
{
    LED_BLUE_GPIO = 1;
    LED_RED_GPIO = 1;
}

/*----------------------------------------------------------------------------
 *       Led control : charge full
 *---------------------------------------------------------------------------*/
void Led_Charge_Full(void)
{
    LED_BLUE_GPIO = 1; 
    LED_RED_GPIO = 1;
}

/*----------------------------------------------------------------------------
 *       Led control : charge error
 *---------------------------------------------------------------------------*/
void Led_Charge_Error(void)
{
    LED_BLUE_GPIO = 1;
    LED_RED_GPIO = 0;
}

/*----------------------------------------------------------------------------
 *       Led control : close all led
 *---------------------------------------------------------------------------*/
void Led_Charge_Off(void)
{
    LED_BLUE_GPIO = 1;
    LED_RED_GPIO = 1;
}

/*----------------------------------------------------------------------------
 *       Led control : charge over-temperature
 *---------------------------------------------------------------------------*/
void Led_Charge_OverTermper(void)
{
    if(LedFlashDelayCnt<500) 
    {
        Led_Charge_Error();
    }
    else
    {
        Led_Charge_Off();
    }
}

/*----------------------------------------------------------------------------
 *       Led control : detect the FOD
 *---------------------------------------------------------------------------*/
void Led_Charge_FOD(void)
{
    if(LedFlashDelayCnt<500) 
    {
        Led_Charge_Error();
    }
    else
    {
        Led_Charge_Off();
    }
}


#ifdef MARKFU
void Display(void)
{
    switch(QiPowerChargeState)
    {
        case State_Charge_Standby:
            if(LedDisplayCnt > 2)
	         LedStatus = 10;
            break;
        case State_Charge_Ping: 
            if(LedDisplayCnt > 2)
	         LedStatus = 10;			
            break;			
        case State_Charge_On:
            break;
        case State_Charge_Full:
            break;
        case State_Charge_FOD:
            Led_Charge_FOD();
            break;
        case State_Charge_OverTemper:
            Led_Charge_OverTermper();
            break;
        default:
	     break;
    }

}
#else
void Display(void)
{
    switch(QiPowerChargeState)
    {
        case State_Charge_Standby:
            Led_Standby();
            break;
        case State_Charge_Ping: 
            Led_Charge_Off();
            break;			
        case State_Charge_On:
            Led_Charge_On();
            break;
        case State_Charge_Full:
            Led_Charge_Full();
            break;
        case State_Charge_FOD:
            Led_Charge_FOD();
            break;
        case State_Charge_OverTemper:
            Led_Charge_OverTermper();
            break;
        default:
	     break;
    }

}
#endif

