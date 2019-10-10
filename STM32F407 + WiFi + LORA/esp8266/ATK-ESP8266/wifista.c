#include "common.h"
#include "stdlib.h"
#include "usart2.h"

u8 netpro=0;		//网络模式
//返回值:0：正常；其他：错误代码
u8 atk_8266_wifista_test(void)
{
	u8 key;  		//检测按键
//---------------------------------------------------	
	u8 i;			//数组下标
	extern u16 USART2_RX_STA; 
	extern u8 USART2_RX_BUF[USART2_REC_LEN];	//串口2/LORA输入口缓存和标志变量

//---------------------------------------------------
	//	u8 ipbuf[16] = "192.168.137.19"; 			//IP缓存
	u8 ipbuf[16] = {0};								//赋初值为0，否则IP键盘那里会出现乱码
	u8 *p;
	u16 t=999;										//加速第一次获取链接状态（第一次检查连接状态）
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;									//连接状态
	p=mymalloc(SRAMIN,32);							//申请32字节内存
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	delay_ms(1000);         //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	
//------------------------设置连接到的WIFI网络名称/加密方式/密码 放到AT指令中----------------------------
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//连接目标路由器,并且获得IP
	
PRESTA:
	//选择TCP客户端
	netpro|=1;
	if(netpro&0X01)     //TCP Client  透传模式
	{
		LCD_Clear(WHITE);
		POINT_COLOR=RED;
		Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 测试",16,240); 
		Show_Str(30,50,200,16,"正在配置ATK-ESP模块,请稍等...",12,0);
		if(atk_8266_ip_set("WIFI-STA 远端IP设置",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))
			goto PRESTA;	//IP输入   参数： ip设置标题、工作模式、端口号、IP地址缓存数组
		
		
		atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
		sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器
		while(atk_8266_send_cmd(p,"OK",200))		//检测连接是否成功
		{
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			Show_Str_Mid(0,40,"WK_UP:返回重选",16,240);
			Show_Str(30,80,200,12,"ATK-ESP 连接TCP失败",12,0); //连接失败	 
			key=KEY_Scan(0);
			if(key==WKUP_PRES)goto PRESTA;
		}	
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传			
	}
	
//-------------------发送接收数据界面设计------------------------------------------
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 测试",16,240);
	Show_Str(30,50,200,16,"正在配置ATK-ESP模块,请稍等...",12,0);			
	LCD_Fill(30,50,239,50+12,WHITE);			//清除之前的显示
	Show_Str(30,50,200,16,"WK_UP:退出测试",12,0);
	LCD_Fill(30,80,239,80+12,WHITE);
	
	atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP
	sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);
	Show_Str(30,65,200,12,p,12,0);				//显示IP地址和端口	
	Show_Str(30,80,200,12,"状态:",12,0); 		//连接状态
	Show_Str(120,80,200,12,"模式:",12,0); 		//连接状态
	Show_Str(30,130,200,12,"发送数据:",12,0); 	//发送数据
	Show_Str(30,180,200,12,"接收数据:",12,0);	//接收数据
//	atk_8266_wificonf_show(30,280,"请设置路由器无线参数为:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
	POINT_COLOR=BLUE;
	Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//连接模式
	USART3_RX_STA=0;
//---------------------------------------------------------------------------------------------	
	
/*
数据的发送和接收不是一直进行的，间断的，所以发送部分里面有个永真循环监听、接收要先判断串口标志位
但是发送成功后必定会接收（回应的数据）
因为加了永真循环监听，所以第一次连接的时候要发送数据后才显示连接状态
*/
//---------------------发送&接收数据-------------------------------------------
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)			//WK_UP 退出测试		 
		{ 
			res=0;					
			atk_8266_quit_trans();	//退出透传
			atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //关闭透传模式
			break;												 
		}
		else 
		{
			//=================================================================
			//串口2接收来自LORA的数据
			printf("\r\n--%s--\r\n",  USART2_RX_BUF);	//发送给串口1，串口调试时使用
			USART2_RX_STA &= 0x7FFF;					//[15]置零，开启下一次接收数据				
					
			for(i = 0; i < USART2_REC_LEN; i++)			//清空数组
			{
				USART2_RX_BUF[i] = 0;
			}	
			//=================================================================
			
//--------------------发送数据-------------------------------------------------					
			if((netpro==1))   //TCP Client
			{
				atk_8266_quit_trans();							//退出透传模式
				atk_8266_send_cmd("AT+CIPSEND","OK",20);        //开始透传    
				//第一次连接时会阻塞在这里，等待LORA第一次发送数据
				while(USART2_RX_BUF[0] == 0);					//判断第0位是否为空，防止输出为空
				delay_ms(10);									//判断第0位还不行，延时一下等待数据接收完毕
				
				LCD_Fill(30+54,145,239,160,WHITE);				//清空上一次LCD屏幕上显示的数据，显示之前的‘清屏’
				Show_Str(30+54,145,200,30,USART2_RX_BUF,12,0);	//显示从LORA接收到的数据，即将要发送的数据
				u3_printf("%s\r\n",USART2_RX_BUF);				//串口3发送到WIFI模块，必须以 \r\n结尾，串口接收函数规定了
			}
		}

		t++;			//用于检测连接状态 大约1000 * 10 ms 检测一次
		delay_ms(10);
		
//-------------------接收数据---------------------------------------------------				
		//每次发送之后要接收反馈信号
		if(USART3_RX_STA&0X8000)		//串口3/WIFI接收到一次数据了
		{ 
			rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART3_RX_BUF[rlen]=0;		//添加结束符 
			printf("%s",USART3_RX_BUF);	//发送到串口1  
			sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数 
			LCD_Fill(30+54,180,500,180,WHITE);
			POINT_COLOR=BRED;
			Show_Str(30+54,180,156,12,p,12,0); 			//显示接收到的数据长度
			POINT_COLOR=BLUE;
			LCD_Fill(30,200,239,500,WHITE);
			Show_Str(30,200,180,190,USART3_RX_BUF,12,0);//显示接收到的数据 	

					
			USART3_RX_STA=0;				//置零，允许下一次接收
			if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
			else t=0;                   	//状态为已经连接了,10秒后再检查
		}  
		if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
		{
			constate=atk_8266_consta_check();//得到连接状态
			if(constate=='+')Show_Str(30+30,80,200,12,"连接成功",12,0);  //连接状态
			else Show_Str(30+30,80,200,12,"连接失败",12,0); 	 
			t=0;
		}
		if((t%20)==0)LED0=!LED0;
		atk_8266_at_response(1);	//串口1调试	
	}
	
	myfree(SRAMIN,p);		//释放内存 
	return res;		
} 




