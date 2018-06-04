/*
	by:sqqdfny 2017-09-30 QQ:792739069  e-mail:sqqdfny@163.com
*/

#include "delay.h"

void DelayTick(u8 delay)
{
	while(delay--);
}

void Delayus(void){
	DelayTick(250);
	DelayTick(250);
	DelayTick(250);
}	
void Delayms(u16 ms){
	while(ms--)
	{
		Delayus();	 
	}
	set_WDCLR;
}

