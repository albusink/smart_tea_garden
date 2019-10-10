//DHT11内部已上拉

//数据格式：40位：8位湿度整数 + 8位湿度小数 + 8位温度整数 + 8位温度小数 + 8为校验和

#include "stm32l1xx.h"
#include "stm32l1xx_it.h"
#include "stm32l1xx_hal_rcc.h"
#include "hal_interface.h"
#include "dth11.h"

#define u8 unsigned char
#define u16 unsigned int

u8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
u8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;

#define BOOL unsigned char
	
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static void DHT11_DataPin_Configure_Output(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    __GPIOA_CLK_ENABLE();    							//使能GPIOA时钟              
    GPIO_InitStructure.Pin = GPIO_PIN_2;             	//PA^2   ant:A0  
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;      //推挽输出
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}
 
static void DHT11_DataPin_Configure_Input(void)
{
	GPIO_InitTypeDef  DataPin;         
    DataPin.Pin = GPIO_PIN_2;             	//PA^2   ant:A0
    DataPin.Mode = GPIO_MODE_IT_FALLING;         //  输入模式
    DataPin.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &DataPin);
}

BOOL DHT11_get_databit(void)
{
    uint8_t val;    
    val = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
	printf("%d\r\n", val);
    if(val == 0){
        return FALSE;
    }else{
        return TRUE;
    }
}

 
void DHT11_set_databit(BOOL level)
{
    if(level == TRUE){
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    }else{
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    }
}

void mdelay(u16 ms)  
{
    if(ms != 0){
        delay(ms);
    }
}


void udelay(u16 us)  
{
	us *= 2;
    while(us--);			//1us
}
//===============延时函数=====================
void Delay10us()		//@12.000MHz
{
	unsigned char i;

	i = 38;
	while (--i);
}
void Delay20ms()		//@12.000MHz
{
	unsigned char i, j, k;

	i = 2;
	j = 56;
	k = 172;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
//===============延时函数=====================

static uint8_t DHT11_read_byte(void)

{     
    uint8_t i;  
    uint8_t data = 0;                        
    for(i = 0; i < 8; i++)       
    {    
        data <<= 1;      
        while( !DHT11_get_databit() );	//跑过前面50us的低电平
//        udelay(10);
//        udelay(10);
//        udelay(10);						//延时30us，如果再次读取还是1，则传输的数据是1，否则不处理直接移位为0
		Delay10us();
		Delay10us();
		Delay10us();
		
        if(DHT11_get_databit())
		{
            data |= 0x01;
			//printf("y\r\n");
            while( DHT11_get_databit() );//一直是高？？？
			//printf("z\r\n");
        }
     }
	 //printf("%d \r\n", data);
     return data;
}

  
static uint8_t DHT11_start_sampling(void)
{
	//DHT11 数据引脚配置 --输出
    DHT11_DataPin_Configure_Output();
	
	DHT11_set_databit(TRUE);		//数据引脚置1
    DHT11_set_databit(FALSE);		//数据引脚置零
//    mdelay(20);						//延时20ms
	Delay20ms();
    DHT11_set_databit(TRUE);		//数据引脚置1
     

//	udelay(10);
//	udelay(10);
//  udelay(10);//延时30us
	Delay10us();
	Delay10us();
	Delay10us();


    DHT11_DataPin_Configure_Input();
	DHT11_set_databit(TRUE);		//数据引脚置1

    if( !DHT11_get_databit() )         //DHT11 响应并拉低
    { 
		while( !DHT11_get_databit() );  //DHT11 响应并拉高
		
		while( DHT11_get_databit() );  //DHT11 响应并拉低
//		printf("haha\r\n");
		return 1;
    }
    return 0;               
}

 
void DHT11_get_data(void)
{      
    u8 temp;
    if(DHT11_start_sampling())
	{
//        printf("DHT11 is ready to transmit data\r\n");
        //读取温湿度数据      
		U8RH_data_H_temp = DHT11_read_byte();
		printf("oooo\r\n");
        U8RH_data_L_temp = DHT11_read_byte();
        U8T_data_H_temp     = DHT11_read_byte();
        U8T_data_L_temp     = DHT11_read_byte();
        U8checkdata_temp = DHT11_read_byte();
		printf("e\r\n");
		printf("%d, %d, %d, %d\r\n", U8RH_data_H_temp,U8RH_data_L_temp,U8T_data_H_temp,U8T_data_L_temp);
		
        /* Data transmission finishes, pull the bus high */
        DHT11_DataPin_Configure_Output();   
        DHT11_set_databit(TRUE);
		
        //校验和
        temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
        if(temp==U8checkdata_temp)
        {
            U8RH_data_H=U8RH_data_H_temp;
            U8RH_data_L=U8RH_data_L_temp;
            U8T_data_H=U8T_data_H_temp;
            U8T_data_L=U8T_data_L_temp;
            U8checkdata=U8checkdata_temp;
            printf("DHT11 tempature %d.%d  humidity %d.%d \r\n",
				U8T_data_H,
                U8T_data_L,
                U8RH_data_H,
                U8T_data_L);
        }else{
            printf("checksum failure \r\n");
        }
    }else{
        printf("DHT11 didn't answer. Sampling failed. \r\n");
    }
}

void DTH11_test()
{
//       DHT11_get_data();
       delay(1000);    
}

 
