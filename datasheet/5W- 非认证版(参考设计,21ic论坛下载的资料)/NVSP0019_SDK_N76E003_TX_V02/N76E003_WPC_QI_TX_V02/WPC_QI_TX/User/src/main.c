/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2016 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Nuvoton Technoledge Corp. 
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : Apr/21/2016
//***********************************************************************************************************

//***********************************************************************************************************
//  File Function: N76E003 PWM Output with dead time insert demo code
//***********************************************************************************************************

#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "ctrl.h"
#include "hardware.h"
#include "qi.h"

bit BIT_TMP;

//------------------- PWM Define End ------------------------------

/************************************************************************************************************
*    Main function 
*		 PWM4 P0.5 simple output a 2.7KHz 1/4high duty signal
************************************************************************************************************/
void main(void)
{
	Set_All_GPIO_Quasi_Mode;
	Timer0_Init();
	Timer2Capture_Init();
	//Pwm_Init();
	WPCQi_Phase = Selection_Phase;
	set_EA;                                     //enable interrupts
	
	while(1) 
	{
		WPC_Qi();
		Display(); 
	}		
}
