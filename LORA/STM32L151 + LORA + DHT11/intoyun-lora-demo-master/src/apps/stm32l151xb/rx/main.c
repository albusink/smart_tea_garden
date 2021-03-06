#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32l1xx.h"
#include "init.h"
#include "radio.h"
#include "sx1276-board.h"
#include "hal_interface.h"


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

#define BUFFER_SIZE   8 // Define the payload size here

static uint16_t bufferSize = BUFFER_SIZE;// RF buffer size
static uint8_t buffer[BUFFER_SIZE] = {0};// RF buffer


bool ledFlag = false;
int8_t snr = 0;
int16_t rssi = 0;

//======================��������==============================
void LED_GPIO_Init(void);
void LedBlink(void);
void OnTxDone( void );
void OnRxDone( uint8_t *payload, uint16_t size, int16_t _rssi, int8_t _snr );
void OnTxTimeout( void );
void OnRxTimeout( void );
void OnRxError( void );
//============================================================

int main(void)
{
    SystemInitialize();
    LED_GPIO_Init();
    delay(100);
    printf("radio RX Mode\r\n");

    SX1276BoardInit();

    // Radio initialization
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;

    Radio.Init( &RadioEvents );

    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetModem( MODEM_LORA );

    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                       LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                       LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                       true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                       LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                       LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                       0, true, 0, 0, LORA_IQ_INVERSION_ON, 1 );

    printf("version = 0x%x\r\n",SX1276GetVersion());

    Radio.Rx(0);

    while(1)
    {
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
    printf("tx done\r\n");
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t _rssi, int8_t _snr )
{
    bufferSize = size;
    memcpy( buffer, payload, bufferSize );

    rssi = _rssi;
    snr = _snr;

    LedBlink();
    //printf("rssi = %d\r\n",rssi);
    //printf("snr = %d\r\n",snr);
    //printf("rx data = ");
	
	printf(" %c%c%c%d\r\n", buffer[0],buffer[1],buffer[2],buffer[3]);
}

void OnTxTimeout( void )
{
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

