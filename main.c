#include "stm32f4xx.h"
#include "GPIO.h"
#include "delay.h"
#include "TIM.h"
#include "Key.h"
#include "ds18b20.h"
#include "OLED.h"

#define Send_data_Delay 20



enum DS18B20_SendDATA_Mode
{
    DS18B20_SendData_Start,
    DS18B20_SendData_SendData_Int,
    DS18B20_SendData_SendData_Float,
    DS18B20_SendData_Stop,
}DS18B20_SendData_Mode = DS18B20_SendData_Stop;
void LED_SendData(uint_fast16_t KEY_State);
float DS18B20_Wendu;
uint_fast16_t Keynum;
int main(void)
{
    delay_init(168);
    DS18B20_Init();              //B6
    OLED_Init();                //B78
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);              //NVIC³õÊ¼»¯
    TIMER3_Init();   
    TIMER4_Init();       //1s  
    TIMER2_Init();              //A0
    Key_Init();                 //A23456
		DS18B20_Wendu = 20;
    while(1)
    {
			DS18B20_Wendu = DS18B20_Get_Temp();
        OLED_Show_Float(0,0,DS18B20_Wendu,2,12); //
        OLED_Refresh();
		Keynum = Key();
        if(Keynum)
        {
            switch(Keynum)
            {
                case 1:
                {
	//								OLED_ShowNum(32,32,Keynum,2,12,1);
                    LED_SendData(1);
                    Keynum = 0;
                    break;
                }
                case 2:
                {
	//								OLED_ShowNum(32,32,Keynum,2,12,1);
                    LED_SendData(2);
                    Keynum = 0;
                    break;
                }
                case 3:
                {
	//								OLED_ShowNum(32,32,Keynum,2,12,1);
                    LED_SendData(3);
                    Keynum = 0;
                    break;
                }
                case 4:
                {
		//							OLED_ShowNum(32,32,Keynum,2,12,1);
                    LED_SendData(4);
                    Keynum = 0;
                    break;
                }
                case 5:
                {
		//							OLED_ShowNum(32,32,Keynum,2,12,1);
                    LED_SendData(5);
                    Keynum = 0;
                    break;
                }
            }
        }
    }
}

void TIM3_IRQHandler(void)
{
    static uint_fast16_t T0Count = 0, Send_count = 0;
    static uint8_t count = 0;
    static uint8_t i = 0;
    static uint16_t Temp = 0;
    if(TIM_GetITStatus(TIM3, TIM_IT_Update)!= RESET)
    {
        T0Count++;
        Send_count++;
        if(T0Count%20==0)
        {
            Key_Loop();
            T0Count = 0;
        }
        if(Send_count >= Send_data_Delay)
        {
            Send_count = 0;
            switch(DS18B20_SendData_Mode)
            {
                case DS18B20_SendData_Start:
                {
                    switch(count)
                    {
                        case 0:{TIM2->CCR1 = 10;count++;break;}            //1 1 1
                        case 1:{TIM2->CCR1 = 10;count++;break;}
                        case 2:{TIM2->CCR1 = 10;count++;DS18B20_SendData_Mode = DS18B20_SendData_SendData_Int;count = 0;break;}
                    }
                    break;
                }
                case DS18B20_SendData_SendData_Int:
                {
                    if(!i)
                    {
                        Temp = (uint16_t)DS18B20_Wendu;
                    }
                    if(Temp & (0x01<<i))
                    {
                        TIM2->CCR1 = 10;
                    }
                    else
                    {
                        TIM2->CCR1 = 20;
                    }
                    i++;
                    if(i >= 6)
                    {
                        DS18B20_SendData_Mode = DS18B20_SendData_SendData_Float;
                        i = 0;
                    }
                    break;
                }
                case DS18B20_SendData_SendData_Float:
                {
                    if(!i)
                    {
                        Temp = (uint16_t)(DS18B20_Wendu*10)%10;
                    }
                    if(Temp & (0x01<<i))
                    {
                        TIM2->CCR1 = 10;
                    }
                    else
                    {
                        TIM2->CCR1 = 20;
                    }
                    i++;
                    if(i >= 5)
                    {
                        DS18B20_SendData_Mode = DS18B20_SendData_Stop;
                        i = 0;
                    }
                    break;
                }
                case DS18B20_SendData_Stop:
                {
                    
                    TIM2->CCR1 = 20;
                    break;
                }
            }
        }
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

void LED_SendData(uint_fast16_t KEY_State)
{
    uint16_t i = 0;
	
    TIM4->DIER &= (uint16_t)~TIM_IT_Update;
    TIM4->CNT = 0;
    OLED_ShowNum(64,0,DS18B20_SendData_Mode,2,12,1);
    while(DS18B20_SendData_Mode != DS18B20_SendData_Stop)                    //·ÀÖ¹¸ú·¢ËÍÎÂ¶È³åÍ»
    {
        if(TIM4->CNT > 2000)
            break;
    }
    TIM_Cmd(TIM3,DISABLE);
    delay_ms(50);
    TIM2->CCR1 = 10;                   //1
    delay_ms(20);
    TIM2->CCR1 = 20;                    //0
    delay_ms(20);
    TIM2->CCR1 = 10;                   //1
    delay_ms(20);
    for(i = 0; i < KEY_State;i++)
    {
        TIM2->CCR1 = 10;
        delay_ms(20);
    }
    TIM2->CCR1 = 20;
    TIM_Cmd(TIM3,ENABLE);
    TIM4->CNT = 0;
    TIM4->DIER |= TIM_IT_Update;
}

void TIM4_IRQHandler(void)                 //·¢ËÍÎÂ¶È
{
    static uint16_t count = 0;
    if(TIM_GetITStatus(TIM4, TIM_IT_Update)!= RESET)
    {
        OLED_ShowNum(16,16,count++,4,12,1);
				DS18B20_SendData_Mode = DS18B20_SendData_Start;
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}



