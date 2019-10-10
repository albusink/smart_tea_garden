#include <stdio.h>
#include <time.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100
#define ADDR "192.168.43.125"
#define PORT 8086
int main(void)
{
	//获取本地时间
	time_t t1;
	struct tm *lt;
	time(&t1);
	lt = localtime(&t1);
	printf("%d/%d/%d %d:%d:%d\n",
		lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);//输出结果
	/*获取当前月份,范围是0-11,所以要加1*/

	//追加方式打开文件
	FILE *fp;
	if(!(fp = fopen("data.txt", "a+")))
	{
		printf("打开文件失败！\n");
		exit(1);
	}

	//初始化WS2_32.dll
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//创建套接字
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//绑定套接字
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(ADDR);
	sockAddr.sin_port = htons(PORT);
	bind(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	//进入监听状态
	listen(sock, 20);

	//接收客户端请求
	SOCKADDR clientAddr;
	int size = sizeof(clientAddr);
	int strLen = 0;
	SOCKET clientSock = accept(sock, &clientAddr, &size);

//----------------------处理接收到数据-----------------------
	printf("连接成功!\n");
	char bufferRecv[BUF_SIZE] = { 0 };
	char Time1[100] = { 0 };
	int i = 0;
	while (1)
	{
		strLen = recv(clientSock, bufferRecv, BUF_SIZE, 0);
		if (!strLen) break;
		send(clientSock, bufferRecv, strLen, 0);
		bufferRecv[strLen] = '\0';
		i++;
		printf("接收了%d次数据！ \t数据为：%s", i, bufferRecv);

		//获取时间
		time(&t1);
		lt = localtime(&t1);
		printf("%d/%d/%d %d:%d:%d\n",
			lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);//输出结果
		printf("------------------------------------\n");
		//年
		Time1[0] = (lt->tm_year + 1900) / 1000 + '0';
		Time1[1] = (lt->tm_year + 1900) % 1000 / 100 + '0';
		Time1[2] = (lt->tm_year + 1900) % 100 / 10 + '0';
		Time1[3] = (lt->tm_year + 1900) % 10 + '0';
		Time1[4] = '/';
		//月
		Time1[5] = (lt->tm_mon + 1) / 10 + '0';
		Time1[6] = (lt->tm_mon + 1) % 10 + '0';
		Time1[7] = '/';
		//日
		Time1[8] = (lt->tm_mday) / 10 + '0';
		Time1[9] = (lt->tm_mday) % 10 + '0';
		Time1[10] = ' ';
		//时
		Time1[11] = (lt->tm_hour) / 10 + '0';
		Time1[12] = (lt->tm_hour) % 10 + '0';
		Time1[13] = ':';
		//分
		Time1[14] = (lt->tm_min) / 10 + '0';
		Time1[15] = (lt->tm_min) % 10 + '0';
		Time1[16] = ':';
		//秒
		Time1[17] = (lt->tm_sec) / 10 + '0';
		Time1[18] = (lt->tm_sec) % 10 + '0';
		Time1[19] = '-';
		Time1[20] = '-';
		Time1[21] = '\0';//乱码 
		if ((fputs(Time1, fp)) == EOF || (fputs(bufferRecv, fp)) == EOF)
		{
			printf("写出数据错误！\n");
			break;
		}
		memset(bufferRecv, 0, 100);
		memset(Time1, 0, 100);
	}
//----------------------处理接收到数据-----------------------

	printf("客户端断开了连接\n");

	//关闭文件、套接字、dll
	fclose(fp);
	closesocket(sock);
	closesocket(clientSock);
	WSACleanup();
	return 0;
}