#include "common.h"
#include "stdlib.h"
#include "usart2.h"

u8 netpro=0;		//����ģʽ
//����ֵ:0���������������������
u8 atk_8266_wifista_test(void)
{
	u8 key;  		//��ⰴ��
//---------------------------------------------------	
	u8 i;			//�����±�
	extern u16 USART2_RX_STA; 
	extern u8 USART2_RX_BUF[USART2_REC_LEN];	//����2/LORA����ڻ���ͱ�־����

//---------------------------------------------------
	//	u8 ipbuf[16] = "192.168.137.19"; 			//IP����
	u8 ipbuf[16] = {0};								//����ֵΪ0������IP����������������
	u8 *p;
	u16 t=999;										//���ٵ�һ�λ�ȡ����״̬����һ�μ������״̬��
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;									//����״̬
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
	delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	
//------------------------�������ӵ���WIFI��������/���ܷ�ʽ/���� �ŵ�ATָ����----------------------------
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//����Ŀ��·����,���һ��IP
	
PRESTA:
	//ѡ��TCP�ͻ���
	netpro|=1;
	if(netpro&0X01)     //TCP Client  ͸��ģʽ
	{
		LCD_Clear(WHITE);
		POINT_COLOR=RED;
		Show_Str_Mid(0,30,"ATK-ESP WIFI-STA ����",16,240); 
		Show_Str(30,50,200,16,"��������ATK-ESPģ��,���Ե�...",12,0);
		if(atk_8266_ip_set("WIFI-STA Զ��IP����",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))
			goto PRESTA;	//IP����   ������ ip���ñ��⡢����ģʽ���˿ںš�IP��ַ��������
		
		
		atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
		sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��TCP������
		while(atk_8266_send_cmd(p,"OK",200))		//��������Ƿ�ɹ�
		{
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			Show_Str_Mid(0,40,"WK_UP:������ѡ",16,240);
			Show_Str(30,80,200,12,"ATK-ESP ����TCPʧ��",12,0); //����ʧ��	 
			key=KEY_Scan(0);
			if(key==WKUP_PRES)goto PRESTA;
		}	
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��			
	}
	
//-------------------���ͽ������ݽ������------------------------------------------
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-ESP WIFI-STA ����",16,240);
	Show_Str(30,50,200,16,"��������ATK-ESPģ��,���Ե�...",12,0);			
	LCD_Fill(30,50,239,50+12,WHITE);			//���֮ǰ����ʾ
	Show_Str(30,50,200,16,"WK_UP:�˳�����",12,0);
	LCD_Fill(30,80,239,80+12,WHITE);
	
	atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP
	sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum);
	Show_Str(30,65,200,12,p,12,0);				//��ʾIP��ַ�Ͷ˿�	
	Show_Str(30,80,200,12,"״̬:",12,0); 		//����״̬
	Show_Str(120,80,200,12,"ģʽ:",12,0); 		//����״̬
	Show_Str(30,130,200,12,"��������:",12,0); 	//��������
	Show_Str(30,180,200,12,"��������:",12,0);	//��������
//	atk_8266_wificonf_show(30,280,"������·�������߲���Ϊ:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
	POINT_COLOR=BLUE;
	Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//����ģʽ
	USART3_RX_STA=0;
//---------------------------------------------------------------------------------------------	
	
/*
���ݵķ��ͺͽ��ղ���һֱ���еģ���ϵģ����Է��Ͳ��������и�����ѭ������������Ҫ���жϴ��ڱ�־λ
���Ƿ��ͳɹ���ض�����գ���Ӧ�����ݣ�
��Ϊ��������ѭ�����������Ե�һ�����ӵ�ʱ��Ҫ�������ݺ����ʾ����״̬
*/
//---------------------����&��������-------------------------------------------
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)			//WK_UP �˳�����		 
		{ 
			res=0;					
			atk_8266_quit_trans();	//�˳�͸��
			atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //�ر�͸��ģʽ
			break;												 
		}
		else 
		{
			//=================================================================
			//����2��������LORA������
			printf("\r\n--%s--\r\n",  USART2_RX_BUF);	//���͸�����1�����ڵ���ʱʹ��
			USART2_RX_STA &= 0x7FFF;					//[15]���㣬������һ�ν�������				
					
			for(i = 0; i < USART2_REC_LEN; i++)			//�������
			{
				USART2_RX_BUF[i] = 0;
			}	
			//=================================================================
			
//--------------------��������-------------------------------------------------					
			if((netpro==1))   //TCP Client
			{
				atk_8266_quit_trans();							//�˳�͸��ģʽ
				atk_8266_send_cmd("AT+CIPSEND","OK",20);        //��ʼ͸��    
				//��һ������ʱ������������ȴ�LORA��һ�η�������
				while(USART2_RX_BUF[0] == 0);					//�жϵ�0λ�Ƿ�Ϊ�գ���ֹ���Ϊ��
				delay_ms(10);									//�жϵ�0λ�����У���ʱһ�µȴ����ݽ������
				
				LCD_Fill(30+54,145,239,160,WHITE);				//�����һ��LCD��Ļ����ʾ�����ݣ���ʾ֮ǰ�ġ�������
				Show_Str(30+54,145,200,30,USART2_RX_BUF,12,0);	//��ʾ��LORA���յ������ݣ�����Ҫ���͵�����
				u3_printf("%s\r\n",USART2_RX_BUF);				//����3���͵�WIFIģ�飬������ \r\n��β�����ڽ��պ����涨��
			}
		}

		t++;			//���ڼ������״̬ ��Լ1000 * 10 ms ���һ��
		delay_ms(10);
		
//-------------------��������---------------------------------------------------				
		//ÿ�η���֮��Ҫ���շ����ź�
		if(USART3_RX_STA&0X8000)		//����3/WIFI���յ�һ��������
		{ 
			rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
			USART3_RX_BUF[rlen]=0;		//��ӽ����� 
			printf("%s",USART3_RX_BUF);	//���͵�����1  
			sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ��� 
			LCD_Fill(30+54,180,500,180,WHITE);
			POINT_COLOR=BRED;
			Show_Str(30+54,180,156,12,p,12,0); 			//��ʾ���յ������ݳ���
			POINT_COLOR=BLUE;
			LCD_Fill(30,200,239,500,WHITE);
			Show_Str(30,200,180,190,USART3_RX_BUF,12,0);//��ʾ���յ������� 	

					
			USART3_RX_STA=0;				//���㣬������һ�ν���
			if(constate!='+')t=1000;		//״̬Ϊ��δ����,������������״̬
			else t=0;                   	//״̬Ϊ�Ѿ�������,10����ټ��
		}  
		if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
		{
			constate=atk_8266_consta_check();//�õ�����״̬
			if(constate=='+')Show_Str(30+30,80,200,12,"���ӳɹ�",12,0);  //����״̬
			else Show_Str(30+30,80,200,12,"����ʧ��",12,0); 	 
			t=0;
		}
		if((t%20)==0)LED0=!LED0;
		atk_8266_at_response(1);	//����1����	
	}
	
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	return res;		
} 




