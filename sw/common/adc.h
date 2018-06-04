

#ifndef __ADC_H__
#define __ADC_H__

#include "includes.h"

//实际数据为温度值 +40度
//此片定义两个宏来做数据转换
#define MIN_TEMP_LIMIT        (-40)               //系统可计算的最低温度值
#define TEMP_2_UINT(temp)     ((u8)((temp) - MIN_TEMP_LIMIT))
// #define UINT_2_TEMP(u_int)    ((s8)(u_int + MIN_TEMP_LIMIT))

#define TEMP_SENSOR_OPEN      ((u8)-1)    //传感器开路
#define TEMP_SENSOR_SHORT     ((u8)-2)    //传感器短路
#define TEMP_HIGH_LIMIT       69          //温度上限,超过此温度认为是温度过高
#define TEMP_LOW_LIMIT        5           //温度下限,低于此温度认为是温度过低

#define ADC_CH_REF             0     //参考通道
#define ADC_CH_TEMP            1     //温度
#define ADC_CH_CURRENT         2		 //电流
#define ADC_CH_INPUT_VOLGATE   3     //输入电压


//获取指定通道当前的AD值
u16 GetAd(u8 ch);

//线圈电流 mA
u16 GetCoilCurrent(void);

void InitAdc(void);

#endif
