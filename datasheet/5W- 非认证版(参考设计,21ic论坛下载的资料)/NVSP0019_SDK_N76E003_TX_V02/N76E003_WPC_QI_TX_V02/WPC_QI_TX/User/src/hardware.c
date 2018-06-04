#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "hardware.h"
#include "qi.h"

#define TH0_INIT        0xFA //5.0ms@XTAL=12MHz, Period = (10.85/2) ms@XTAL=22.1184MHz 
#define TL0_INIT        0xCA

/************************************************************************************************************
*    TIMER 0 interrupt subroutine
************************************************************************************************************/
void Timer0_ISR (void) interrupt 1          //interrupt address is 0x000B
{
	TH0 = TH0_INIT;
	TL0 = TL0_INIT;    
	WPC_Qi_Delay_Count();
}

void Timer0_Init(void)
{
	TMOD = 0xFF;
	TIMER0_MODE1_ENABLE;                        //Timer 0 mode configuration
	clr_T0M;	
	TH0 = TH0_INIT;
	TL0 = TL0_INIT;	
	
	set_ET0;                                    //enable Timer0 interrupt	
	set_TR0;                                    //Timer0 run	
}

/************************************************************************************************************
*    Timer2 Capture interrupt subroutine
************************************************************************************************************/
void Capture_ISR (void) interrupt 12
{
	uint16_t Pulse;

	clr_CAPF0;                          // clear capture0 interrupt flag
	Pulse = C0H;
	Pulse <<= 8;
	Pulse |= C0L;
	Decode(Pulse);    
	clr_TF2;
}

void Timer2Capture_Init(void)
{
	P01_Input_Mode;
	P01 = 1;

	TIMER2_CAP0_Capture_Mode;
	IC4_P01_CAP0_BothEdge_Capture;
			
	set_ECAP;                                   //Enable Capture interrupt
	set_TR2;                                    //Triger Timer2
	set_EA;	
}

void PWM_DEAD_TIME_VALUE(UINT16	DeadTimeData)
{
	UINT8 deadtmphigh,deadtmplow;
	
	deadtmplow = DeadTimeData;
	deadtmphigh = DeadTimeData>>8;
	BIT_TMP = EA;
	if (deadtmphigh==0x01)
	{
		EA = 0;
		TA = 0xAA;
		TA = 0x55;
		PDTEN|=0x10;
	}
	TA = 0xAA;
	TA = 0x55;
	PDTCNT = deadtmplow;
	EA = BIT_TMP;
}

void Pwm_SetFrq(uint32_t frq)
{
	uint16_t reg;

       //frq set
	reg = 16000000/frq;
	reg -= 1;
	PWMPH = (uint8_t)(reg >> 8);
	PWMPL = (uint8_t)(reg);

	//duty set
	reg >>= 1;
	PWM0H = (uint8_t)(reg >> 8);
	PWM0L = (uint8_t)(reg);	
	PWM2H = (uint8_t)(reg >> 8);
	PWM2L = (uint8_t)(reg);

	while(LOAD);
	set_LOAD;
}

void Pwm_Init(void)
{	
/*-----------------------------------------------------------------------------------------------------------------
	PWM frequency 	= Fpwm/((PWMPH,PWMPL)+1) = (16MHz/8)/(0x7CF+1) = 1KHz (1ms)
	PWM2 high duty 	= PWM2H,PWM2L = 0x01CF = 1/4 PWM period
	PWM0 high duty 	= PWM0H,PMW0L = 0x03CF = 1/2 PWM period
	Dead time 			= 0x1FF <PDTEN.4+PDTCNT[7:0]>/Fsys = 0x1FF/Fsys = 512/16000000 = 32 us (max value)
-----------------------------------------------------------------------------------------------------------------*/
	P12_PushPull_Mode;
	P11_PushPull_Mode;
	P10_PushPull_Mode;
	P00_PushPull_Mode;

    PWM0_P12_OUTPUT_DISABLE;
    PWM1_P11_OUTPUT_DISABLE;
    PWM2_P10_OUTPUT_DISABLE;
    PWM3_P00_OUTPUT_DISABLE;
    P12 = 1;
    P11 = 1;
    P10 = 1;
    P00 = 1;
	
	PWM0_P12_OUTPUT_ENABLE;
	PWM1_P11_OUTPUT_ENABLE;
	PWM2_P10_OUTPUT_ENABLE;
	PWM3_P00_OUTPUT_ENABLE;
	
	PWM_COMPLEMENTARY_MODE;					//Only this mode support deat time function
	PWM_CLOCK_DIV_1; 

	Pwm_SetFrq(WPC_QI_INIT_FREQ);
	PWM2_OUTPUT_INVERSE;
	PWM3_OUTPUT_INVERSE;
	PWM01_DEADTIME_ENABLE;
	PWM23_DEADTIME_ENABLE;
	PWM_DEAD_TIME_VALUE(10);			//value never over 0x1FF
		
	//Please always setting Dead time if needed before PWM run.		
    	set_LOAD;
    	set_PWMRUN;		
}

void Pwm_Turn_Off(void)
{
	clr_LOAD;
	clr_PWMRUN;
}

void Pwm_Work_Q_Measure(void)
{	
/*-----------------------------------------------------------------------------------------------------------------
	PWM frequency 	= Fpwm/((PWMPH,PWMPL)+1) = (16MHz/8)/(0x7CF+1) = 1KHz (1ms)
	PWM2 high duty 	= PWM2H,PWM2L = 0x01CF = 1/4 PWM period
	PWM0 high duty 	= PWM0H,PMW0L = 0x03CF = 1/2 PWM period
	Dead time 			= 0x1FF <PDTEN.4+PDTCNT[7:0]>/Fsys = 0x1FF/Fsys = 512/16000000 = 32 us (max value)
-----------------------------------------------------------------------------------------------------------------*/
	P12_PushPull_Mode;
	P11_PushPull_Mode;
	P10_PushPull_Mode;
	P00_PushPull_Mode;

    PWM0_P12_OUTPUT_DISABLE;
    PWM1_P11_OUTPUT_DISABLE;
    PWM2_P10_OUTPUT_DISABLE;
    PWM3_P00_OUTPUT_DISABLE;
    P12 = 1;
    P11 = 1;
    P10 = 1;
    P00 = 1;
	
	PWM0_P12_OUTPUT_ENABLE;
	PWM1_P11_OUTPUT_ENABLE;
	PWM2_P10_OUTPUT_ENABLE;
	PWM3_P00_OUTPUT_ENABLE;
	
	PWM_COMPLEMENTARY_MODE;					//Only this mode support deat time function
	PWM_CLOCK_DIV_1; 

	Pwm_SetFrq(WPC_QI_INIT_FREQ);
	PWM2_OUTPUT_INVERSE;
	PWM3_OUTPUT_INVERSE;
	PWM01_DEADTIME_ENABLE;
	PWM23_DEADTIME_ENABLE;
	PWM_DEAD_TIME_VALUE(10);			//value never over 0x1FF
		
	//Please always setting Dead time if needed before PWM run.		
    	set_LOAD;
    	set_PWMRUN;		
}

void ADC_Convert(uint8_t channel,uint16_t *result)
{	
	switch(channel)
	{
#ifdef MARKFU	
		case CHANNEL_TMP:
			Enable_ADC_AIN0;
		break;
		case CHANNEL_CUR:
			Enable_ADC_AIN1;
		break;
		case CHANNEL_VOL:
			Enable_ADC_AIN4;
		break;
#else
		case CHANNEL_TMP:
			Enable_ADC_AIN2;
		break;
		case CHANNEL_CUR:
			Enable_ADC_AIN3;
		break;
		case CHANNEL_VOL:
			Enable_ADC_AIN4;
		break;
#endif
		case CHANNEL_Q:
			Enable_ADC_AIN5;
		break;
		
		case CHANNEL_BAND_GAP:
			Enable_ADC_BandGap;
		break;		
	}
	clr_ADCF;
	ADCF = 0;
	set_ADCS;                             //Trigger ADC start conversion
	while(ADCF == 0);
	ADCF = 0;
	*result = (uint16_t )ADCRH;
	*result = (*result << 4) | (uint16_t )(ADCRL & 0x0f);
}

