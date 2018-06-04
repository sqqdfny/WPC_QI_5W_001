//#include "stm8s003k3.h"
//#include "stdlib.h"
//#include "stdio.h"
//#include "string.h"

#ifndef NULL
	#define NULL 	(0UL)
#endif

#define __ASM            _asm                                      /*!< asm keyword for ARM Compiler          */
//#define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */
//#define __STATIC_INLINE  static __inline

#define SYSTEM_TICK            100                           //о╣мЁ╫зед


#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

typedef bit                   BIT;
typedef unsigned char         UINT8;
typedef unsigned int          UINT16;
typedef unsigned long         UINT32;
typedef unsigned char         uint8_t;
typedef unsigned int          uint16_t;
typedef unsigned long         uint32_t;
typedef char                  int8_t;

typedef char               s8;
typedef unsigned char      u8;
typedef short int          s16;
typedef unsigned short int u16;
typedef long int           s32;
typedef unsigned long int  u32;
typedef               bit  bool;
#define false              0
#define true               1
// typedef enum
// {
// 	false = 0,
// 	true = !false
// }bool;


#define BIT0     0x01
#define BIT1     0x02
#define BIT2     0x04
#define BIT3     0x08
#define BIT4     0x10
#define BIT5     0x20
#define BIT6     0x40
#define BIT7     0x80



//comm pin
#define SetSda()       (PB_ODR |=  BIT5) 
#define ClrSda()       (PB_ODR &= ~BIT5)
#define GetSda()       (PB_IDR &   BIT5)
#define SetSdaOutput() (PB_DDR |=  BIT5) 
#define SetSdaInput()  (PB_DDR &= ~BIT5) 

#define SetSck()  (PC_ODR |=  BIT3) 
#define ClrSck()  (PC_ODR &= ~BIT3)     		

#endif

