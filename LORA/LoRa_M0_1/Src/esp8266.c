//��Ҫһ���ܶ���д�Ĵ�������espͨ��

#include "esp8266.h"
uint16_t  BufSize = BUFSIZE; 
uint32_t  Timeout = 1000; 
uint8_t num = 0;//ִ�н���Ļس���Ŀ

uint8_t cmd[BUFSIZE] = {0};

uint8_t esp_init()
{
	//�رջ���
/*	strcpy(cmd, "ATE0\r\n");
	if(send_cmd(cmd, "OK") != 1)
	{
		printf("�رջ���ʧ��\r\n");
		return 0;
	}*/
	//AT
	strcpy(cmd, "AT\r\n");
	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("��������ESP8266......\r\n");
	}
	HAL_Delay(100);
	
	//����Ϊ STA ģʽ
	strcpy(cmd, "AT+CWMODE=1\r\n");
	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("��������Ϊ STA ģʽ......\r\n");
	}
	HAL_Delay(1000);
	//����
	//����֮�󷵻ص��ַ�����û��OK
	//������֮����Զ�������һ�����ӵ��Ǹ�WiFi���м�¼��
	//A:ֱ�ӷ���һ�Σ��ȴ�ʱ�䳤һ��
	strcpy(cmd, "AT+RST\r\n");
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
	
/*	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("��������......\r\n");
	}
*/
	HAL_Delay(2000);
	//����·����
	//һ�㰴����һ�δ洢��WiFi�������ӣ���Ҫ�ĵĻ���ֻ��Ҫ����һ�Σ�����Ҫ����5s����
	//A:ֱ�ӷ��ͣ�Ȼ���5s
	strcpy(cmd, "AT+CWJAP=\"AlbusInk\",\"AlbusInk\"\r\n");//AT+CWJAP="AlbusInk","AlbusInk"
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
/*	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("��������·����......\r\n");
	}
*/	
	HAL_Delay(5000);
	
	//ģ�����ӵ� server
	//���ӳɹ����е����룬���������OK�����������ַ���Ӧ���Ǵ����Ǳ����Ļ��ǲ���
	//A:ֱ�ӷ���
	strcpy(cmd, "AT+CIPSTART=\"TCP\",\"192.168.43.125\",12345\r\n");
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
/*	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("��������server......\r\n");
	}
*/
	HAL_Delay(1000);
	//����͸��ģʽ
	//��ѭ��Ҳ�ǲ��У����Ǵ����Ǳߵ������ˣ��жϲ�����OK
	//д������Ĵ��ڰ�д�ˣ�����������������������������
	strcpy(cmd, "AT+CIPMODE=1\r\n");
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
/*	while(!send_cmd(cmd, "OK"))
	{
		HAL_Delay(1000);
		printf("���Կ���͸��ģʽ......\r\n");
	}
*/
	HAL_Delay(1000);
	return 1;
}

//˵�� ��wait_time�����Ȳ����ˣ�����Ҫ���Ч�ʸ�����˵���õĻ����Ա�֤����ʵʱ��
//��ߵ�whileѭ���ʹ����жϽ��ս���ִ��,������Ҫ��Ӻܶ����

//��wait_timeʱ�����жϱ�־λUART2_RX_STA���ж��Ƿ������ɣ�Ȼ����rev�Ƚϼ���
//�ص����������־λ����λ 
//num: ִ�н���Ļس���Ŀ
uint8_t send_cmd(uint8_t * cmd, uint8_t * rev)
{
	HAL_UART_Transmit(&huart2, cmd, strlen((char const*)cmd), 100);
	HAL_Delay(10);//115200 

	pRxData[UART2_Rx_Index] = '\0';
	UART2_Rx_Index = 0;
	printf("-%s-\r\n", pRxData);
	//����ESP���ص�ֵ���ң�����ֱ�Ӿ�ȷ�ж��ǲ�����
	//A:�ж�pRxData���Ƿ���OK�Ӵ�������
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

