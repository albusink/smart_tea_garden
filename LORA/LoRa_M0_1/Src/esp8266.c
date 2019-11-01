//需要一个能读能写的串口来和esp通信

#include "esp8266.h"
uint16_t  BufSize = BUFSIZE; 
uint32_t  Timeout = 1000; 
uint8_t num = 0;//执行结果的回车数目

uint8_t cmd[BUFSIZE] = {0};

uint8_t esp_init()
{
	//关闭回显
/*	strcpy(cmd, "ATE0\r\n");
	if(send_cmd(cmd, "OK") != 1)
	{
		printf("关闭回显失败\r\n");
		return 0;
	}*/
	//AT
	strcpy(cmd, "AT\r\n");
	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("尝试连接ESP8266......\r\n");
	}
	HAL_Delay(100);
	
	//设置为 STA 模式
	strcpy(cmd, "AT+CWMODE=1\r\n");
	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("尝试设置为 STA 模式......\r\n");
	}
	HAL_Delay(1000);
	//重启
	//重启之后返回的字符串中没有OK
	//且重启之后会自动连接上一次连接的那个WiFi（有记录）
	//A:直接发送一次，等待时间长一点
	strcpy(cmd, "AT+RST\r\n");
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
	
/*	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("尝试重启......\r\n");
	}
*/
	HAL_Delay(2000);
	//连接路由器
	//一般按照上一次存储的WiFi进行连接，需要改的话，只需要发送一次，但是要等上5s左右
	//A:直接发送，然后等5s
	strcpy(cmd, "AT+CWJAP=\"AlbusInk\",\"AlbusInk\"\r\n");//AT+CWJAP="AlbusInk","AlbusInk"
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
/*	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("尝试连接路由器......\r\n");
	}
*/	
	HAL_Delay(5000);
	
	//模块连接到 server
	//连接成功后有点乱码，导致最后不是OK，是其他的字符，应该是串口那边做的还是不好
	//A:直接发吧
	strcpy(cmd, "AT+CIPSTART=\"TCP\",\"192.168.43.125\",12345\r\n");
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
/*	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("尝试连接server......\r\n");
	}
*/
	HAL_Delay(1000);
	//开启透传模式
	//用循环也是不行，就是串口那边的问题了，判断不出来OK
	//写了两天的串口白写了？？？？？？？？？？？？？？？
	strcpy(cmd, "AT+CIPMODE=1\r\n");
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
/*	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("尝试开启透传模式......\r\n");
	}
*/
	HAL_Delay(1000);
	return 1;
}

//说明 ：wait_time暂且先不用了，后面要求的效率高了再说，用的话可以保证更加实时：
//这边的while循环和串口中断接收交替执行,但是需要添加很多代码

//在wait_time时间内判断标志位UART2_RX_STA以判断是否接收完成，然后与rev比较即可
//回调函数负责标志位的置位 
//num: 执行结果的回车数目
uint8_t send_cmd(uint8_t * cmd, uint8_t * rev)
{
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
	HAL_Delay(10);//115200 

	pRxData[UART2_Rx_Index] = '\0';
	UART2_Rx_Index = 0;
	printf("-%s-\r\n", pRxData);
	//由于ESP返回的值很乱，所以直接精确判断是不行了
	//A:判断pRxData中是否有OK子串就行了
/*	if(pRxData[strlen((char const*)pRxData) -4] == 'O' && pRxData[strlen((char const*)pRxData) -3] == 'K')
	{
		memset(pRxData, 0, BufSize);
		return 1;
	}
*/
	if(strstr((char const*)pRxData, (char const*)rev) != NULL)
	{
		memset(pRxData, 0, BufSize);
		return 1;
	}
	else
	{
		memset(pRxData, 0, BufSize);
		return 0;
	}

	
}

