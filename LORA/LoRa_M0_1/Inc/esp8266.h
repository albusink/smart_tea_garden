#ifndef __ESP8266_H
#define __ESP8266_H

#include "main.h"
#include "usart.h"
#include "stm32l1xx_hal_uart.h"
#include <string.h>
#include <stdio.h>



uint8_t send_cmd(uint8_t * cmd, uint8_t * rev);
uint8_t esp_init();
//ȫ�̾�һ����ʼ��������1���Ӳ���Ƿ����ߣ�����STA��TCP �ͻ��ˡ�͸��
//Ȼ���ֱ�������ڷ����ݾͿ�����



#endif