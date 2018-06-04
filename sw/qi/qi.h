


#ifndef __QI_H__
#define __QI_H__

#include "includes.h"


//A11

#define WPC_QI_MAX_FREQ      205000
#define WPC_QI_INIT_FREQ     185000 //185000 //175000
#define WPC_QI_MIN_FREQ	     112000 // 112000

#define WPC_LOW_POWER_FREQ_CTRL  0  

#define WPC_QI_MAX_POWER        30 //*0.5
#define MAX_TIME                50000

#define TX_MAJOR_VER             12
#define TX_MINOR_VER             1
#define TX_MANUFAC_CODE          0xffff

#define TX_POWER_CLASS          0
#define TX_GUARANTEED_POWER     30 //*0.5
#define TX_POTENTIAL_POWER      30 //*0.5
#define TX_NOT_RES_SENS         0


//Power Transmitter timing in the ping phase
#define MAX_PING_TIME                  270    //70
#define MAX_TERMINATE_TIME             28   //power signal termination time
#define MAX_FIRST_TIME                 20   //first packet time out
#define MAX_EXPIRE_TIME                175   //power signal expiration time

//Power Transmitter timing in the identification & configuration phase
#define MAX_NEXT_TIME                   125   //25
#define MAX_PACKET_TIME                 170 

//Power Transmitter timing in the negotiation phase
#define MAX_NEGOTIATE_TIME                   250 


//Power Transmitter timing in the power transfer phase
#define MAX_POWER_TIME                 24000 
#define MAX_CONTROL_TIME               1800 


//Power Transmitter Identification Packet (0x30)
#define QI_TX_MAJOR_VER  1
#define QI_TX_MINOR_VER  2
#define QI_TX_MANUFACTURE_CODE 0x5555

//Power Transmitter Capability Packet (0x31)
#define QI_TX_POWER_CLASS   0x00
#define QI_TX_POWER_GPV       //Guaranteed Power Value
#define QI_TX_POWER_PPV       //Potential Power Value
#define QI_TX_POWER_NRS      //Not Res Sens


#define UCID0                       0x3036414c
#define UCID1                       0xff4e5653
#define UCID2                       0xffffffff
#define UCID3                       0x1f1f0024

//#define HALF_PERIOD_VALUE           256
//#define ONE_PERIOD_VALUE            512
//#define MIN_HALF_PERIOD             200
//#define MAX_ONE_PERIOD              550

#define HALF_PERIOD_VALUE           4000//250
#define ONE_PERIOD_VALUE            8000//500
#define MIN_HALF_PERIOD             1600//100
#define MAX_ONE_PERIOD              10400//650

#define RESPONSE_ACK                  0xFF
#define RESPONSE_NAK                  0x00
#define RESPONSE_ND                    0x55

//#define TX_Q_DETECT      1

#define TX_POWER_A          0.60        //0.64
#define TX_POWER_B           -1727   // -326
#define TX_POWER_P_LOSS     350  //350
#define TX_POWER_OTP_V      3845
#define TX_POWER_MAX        10000


typedef enum
{
		State_Charge_Stanby = 0,
    State_Charge_Ping,	
	  State_Charge_Config,
    State_Charge_On,
    State_Charge_Full,
	  State_Charge_RePing,
    State_Charge_OverTemper,
		State_Charge_Err,
}QI_POWER_CHARGE_STATE_TYPEDEF;
//==========================================================


//==========================================================
void QiTimeTickHook(void);
//µ±Ç°³äµç°å×´Ì¬
extern QI_POWER_CHARGE_STATE_TYPEDEF GetCurChargeState(void);



extern void WPC_Qi(void);
extern void QiInit(void);
//==========================================================
#endif    //__QI_H__
//==========================================================
//end files
