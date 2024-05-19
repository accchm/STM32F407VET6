#include "GPIO.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOFʱ��
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                 //�������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;              //������
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOB, GPIO_Pin_2,Bit_SET);
}

void LED_On(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_2,Bit_RESET);
}

