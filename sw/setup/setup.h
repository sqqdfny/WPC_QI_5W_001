

#ifndef __SETUP_H__
#define __SETUP_H__
#include "includes.h"

//保存数据的延时
//为了减少写FLASH次数,每次修改参数后,延时一段时间再写入FLASH,
//避免将快速调节参数时的中间值写入FLASH
//单位为 1/SYSTEM_TICK
#define SAVE_SETUP_DLY           200      

#define IAP_USER_CONFIG_ADDR     0x4400
//===================================================================

//===================================================================
//每个数据块8字节,超过8bytes的参数可以分配连续的多块,但不能越过页边界
//每个页 128B = 8 * 16 
//共有 8页 存储配置数据  8 * 16 = 128个配置项
typedef enum
{
  SETUP_INDEX_CONFIG = 0,     			//工作模式  
}enumSetupIndex;

//保存一个参数项
//len->max==8
#define SetupSavePara_Chip(index, p, len) SetupSavePara_Chip_Addr(IAP_USER_CONFIG_ADDR + (((u16)index) << 3), p, len)

//读取一个参数项
//len->max==8
#define SetupReadPara_Chip(index, p, len) SetupReadPara_Chip_Addr(IAP_USER_CONFIG_ADDR + (((u16)index) << 3), p, len)

//addr->
//p   ->
//len ->  max=8
void SetupSavePara_Chip_Addr(u16 addr, void * p, u8 len);
void SetupReadPara_Chip_Addr(u16 addr, void * p, u8 len);


//===================================================================
#endif  //__SETUP_H__

//===================================================================

//end files

