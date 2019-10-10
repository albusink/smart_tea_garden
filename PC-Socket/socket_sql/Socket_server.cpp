#include <stdio.h>
#include <time.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100
#define ADDR "192.168.43.125"
#define PORT 8086
int main(void)
{
	//��ȡ����ʱ��
	time_t t1;
	struct tm *lt;
	time(&t1);
	lt = localtime(&t1);
	printf("%d/%d/%d %d:%d:%d\n",
		lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);//������
	/*��ȡ��ǰ�·�,��Χ��0-11,����Ҫ��1*/

	//׷�ӷ�ʽ���ļ�
	FILE *fp;
	if(!(fp = fopen("data.txt", "a+")))
	{
		printf("���ļ�ʧ�ܣ�\n");
		exit(1);
	}

	//��ʼ��WS2_32.dll
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//�����׽���
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//���׽���
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(ADDR);
	sockAddr.sin_port = htons(PORT);
	bind(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	//�������״̬
	listen(sock, 20);

	//���տͻ�������
	SOCKADDR clientAddr;
	int size = sizeof(clientAddr);
	int strLen = 0;
	SOCKET clientSock = accept(sock, &clientAddr, &size);

//----------------------������յ�����-----------------------
	printf("���ӳɹ�!\n");
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
		printf("������%d�����ݣ� \t����Ϊ��%s", i, bufferRecv);

		//��ȡʱ��
		time(&t1);
		lt = localtime(&t1);
		printf("%d/%d/%d %d:%d:%d\n",
			lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);//������
		printf("------------------------------------\n");
		//��
		Time1[0] = (lt->tm_year + 1900) / 1000 + '0';
		Time1[1] = (lt->tm_year + 1900) % 1000 / 100 + '0';
		Time1[2] = (lt->tm_year + 1900) % 100 / 10 + '0';
		Time1[3] = (lt->tm_year + 1900) % 10 + '0';
		Time1[4] = '/';
		//��
		Time1[5] = (lt->tm_mon + 1) / 10 + '0';
		Time1[6] = (lt->tm_mon + 1) % 10 + '0';
		Time1[7] = '/';
		//��
		Time1[8] = (lt->tm_mday) / 10 + '0';
		Time1[9] = (lt->tm_mday) % 10 + '0';
		Time1[10] = ' ';
		//ʱ
		Time1[11] = (lt->tm_hour) / 10 + '0';
		Time1[12] = (lt->tm_hour) % 10 + '0';
		Time1[13] = ':';
		//��
		Time1[14] = (lt->tm_min) / 10 + '0';
		Time1[15] = (lt->tm_min) % 10 + '0';
		Time1[16] = ':';
		//��
		Time1[17] = (lt->tm_sec) / 10 + '0';
		Time1[18] = (lt->tm_sec) % 10 + '0';
		Time1[19] = '-';
		Time1[20] = '-';
		Time1[21] = '\0';//���� 
		if ((fputs(Time1, fp)) == EOF || (fputs(bufferRecv, fp)) == EOF)
		{
			printf("д�����ݴ���\n");
			break;
		}
		memset(bufferRecv, 0, 100);
		memset(Time1, 0, 100);
	}
//----------------------������յ�����-----------------------

	printf("�ͻ��˶Ͽ�������\n");

	//�ر��ļ����׽��֡�dll
	fclose(fp);
	closesocket(sock);
	closesocket(clientSock);
	WSACleanup();
	return 0;
}