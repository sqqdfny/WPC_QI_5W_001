/*
	by:sqqdfny 2017-09-30 QQ:792739069  e-mail:sqqdfny@163.com
*/


#include "adc.h"


// #define MAX_TEMP_TAB   (sizeof(temp_tab) / 2)
// //==========================================================
// //10K 3380 -20-60℃
// u16 const code temp_tab[] = 
// {
// 	3613 ,3591 ,3567 ,3543 ,3518 ,3492 ,3466 ,3439 ,3411 ,3382 ,  //-20~-11
// 	3353 ,3323 ,3292 ,3261 ,3229 ,3196 ,3163 ,3129 ,3094 ,3059 ,  //-10~-1
// 	3024 ,2988 ,2951 ,2914 ,2877 ,2838 ,2800 ,2762 ,2723 ,2684 ,  //0~9
// 	2644 ,2605 ,2565 ,2525 ,2485 ,2445 ,2405 ,2365 ,2325 ,2285 ,  //10~19
// 	2245 ,2205 ,2165 ,2126 ,2087 ,2048 ,2009 ,1970 ,1932 ,1895 ,  //20~29
// 	1856 ,1820 ,1783 ,1747 ,1710 ,1675 ,1640 ,1606 ,1572 ,1537 ,  //30~39
// 	1505 ,1472 ,1441 ,1408 ,1378 ,1347 ,1317 ,1288 ,1259 ,1231 ,  //40~49
// 	1203 ,1176 ,1149 ,1123 ,1097 ,1073 ,1048 ,1023 ,1000 ,976  ,  //50~59
// 	955  ,933  ,911  ,891  ,868  ,850  ,829  ,811  ,793  ,774  ,  //60~69
//   755                                                           //70
// };
//==========================================================
//ch: 0~15
// static u16 idata adc_buffer[4] = 0;
// static u8  idata temp_buffer = 0;
// static u8  idata adc_sampling_period = 0;

// //获取指定通道当前的AD值
// u16 GetAd(u8 ch)
// {
// 	u16 tmp;
// 	switch(ch)
// 	{
// 		case ADC_CH_TEMP:
// 			ADCCON0 = 0x02;  	//Enable_ADC_AIN2;
// 			break;
// 		case ADC_CH_CURRENT:
// 			ADCCON0 = 0x03;  	//Enable_ADC_AIN3;
// 			break;
// 		case ADC_CH_INPUT_VOLGATE:
// 			ADCCON0 = 0x04;  	//Enable_ADC_AIN4;
// 			break;
// 		default:
// 			ADCCON0 = 0x08;  //Enable_ADC_BandGap;
// 			break;
// 	}
// 	
// 	clr_ADCF;
// 	set_ADCS;																	// Each time ADC start trig signal
// 	while(ADCF == 0);
// 	tmp = (u16)ADCRH;
// 	tmp = (tmp << 4) | ((u16)(ADCRL & 0x0f));
// 	return (tmp);
// }

// //获取指定通道温度
// //return(℃): -20 - 59   
// //return: <-20->传感器开路
// //return: >+59->传感器短路
// //51MCU在汇编层没有负数指令,为了比较方便,实际返回值为温度值 +40℃
// u8 GetTemp(u8 ch)
// {	
// 	ch = ch;
// 	return(temp_buffer);
// }

// static s8 Ad2Temp(u16 ad)
// {
// 	s8 i;
// 	if(ad > temp_tab[0])                return TEMP_SENSOR_OPEN;
// 	if(ad < temp_tab[MAX_TEMP_TAB - 1]) return TEMP_SENSOR_SHORT;
// 	
// 	for(i = (MAX_TEMP_TAB - 2); i > 0; i --)
// 	{
// 		if(ad < temp_tab[i]) break;
// 	}
// 	return TEMP_2_UINT(i - 20);
// }
// //==========================================================

// void AdcTimeHook(void)
// {
// 	u16 temp1=0,temp2=0;
// 	if(0 == adc_sampling_period--)
// 	{
// 		adc_sampling_period = SYSTEM_TICK;
// 		
// // 		ADCCON0 = 0x08;  //Enable_ADC_BandGap;
// // 		clr_ADCF;
// // 		set_ADCS;																	// Each time ADC start trig signal
// // 		while(ADCF == 0);
// // 		adc_buffer[ADC_CH_REF] = (((u16)ADCRH) << 4) | ((u16)(ADCRL & 0x0f));
// 		
// 		ADCCON0 = 0x02;  //Enable_ADC_AIN1;
// 		clr_ADCF;
// 		set_ADCS;																	// Each time ADC start trig signal
// 	  while(ADCF == 0);
// 		adc_buffer[ADC_CH_TEMP] = (((u16)ADCRH) << 4) | ((u16)(ADCRL & 0x0f));
// 		temp_buffer = Ad2Temp(adc_buffer[ADC_CH_TEMP]);

// 		ADCCON0 = 0x03;  //Enable_ADC_AIN3;
// 		clr_ADCF;
// 		set_ADCS;																	// Each time ADC start trig signal
// 	  while(ADCF == 0);
// 		adc_buffer[ADC_CH_CURRENT] = (((u16)ADCRH) << 4) | ((u16)(ADCRL & 0x0f));
// 		
// 		ADCCON0 = 0x04;  //Enable_ADC_AIN4;
// 		clr_ADCF;
// 		set_ADCS;																	// Each time ADC start trig signal
// 	  while(ADCF == 0);
// 		adc_buffer[ADC_CH_INPUT_VOLGATE] = (((u16)ADCRH) << 4) | ((u16)(ADCRL & 0x0f));
// 	}
// }
//==========================================================
//线圈电流 mA
u16 GetCoilCurrent(void)
{
	u8 tmp8;
	u32 tmp = 0;
	ADCCON0 = 0x03;  	//Enable_ADC_AIN3;
	clr_ADCF;
	set_ADCS;																	// Each time ADC start trig signal
	while(ADCF == 0);
	tmp8 = ADCRH;
	*(((u8*)(&tmp)) + 2) = (tmp8 >> 4) & 0x0F;
	tmp8 = ((tmp8 << 4) & 0xF0) | (ADCRL & 0x0f);
	*(((u8*)(&tmp)) + 3) = tmp8;
	
	tmp = tmp * 15625;
	tmp = tmp / 13824;
	return ((u16)tmp);//(*((u16*)((u8*)(&tmp)) + 2));
}
//==========================================================
void InitAdc(void)
{
	u8 temp=0;
	P05_Input_Mode;
	P06_Input_Mode;
	P07_Input_Mode;
	
	ADCCON1 = BIT0;    //ADCEN
	ADCCON2 = 0;
	AINDIDS = BIT2 | BIT3 | BIT4;  //关闭数字输入
	
	ADCCON0 = 0x08;
	set_ADCS;
	while(ADCF == 0);
	set_ADCS;
	while(ADCF == 0);
	set_ADCS;
	while(ADCF == 0);
}
//==========================================================
//end files

