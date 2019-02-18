#include "sysclock.h"
#include "stm32f10x_rcc.h"
/*
*	�޸�ϵͳʱ��
*divʱ��Դ��Ƶ����
*pllm��Ƶ����
*/
void RCC_HSE_Config(uint32_t div, uint32_t pllm)
{
	RCC_DeInit();								//��λRCC�Ĵ���
	RCC_HSEConfig(RCC_HSE_ON);	//select�ⲿ������HSE
	
	if(SUCCESS == RCC_WaitForHSEStartUp()){	//�ȴ���������
		RCC_PCLK1Config(RCC_HCLK_Div2);				//���õ���AHBʱ�ӣ�PCLK1��
		RCC_PCLK2Config(RCC_HCLK_Div1);				//���ø���AHBʱ�ӣ�PCLK1��
		RCC_PLLConfig(div,pllm);							//����PLLʱ��Դ����Ƶϵ��
		RCC_PLLCmd(ENABLE); 									//ʹ��PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);//���ָ����RCC��־λ�������,PLL����
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				//����ϵͳʱ�ӣ�SYSCLK��
		while(RCC_GetSYSCLKSource()!=0x08);								//��������ϵͳʱ�ӵ�ʱ��Դ,0x08��PLL��Ϊϵͳʱ��
	}
	
	return ;
}

