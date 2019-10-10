/*
����1�����س��򡢴��ڵ��� �����printf
����2��LORA���ݽ��մ��� �����
����3��WIFIģ��   �����u3_printf
//	sprintf���������/�����ַ���
*/

#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usmart.h"  
#include "sram.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "usmart.h"	
#include "touch.h"		
#include "usart3.h"
#include "common.h" 

#include "usart2.h"

//����2
extern u16 USART2_RX_STA; 
extern u8 USART2_RX_BUF[USART2_REC_LEN];

u8 i;
int main(void)
{        
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  			//��ʼ����ʱ����
	uart_init(115200);			//��ʼ�����ڲ�����Ϊ115200
	usart3_init(115200);  		//��ʼ������3������Ϊ115200
	uart2_init(115200);  //��ʼ������2������Ϊ115200
	LED_Init();					//��ʼ��LED  
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ��  
	W25QXX_Init();				//��ʼ��W25Q128
	tp_dev.init();				//��ʼ��������
	usmart_dev.init(168);		//��ʼ��USMART
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
  	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	font_init();				//����ֿ��Ƿ�OK

	
	atk_8266_test();		//����ATK_ESP8266����
}













