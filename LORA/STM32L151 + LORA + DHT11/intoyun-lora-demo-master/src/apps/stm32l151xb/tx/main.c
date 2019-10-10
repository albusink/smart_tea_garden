/*
接收的数据通过USART发送出去 （PA9 PA10）
*/


#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.H>
#include "stm32l1xx.h"
#include "init.h"
#include "radio.h"
#include "sx1276-board.h"
#include "hal_interface.h"
#include "dth11.h"


#define RF_FREQUENCY                               435575000 // Hz

#define TX_OUTPUT_POWER                            20        // dBm

#define LORA_BANDWIDTH                             0         // [0: 125 kHz,
                                                             //  1: 250 kHz,
                                                             //  2: 500 kHz,
                                                             //  3: Reserved]

#define LORA_SPREADING_FACTOR                      7         // [SF7..SF12]
#define LORA_CODINGRATE                            1         // [1: 4/5,
                                                             //  2: 4/6,
                                                             //  3: 4/7,
                                                             //  4: 4/8]

#define LORA_PREAMBLE_LENGTH                       8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                        1023      // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                 false
#define LORA_IQ_INVERSION_ON                       false

static RadioEvents_t RadioEvents;

#define LED_PIN           GPIO_PIN_12
#define LED_GPIO_PORT     GPIOB
#define LED_ON            HAL_GPIO_WritePin(LED_GPIO_PORT,LED_PIN, GPIO_PIN_RESET)
#define LED_OFF           HAL_GPIO_WritePin(LED_GPIO_PORT,LED_PIN, GPIO_PIN_SET)
#define LED_TOG           HAL_GPIO_TogglePin(LED_GPIO_PORT,LED_PIN)

#define BUFFER_SIZE   5 // Define the payload size here

static uint16_t bufferSize = BUFFER_SIZE;// RF buffer size
//static uint8_t buffer[BUFFER_SIZE] = {1,2,3,4,5,6,7,8};// RF buffer
static uint8_t buffer[BUFFER_SIZE] = {1,2,3,4,5};// RF buffer

//温湿度数据：
extern u8 U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;


bool ledFlag = false;
int8_t snr = 0;
int16_t rssi = 0;
//======================函数声明==============================
void LED_GPIO_Init(void);
void LedBlink(void);
void OnTxDone( void );
void OnRxDone( uint8_t *payload, uint16_t size, int16_t _rssi, int8_t _snr );
void OnTxTimeout( void );
void OnRxTimeout( void );
void OnRxError( void );
uint8_t getT(void);
uint8_t getH(void);

//===========================================================

uint8_t flag = 1;
uint8_t T = 22;
uint8_t H = 22;
uint8_t I = 3;//1000
//uint8_t C = ;


int main(void)
{
    SystemInitialize();
    LED_GPIO_Init();
    delay(100);
    printf("radio TX Mode\r\n");
    SX1276BoardInit();

    // Radio initialization
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;

	//LORA模块初始化
    Radio.Init( &RadioEvents );

    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetModem( MODEM_LORA );

	//发送端配置
    Radio.SetTxConfig( 	MODEM_LORA, TX_OUTPUT_POWER, 0, 
						LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
						LORA_CODINGRATE, LORA_PREAMBLE_LENGTH, 
						LORA_FIX_LENGTH_PAYLOAD_ON, true, 0, 
						0, LORA_IQ_INVERSION_ON, 3000 );
/*	//接收端配置
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                       LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                       LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                       0, true, 0, 0, LORA_IQ_INVERSION_ON, 1 );
*/
    printf("version = 0x%x\r\n",SX1276GetVersion());


    while(1)
    {
		buffer[0] = 'A';
		//buffer[0] = 'B';
		buffer[2] = '=';
		if(flag == 1)
		{
			T = getT();
			buffer[1] = 'T';
			buffer[3] = T;
		}
		else if(flag == 2)
		{
			H = getH();
			buffer[1] = 'H';
			buffer[3] = H;
			flag = 0;
		}
		
        Radio.Send(buffer, BUFFER_SIZE);
		flag++;
		delay(1000);//延时一下  等待发送数据成功	
		DTH11_test();
        delay(20000);//20s
		
    }
}









void LED_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __GPIOC_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    //GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);
}

void LedBlink(void)
{
    ledFlag = !ledFlag;
    if(!ledFlag){
        LED_OFF;
    }else{
        LED_ON;
    }
}

void OnTxDone( void )
{
    Radio.Sleep( );
    LedBlink();
    printf("tx done\r\n");
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t _rssi, int8_t _snr )
{
    bufferSize = size;
    memcpy( buffer, payload, bufferSize );

    rssi = _rssi;
    snr = _snr;

    printf("rssi = %d\r\n",rssi);
    printf("snr = %d\r\n",snr);
    printf("rx data = ");
    for(uint8_t i = 0; i < bufferSize-1; i++)
    {
        printf("%d ", buffer[i]);
    }
    printf("%d\r\n", buffer[bufferSize]);
}

void OnTxTimeout( void )
{
    Radio.Sleep( );
    printf("tx timeout\r\n");
}

void OnRxTimeout( void )
{
    printf("rx timeout\r\n");
}

void OnRxError( void )
{
    printf("rx error\r\n");
}



uint8_t getT(void)
{
	return 22 + rand()%3;
	//return U8T_data_H;
}
uint8_t getH(void)
{
	return 50 + rand()%5;
}
