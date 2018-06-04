#ifndef _HARDWARE_H
#define _HARDWARE_H


#define PWM_CLK                                 (48000UL)

//#define MARKFU     1 

#ifdef MARKFU
#define CHANNEL_VOL                              4
#define CHANNEL_CUR                              1
#define CHANNEL_TMP                              0
#else
#define CHANNEL_VOL                              4
#define CHANNEL_CUR                              3
#define CHANNEL_TMP                              2
#endif
#define CHANNEL_Q                              5
#define CHANNEL_BAND_GAP               6

void Pwm_SetFrq(uint32_t frq);
void Timer0_Init(void);
void Timer2Capture_Init(void);
void Pwm_Init(void);
void Pwm_Work_Q_Measure(void);
void ADC_Convert(uint8_t channel,uint16_t *result);

#endif


