#include "stm32f4xx.h"
#include "GPIO.h"
#include "delay.h"
#include "TIM.h"
#include "Key.h"
#include "ds18b20.h"
#include "OLED.h"

enum DS18B20_SendDATA_Mode
{
    DS18B20_SendData_Start,
    DS18B20_SendData_SendData_Int,
    DS18B20_SendData_SendData_Float,
    DS18B20_SendData_Stop,
}DS18B20_SendData_Mode;
void LED_SendData(uint_fast16_t KEY_State);
float DS18B20_Wendu;
uint_fast16_t Keynum;
int main(void)
{
    delay_init(72);
    DS18B20_Init();
    OLED_Init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);              //NVIC初始化
    TIMER2_Init();
    TIMER3_Init();
    Key_Init();
    while(1)
    {
        DS18B20_Wendu = DS18B20_GetTemperture(0.625);
        OLED_Show_Float(0,0,DS18B20_Wendu,2,12); //
        OLED_Refresh();
		Keynum = Key();
        if(!Keynum)
        {
            switch(Keynum)
            {
                case 1:
                {
                    LED_SendData(1);
                    Keynum = 0;
                    break;
                }
                case 2:
                {
                    LED_SendData(2);
                    Keynum = 0;
                    break;
                }
                case 3:
                {
                    LED_SendData(3);
                    Keynum = 0;
                    break;
                }
                case 4:
                {
                    LED_SendData(4);
                    Keynum = 0;
                    break;
                }
            }
        }
    }
}

void TIM3_IRQHandler(void)
{
    static uint_fast16_t T0Count = 0;
    if(TIM_GetITStatus(TIM3, TIM_IT_Update)!= RESET)
    {
        T0Count++;
		if(T0Count%20==0)
		{
			Key_Loop();
			T0Count = 0;
		}
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

void LED_SendData(uint_fast16_t KEY_State)
{
    uint16_t i = 0;
    while(DS18B20_SendData_Mode != DS18B20_SendData_Stop);                    //防止跟发送温度冲突
    TIM_Cmd(TIM4,DISABLE);                   //关闭发送温度定时器
    TIM_Cmd(TIM3,DISABLE);                   //关闭按键中断
    TIM_Cmd(TIM2,DISABLE);                   //0
    delay_ms(10);
    TIM_Cmd(TIM2,ENABLE);                    //1
    delay_ms(10);
    TIM_Cmd(TIM2,DISABLE);                   //0
    delay_ms(10);
    for(i = 0; i < KEY_State;i++)
    {
        TIM_Cmd(TIM2,DISABLE);
        delay_ms(10);
    }
    TIM_Cmd(TIM2,ENABLE);
    TIM_Cmd(TIM3,ENABLE);
    TIM_Cmd(TIM4,ENABLE);                   //关闭发送温度定时器
}

void TIM4_IRQHandler(void)                 //发送温度
{
    static uint8_t count = 0;
    static uint8_t i = 0;
    static uint16_t Temp = 0;
     if(TIM_GetITStatus(TIM4, TIM_IT_Update)!= RESET)
    {
        switch(DS18B20_SendData_Mode)
        {
            case DS18B20_SendData_Start:
            {
                switch(count)
                {
                    case 0:{TIM_Cmd(TIM2,DISABLE);count++;break;}
                    case 1:{TIM_Cmd(TIM2,DISABLE);count++;break;}
                    case 2:{TIM_Cmd(TIM2,DISABLE);count++;DS18B20_SendData_Mode = DS18B20_SendData_SendData_Int;count = 0;break;}
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
                    TIM_Cmd(TIM2,ENABLE);
                }
                else
                {
                    TIM_Cmd(TIM2,DISABLE);
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
                    Temp = (uint16_t)(DS18B20_Wendu*1000)%1000;
                }
                if(Temp & (0x01<<i))
                {
                    TIM_Cmd(TIM2,ENABLE);
                }
                else
                {
                    TIM_Cmd(TIM2,DISABLE);
                }
                if(i >= 4)
                {
                    DS18B20_SendData_Mode = DS18B20_SendData_Stop;
                    i = 0;
                }
            }
            case DS18B20_SendData_Stop:
            {
                break;
            }
        }
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}



