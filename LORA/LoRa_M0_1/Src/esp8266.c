//��Ҫһ���ܶ���д�Ĵ�������espͨ��

#include "esp8266.h"
uint16_t  BufSize = BUFSIZE; 
uint32_t  Timeout = 1000;  

uint8_t cmd[BUFSIZE] = "AT\r\n";

uint8_t esp_init()
{
	if(send_cmd(cmd, "OK", 1000) == 1)
		return 1;
	else
		return 0;
}


//��wait_timeʱ�����жϱ�־λUART1_RX_STA���ж��Ƿ������ɣ�Ȼ����rev�Ƚϼ���
//�ص����������־λ����λ
uint8_t send_cmd(uint8_t * cmd, uint8_t * rev, uint16_t wait_time)
{
	UART1_RX_STA = 0;
	HAL_UART_Transmit(&huart2, cmd, BufSize, 0xffffff);
	while(wait_time--)
	{
		if(UART1_RX_STA == 1)
		{
			if( strncmp((char const*)pRxData, (char const*)rev, strlen((char const*)rev)) == 0)
				return 1;
		}
	}
	return 0;//��ʱ
}

