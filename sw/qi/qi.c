

#include "qi.h"
#include "adc.h"


static volatile u8 xdata qi_dly_tick = 0;  			  //QI延时计数用
static volatile u16 xdata qi_rx_cmd_timeout = 0;	//QI接收命令超时,认为是充电设备被移开,需要停止电能传输

static QI_POWER_CHARGE_STATE_TYPEDEF data QiPowerChargeState = State_Charge_Stanby;
static u32 xdata qi_tx_freq = WPC_QI_INIT_FREQ;
static u8  xdata qi_tx_duty = 50;
static u16 xdata qi_wait_ms = 0;


static volatile u8 xdata rx_signal_strength;   	//接收机的信号强度
static volatile u8 xdata rx_power_class;				//接收机的类型
static volatile u8 xdata max_rx_power;					//接收机的最大功率
static volatile s8 xdata rx_control_error;     	//误差控制参数
static volatile u8 xdata rx_charge_state;				//接收机充电状态 0-100 0xff->无电池
static volatile u8 xdata hold_off_ms;						//功率控制延时
static volatile bit rx_control_error_flag = 0; 	//误差控制执行标志
//====================================================================
void QiTimeTickHook(void)
{
	if(qi_dly_tick) 			qi_dly_tick --;
	if(qi_rx_cmd_timeout) qi_rx_cmd_timeout --;
}
//====================================================================
//当前充电板状态
QI_POWER_CHARGE_STATE_TYPEDEF GetCurChargeState(void)
{
	return (QiPowerChargeState);
}

//====================================================================

//====================================================================
//以下为数据的接收与解析
//====================================================================
typedef union 
{
	u8 d;
	struct
	{
		u8 b0 : 1;
		u8 b1 : 1;
		u8 b2 : 1;
		u8 b3 : 1;
		u8 b4 : 1;
		u8 b5 : 1;
		u8 b6 : 1;
		u8 b7 : 1;
	}b;
}structIrCode;

static volatile u16 data cap;
static volatile u8 data cap_index;
static volatile bit sync_flag = 0;        			//捕获到同步头标志
static volatile bit sync_valid_flag = 0;  			//同步头有效标志
static volatile bit first_start_bit_flag = 0;		//同步头之后的第一个起始位捕获标志
static volatile bit start_bit_valid_flag = 0;		//开始位有效标志
static volatile bit data_bit_sikp_flag;					//接收到位是1时,需要跳过下一个边沿
static volatile structIrCode data rdata;				//接收到的有效字节
static volatile bit data_bit_parity;						//接收到的字节校验位
static volatile u8 data rx_buf[27];							//最多有27bytes
static volatile u8 data rx_count;								//接收到的字节数
static volatile u8 data rx_count_req;						//需要接收的字节数

// static volatile u8 
//--------------------------------------------------------------------
//返回 1 表示校验通过
static bit VerifyRxData(void)
{
	u8 i, checksum = 0;
	if(0 == rx_count) return 0;
	for(i = 0; i < rx_count; i ++)
	{
		checksum ^= rx_buf[i];
	}
	return (0 == checksum) ? 1 : 0;
}
//--------------------------------------------------------------------
static void SignalStrengthParse(void)
{
	rx_signal_strength = rx_buf[1];
	if(QiPowerChargeState == State_Charge_Ping)
	{//如果当前为PING阶段,进入配置阶段
		QiPowerChargeState = State_Charge_Config;
		qi_wait_ms = 1;
	}
}
//--------------------------------------------------------------------
//结束功率传输
static void EndPowerTransferParse(void)
{
	switch(rx_buf[1])
	{
		case 0x01: 	//充满
			QiPowerChargeState = State_Charge_Full;
			break;
		case 0x07:	//重新谈判
			QiPowerChargeState = State_Charge_RePing;
			break;
		case 0x08:	//接收器认为发射器不响应控制错误数据包
			break;
		default:		//其它错误
			QiPowerChargeState = State_Charge_Err;
			break;
	}
}
//--------------------------------------------------------------------
static void ControlErrorParse(void)
{
	if(QiPowerChargeState == State_Charge_On)
	{
		rx_control_error = (s8)(rx_buf[1]);
		rx_control_error_flag = 1;
	}
}
//--------------------------------------------------------------------
//配置数据解析
static void ConfigurationParse(void)
{
	rx_power_class = (rx_buf[1] >> 6) & 0x03;
	max_rx_power = rx_buf[1] & 0x3f;
	if(QiPowerChargeState < State_Charge_On)
	{//如果当前为PING阶段,收到配置信息后进入功率传输阶段
		QiPowerChargeState = State_Charge_On;
		qi_wait_ms = 1;
	}
}
//--------------------------------------------------------------------
static void IdentificationParse(void)
{
}
static void ExtendedIdentificationParse(void)
{
}
//--------------------------------------------------------------------
//接收到的数据解析
static void QiRxDataParse(void)
{
	if(VerifyRxData())
	{
		qi_rx_cmd_timeout = SYSTEM_TICK * 3;
		switch(rx_buf[0])
		{
			case 0x01:  	//Signal Strength
				SignalStrengthParse();
				break;
			case 0x02:		//End Power Transfer
				EndPowerTransferParse();
				break;
			case 0x03:		//Control Error
				ControlErrorParse();
				break;
			case 0x04:		//8-bit Received Power
				break;
			case 0x05:		//Charge Status
				rx_charge_state = rx_buf[1];
				break;
			case 0x06:		//Power Control Hold-off
				hold_off_ms = rx_buf[1];
				break;
			case 0x51:		//Configuration
				ConfigurationParse();
				break;
			case 0x71:		//Identification 
				IdentificationParse();
				break;
			case 0x81:		//Extended Identification
				ExtendedIdentificationParse();
				break;
			default:
				rx_buf[1] = rx_buf[1];
				break;
		}
	}
}
//--------------------------------------------------------------------
void TimerCapture_ISR(void) interrupt 12  using 2  //interrupt address is 0063H
{
	CAPCON0 &= ~BIT0;
	*(((u8*)&cap)  ) = C0H;
	*(((u8*)&cap)+1) = C0L;
	
	if(sync_flag)
	{
		if(sync_valid_flag)
		{
			if(first_start_bit_flag)
			{
				if(start_bit_valid_flag)
				{
					if(!data_bit_sikp_flag)
					{//不需要跳过边沿
						if(cap > 150 && cap < 600)
						{
							cap_index ++;
							if(cap_index < 9)
							{
								rdata.d >>= 1;
								data_bit_sikp_flag = (cap < 350) ? 1 : 0;
								rdata.b.b7 = data_bit_sikp_flag;
							}
							else
							if(cap_index == 9)
							{
								data_bit_parity = (cap > 350) ? 1 : 0;
							}
							else
							{//已经接收完一个字节,准备寻找下一个字节的起始位
								start_bit_valid_flag = 0;
								rx_buf[rx_count] = rdata.d;
								rx_count ++;
								if(rx_count > 27)
								{
									sync_flag = 0;
								}
								else
								if(rx_count >= rx_count_req)
								{//接收到完整的数据包
									sync_flag = 0;
									QiRxDataParse();
								}
								else
								if(1 == rx_count)
								{
									if(rx_buf[0] < 0x20)
									{
										rx_count_req = 3;
									}
									else
									if(rx_buf[0] < 0x80)
									{
										rx_count_req = 4 + ((rx_buf[0] - 32) >> 4);
									}
									else
									if(rx_buf[0] < 0xE0)
									{
										rx_count_req = 10 + ((rx_buf[0] - 128) >> 3);
									}
									else
									{
										rx_count_req = 22 + ((rx_buf[0] - 224) >> 2);
									}
								}
							}
						}
						else
						{
							sync_flag = 0;
						}
					}
					else
					{//需要跳过一个边沿
						if(cap > 150 && cap < 350)
						{
							data_bit_sikp_flag = 0;
						}
						else
						{
							sync_flag = 0;
						}
					}
				}
				else
				{
					if(cap > 350 && cap < 600)
					{//找到有效的起始位
						start_bit_valid_flag = 1;
						data_bit_sikp_flag = 0;
						cap_index = 0;
					}
				}
			}
			else
			{
				cap_index ++;
				if(cap > 350 && cap < 600)
				{
					if(cap_index <= 50)
					{//找到第一个有效的起始位
						first_start_bit_flag = 1;
						start_bit_valid_flag = 1;
						data_bit_sikp_flag = 0;
						cap_index = 0;
						rx_count = 0;
						rx_count_req = 3;
					}
					else
					{
						sync_flag = 0;
					}
				}
			}
		}
		else
		{
			if(cap > 150 && cap < 350)
			{
				cap_index ++;
				if(cap_index >= 22)
				{//找到有效的同步头
					sync_valid_flag = 1;
					first_start_bit_flag = 0;
				}
			}
			else
			{
				sync_flag = 0;
			}
		}
	}
	else
	{
		if(cap > 150 && cap < 350)
		{//找到同步头起始
			sync_flag = 1;
			sync_valid_flag = 0;
			cap_index = 1;
		}
	}
}
//--------------------------------------------------------------------
void Timer2Capture_Init(void)
{
	//timer2 cap
	//P1[5]
	set_P0M1_1;clr_P0M2_1;
	P01 = 1;
	
	T2CON = BIT2;   //TR2 = 1;
	T2MOD = (2 << 4) | BIT3 | (1);    //16M/16  = 1us  CAP0
	
	CAPCON1 = (2);   //Rising_Falling
	CAPCON2 = BIT4;  //CAP0 Noise Filter Enable
	CAPCON3 = (5);   //CAP0 Input->P0[1]
	CAPCON0 = BIT4;  //CAP0 ENABLE
	
	clr_ET2;
	set_ECAP;
}
//====================================================================


//====================================================================
//PWM
static void PWM_DEAD_TIME_VALUE(UINT16	DeadTimeData)
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

//duty: 占空比 10 - 50
//只有frq == WPC_QI_MAX_FREQ, duty才有效
static void Pwm_SetFrq(uint32_t frq, u8 duty)
{
	uint16_t reg, duty_tmp;
  
	qi_tx_freq = (frq > WPC_QI_MAX_FREQ) ? WPC_QI_MAX_FREQ : frq;
	qi_tx_freq = (frq < WPC_QI_MIN_FREQ) ? WPC_QI_MIN_FREQ : frq;

	if(frq < WPC_QI_MAX_FREQ)
	{
		qi_tx_duty = 50;
	}
	else
	{
		qi_tx_duty = duty;
	}
  //frq set
	reg = 16000000/qi_tx_freq;
	
	if(qi_tx_duty  < 50)
	{
		duty_tmp = reg * qi_tx_duty / 100;
	}
	else
	{
		duty_tmp = reg >> 1;
	}
	
	reg -= 1;
	PWMPH = (uint8_t)(reg >> 8);
	PWMPL = (uint8_t)(reg);
	    
	PWM0H = (uint8_t)(duty_tmp >> 8);
	PWM0L = (uint8_t)(duty_tmp);	
	PWM2H = (uint8_t)(duty_tmp >> 8);
	PWM2L = (uint8_t)(duty_tmp);

	set_PWMRUN;
	while(LOAD);
	set_LOAD;
	
	PMEN &= ~(BIT0 | BIT1 | BIT2 | BIT3);
}

void Pwm_Init(void)
{	
/*-----------------------------------------------------------------------------------------------------------------
	PWM frequency 	= Fpwm/((PWMPH,PWMPL)+1) = (16MHz/8)/(0x7CF+1) = 1KHz (1ms)
	PWM2 high duty 	= PWM2H,PWM2L = 0x01CF = 1/4 PWM period
	PWM0 high duty 	= PWM0H,PMW0L = 0x03CF = 1/2 PWM period
	Dead time 			= 0x1FF <PDTEN.4+PDTCNT[7:0]>/Fsys = 0x1FF/Fsys = 512/16000000 = 32 us (max value)
-----------------------------------------------------------------------------------------------------------------*/
	P12 = 0;
	P11 = 0;
	P10 = 1;
	P00 = 1;
	clr_P0M1_0;set_P0M2_0;
	clr_P1M1_0;set_P1M2_0;
	clr_P1M1_1;set_P1M2_1;
	clr_P1M1_2;set_P1M2_2;
	
// 	PWM0_P12_OUTPUT_DISABLE;
// 	PWM1_P11_OUTPUT_DISABLE;
// 	PWM2_P10_OUTPUT_DISABLE;
// 	PWM3_P00_OUTPUT_DISABLE;
	
	PWM2_OUTPUT_INVERSE;
	PWM3_OUTPUT_INVERSE;
	
	PMD &= ~(BIT0 | BIT1 | BIT2 | BIT3);
	PMEN |= (BIT0 | BIT1 | BIT2 | BIT3);
	
	PWM0_P12_OUTPUT_ENABLE;
	PWM1_P11_OUTPUT_ENABLE;
	PWM2_P10_OUTPUT_ENABLE;
	PWM3_P00_OUTPUT_ENABLE;
	
	PWM_COMPLEMENTARY_MODE;					//Only this mode support deat time function
	PWM_CLOCK_DIV_1; 
  PWM_GP_MODE_ENABLE;
	
	PWM01_DEADTIME_ENABLE;
	PWM23_DEADTIME_ENABLE;
	PWM_DEAD_TIME_VALUE(6);			//value never over 0x1FF
}

void Pwm_Turn_Off(void)
{
	PMEN |= (BIT0 | BIT1 | BIT2 | BIT3);
	while(LOAD);
	clr_LOAD;
	clr_PWMRUN;
}
//====================================================================

//====================================================================
//功率控制的PID算法
//返回控制频率
//duty: 占空比
//返回0表示不调整
#define KP   1.0
#define KI   0.0
#define KD   0.0
static bit QiPid(s8 error, u32 * freq, u8 * duty)
{
	u16 coil_current;
  u32	current;
	coil_current = GetCoilCurrent();
	
// 	if(error < 0)
// 	{
// 		current = coil_current;
// 	}
	current = (coil_current) * (1.0f + (error / 128.0f));
// 	current = (coil_current) * (100 + (error * 100 / 128)) / 100;
	
	*duty = 50;
	if(current == coil_current) 
	{
		*freq = qi_tx_freq;
		return (0);
	}
// 	else
// 	if(current <  coil_current) 
// 	{
// 		coil_current = coil_current;
// 	}
	
	*freq = qi_tx_freq - (current - coil_current) * 100;
	if(*freq >= WPC_QI_MAX_FREQ) 
	{
		*freq = WPC_QI_MAX_FREQ;
		if(qi_tx_duty > 10)
		{
			*duty = qi_tx_duty - 1;
		}
		else
		{
			*duty = 10;
		}
	}
	
	return (1);
}
//====================================================================
static void QiDelayMs(u16 ms)
{
	qi_wait_ms = ms;
	while(qi_wait_ms --)
	{
		TH1 = (u8)((65536 - 1335) >> 8);
		TL1 = (u8)( 65536 - 1335);
		TF1 = 0;
		TR1 = 1;    //start timer1
		while(!TF1);
	}
}
//====================================================================
static void QiPing(void)
{
	if(qi_dly_tick) return;
	qi_dly_tick  = SYSTEM_TICK - 10;
	
	Pwm_SetFrq(WPC_QI_INIT_FREQ, 0);
	QiDelayMs(70);
	if(QiPowerChargeState == State_Charge_Ping)
	{//如果PING成功了,不关闭PWM
		Pwm_Turn_Off();
	}
}
//====================================================================
static void QiConfig(void)
{
}
//====================================================================
static void QiTransfer(void)
{
	u32 freq;
	u8 duty;
	if(rx_control_error_flag)
	{
		rx_control_error_flag = 0;
		if(QiPid(rx_control_error, &freq, &duty))
		{
			Pwm_SetFrq(freq, duty);
		}
	}
}
//====================================================================
static void QiTransferFull(void)
{
	if(qi_tx_freq != WPC_QI_MAX_FREQ && qi_tx_duty != 10)
	{//充满后,以最小功率运行,直到设备移开
		Pwm_SetFrq(WPC_QI_MAX_FREQ, 10);
	}
}
//====================================================================
static void QiTransferRePing(void)
{
	Pwm_Turn_Off();
	QiPowerChargeState = State_Charge_Stanby;
	qi_dly_tick = SYSTEM_TICK * 2;
}
//====================================================================
static void QiTransferError(void)
{
	Pwm_Turn_Off();
	QiPowerChargeState = State_Charge_Stanby;
	qi_dly_tick = SYSTEM_TICK * 2;
}
//====================================================================
void WPC_Qi(void)
{
	switch(QiPowerChargeState)
	{
		case State_Charge_Stanby:
			if(qi_dly_tick) break;
			QiPowerChargeState = State_Charge_Ping;
			break;
		case State_Charge_Ping:
			QiPing();
			break;	
		case State_Charge_Config:
			QiConfig();
			break;
		case State_Charge_On:
			QiTransfer();
			break;
		case State_Charge_Full:
			QiTransferFull();
			break;
		case State_Charge_RePing:
			QiTransferRePing();
			break;
		case State_Charge_OverTemper:
		case State_Charge_Err:
			QiTransferError();
			break;
		default:
		{
			Pwm_Turn_Off();
			QiPowerChargeState = State_Charge_Stanby;
		  qi_dly_tick = SYSTEM_TICK;
		}
			break;
	}
	
	
	if(qi_rx_cmd_timeout || (QiPowerChargeState <= State_Charge_Ping)) return;
	
	{
		Pwm_Turn_Off();
		QiPowerChargeState = State_Charge_Stanby;
		qi_dly_tick = 1;
	}
}
//====================================================================
void QiInit(void)
{
	Pwm_Init();
	Timer2Capture_Init();
	
	//Timer1
  clr_T1M;        //time1 clk = Fsys/12
	TMOD |= BIT4;   //16bit timer mode
}
//====================================================================
//end files
