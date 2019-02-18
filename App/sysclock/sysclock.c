#include "sysclock.h"
#include "stm32f10x_rcc.h"
/*
*	修改系统时钟
*div时钟源分频因子
*pllm倍频因子
*/
void RCC_HSE_Config(uint32_t div, uint32_t pllm)
{
	RCC_DeInit();								//复位RCC寄存器
	RCC_HSEConfig(RCC_HSE_ON);	//select外部高速震荡HSE
	
	if(SUCCESS == RCC_WaitForHSEStartUp()){	//等待晶振起振
		RCC_PCLK1Config(RCC_HCLK_Div2);				//设置低速AHB时钟（PCLK1）
		RCC_PCLK2Config(RCC_HCLK_Div1);				//设置高速AHB时钟（PCLK1）
		RCC_PLLConfig(div,pllm);							//设置PLL时钟源及倍频系数
		RCC_PLLCmd(ENABLE); 									//使能PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);//检查指定的RCC标志位设置与否,PLL就绪
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				//设置系统时钟（SYSCLK）
		while(RCC_GetSYSCLKSource()!=0x08);								//返回用作系统时钟的时钟源,0x08：PLL作为系统时钟
	}
	
	return ;
}

