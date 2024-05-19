#include "GPIO.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOF时钟
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                 //推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;              //不上拉
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOB, GPIO_Pin_2,Bit_SET);
}

void LED_On(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_2,Bit_RESET);
}

