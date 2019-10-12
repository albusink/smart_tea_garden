//需要一个能读能写的串口来和esp通信

#include "esp8266.h"
uint16_t  BufSize = BUFSIZE; 
uint32_t  Timeout = 1000; 
uint8_t num = 0;//执行结果的回车数目

uint8_t cmd[BUFSIZE] = "AT\r\n";

uint8_t esp_init()
{
	if(send_cmd(cmd, "OK", 100, 3) == 1)
		return 1;
	else
		return 0;
}


//在wait_time时间内判断标志位UART2_RX_STA以判断是否接收完成，然后与rev比较即可
//回调函数负责标志位的置位 
//num: 执行结果的回车数目
uint8_t send_cmd(uint8_t * cmd, uint8_t * rev, uint16_t wait_time, uint8_t n)
{
	UART2_RX_STA = 0;
	num = n;
	int i = 0;
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
	while(wait_time--)
	{
		HAL_Delay(1);
		if(UART2_RX_STA == 1)
		{	
			for(i = 0; i < strlen((char const*)pRxData); i++)
			{
				printf("-%d", pRxData[i]);//-65-84-13-10-13-10-79-75-13-10  三对\r\n
			}
			printf("1 Last pRxData ：-%s-\r\n", pRxData);
			if(pRxData[strlen((char const*)pRxData) -4] == 'O' && pRxData[strlen((char const*)pRxData) -3] == 'K')
				return 1;
			else
				return 0;
		}
	}

	return 0;//超时
}

