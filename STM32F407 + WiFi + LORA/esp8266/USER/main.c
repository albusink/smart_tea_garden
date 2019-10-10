/*
串口1：下载程序、串口调试 输出：printf
串口2：LORA数据接收传入 无输出
串口3：WIFI模块   输出：u3_printf
//	sprintf函数：组合/连接字符串
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

//串口2
extern u16 USART2_RX_STA; 
extern u8 USART2_RX_BUF[USART2_REC_LEN];

u8 i;
int main(void)
{        
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  			//初始化延时函数
	uart_init(115200);			//初始化串口波特率为115200
	usart3_init(115200);  		//初始化串口3波特率为115200
	uart2_init(115200);  //初始化串口2波特率为115200
	LED_Init();					//初始化LED  
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化  
	W25QXX_Init();				//初始化W25Q128
	tp_dev.init();				//初始化触摸屏
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	font_init();				//检查字库是否OK

	
	atk_8266_test();		//进入ATK_ESP8266测试
}













