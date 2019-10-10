//DHT11�ڲ�������

//���ݸ�ʽ��40λ��8λʪ������ + 8λʪ��С�� + 8λ�¶����� + 8λ�¶�С�� + 8ΪУ���

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
    __GPIOA_CLK_ENABLE();    							//ʹ��GPIOAʱ��              
    GPIO_InitStructure.Pin = GPIO_PIN_2;             	//PA^2   ant:A0  
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;      //�������
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}
 
static void DHT11_DataPin_Configure_Input(void)
{
	GPIO_InitTypeDef  DataPin;         
    DataPin.Pin = GPIO_PIN_2;             	//PA^2   ant:A0
    DataPin.Mode = GPIO_MODE_IT_FALLING;         //  ����ģʽ
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
//===============��ʱ����=====================
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
//===============��ʱ����=====================

static uint8_t DHT11_read_byte(void)

{     
    uint8_t i;  
    uint8_t data = 0;                        
    for(i = 0; i < 8; i++)       
    {    
        data <<= 1;      
        while( !DHT11_get_databit() );	//�ܹ�ǰ��50us�ĵ͵�ƽ
//        udelay(10);
//        udelay(10);
//        udelay(10);						//��ʱ30us������ٴζ�ȡ����1�������������1�����򲻴���ֱ����λΪ0
		Delay10us();
		Delay10us();
		Delay10us();
		
        if(DHT11_get_databit())
		{
            data |= 0x01;
			//printf("y\r\n");
            while( DHT11_get_databit() );//һֱ�Ǹߣ�����
			//printf("z\r\n");
        }
     }
	 //printf("%d \r\n", data);
     return data;
}

  
static uint8_t DHT11_start_sampling(void)
{
	//DHT11 ������������ --���
    DHT11_DataPin_Configure_Output();
	
	DHT11_set_databit(TRUE);		//����������1
    DHT11_set_databit(FALSE);		//������������
//    mdelay(20);						//��ʱ20ms
	Delay20ms();
    DHT11_set_databit(TRUE);		//����������1
     

//	udelay(10);
//	udelay(10);
//  udelay(10);//��ʱ30us
	Delay10us();
	Delay10us();
	Delay10us();


    DHT11_DataPin_Configure_Input();
	DHT11_set_databit(TRUE);		//����������1

    if( !DHT11_get_databit() )         //DHT11 ��Ӧ������
    { 
		while( !DHT11_get_databit() );  //DHT11 ��Ӧ������
		
		while( DHT11_get_databit() );  //DHT11 ��Ӧ������
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
        //��ȡ��ʪ������      
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
		
        //У���
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

 
