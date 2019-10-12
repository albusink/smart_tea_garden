#ifndef __ESP8266_H
#define __ESP8266_H

#include "main.h"
#include "usart.h"
#include "stm32l1xx_hal_uart.h"
#include <string.h>
#include <stdio.h>

extern uint8_t num;//执行结果的回车数目

uint8_t send_cmd(uint8_t * cmd, uint8_t * rev, uint16_t wait_time, uint8_t n);
uint8_t esp_init();
//全程就一个初始化函数：1检测硬件是否在线，进入STA、TCP 客户端、透传
//然后就直接往串口发数据就可以了



#endif