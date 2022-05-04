/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	esp8266.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		ESP8266�ļ�����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "usart.h"
#include "usart1.h"
#include "lcd.h"

//C��
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#define ESP8266_WIFI_INFO		"AT+CWJAP=\"YEJ\",\"abc88888\"\r\n"

//#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"broker-cn.emqx.io\",1883\r\n"
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"120.77.79.86\",1883\r\n"
//#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtt.mqttsmarthome.online\",8084\r\n"

#define WEATHER_SERVERIP		"AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n"
unsigned char esp8266_buf[128];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
unsigned char esp32_buf[500];//�����洢������Ϣ��
unsigned short esp32_cnt = 0;//��������������Ϣ�����С��
extern u8 weatherflag;//��ȡ������־ 1�����ڻ�ȡ���� 0��û�л�ȡ����
extern u8 wififlag1;//wifiesp8266���ӱ�־
extern u8 wififlag2;//wifiesp32���ӱ�־
Results results[] = {{0}};
//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(USART2, data, len);		//�����豸������������
	}

}

//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Init(void)
{
	
//	GPIO_InitTypeDef GPIO_Initure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

//	//ESP8266��λ����
//	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Initure.GPIO_Pin = GPIO_Pin_1;					//GPIOE1-��λ
//	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOE, &GPIO_Initure);
//	
//	GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_RESET);
//	delay_ms(250);
//	GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_SET);
//	delay_ms(500);
	
	ESP8266_Clear();
	
	UsartPrintf(USART_DEBUG, "0. AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		delay_ms(500);
	//LCD_ShowString(54,110,150,16,16,"AT OK");
	
	UsartPrintf(USART_DEBUG, "1. RST\r\n");
	ESP8266_SendCmd("AT+RST\r\n", "OK");
		delay_ms(500);
	//LCD_ShowString(54,120,150,16,16,"RTS OK");
	
	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "OK");
		delay_ms(500);
	//LCD_ShowString(54,120,150,16,16,"AT+CIPCLOSE OK");
		UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		delay_ms(500);
	
//	UsartPrintf(USART_DEBUG, "3. AT+CIPMUX=0\r\n");
//	ESP8266_SendCmd("AT+CIPMUX=0\r\n", "OK");
//		delay_ms(500);

	//LCD_ShowString(54,130,150,16,16,"AT+CWMODE=1 OK");
	
	UsartPrintf(USART_DEBUG, "4. AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		delay_ms(500);
	//LCD_ShowString(54,140,150,16,16,"AT+CWDHCP=1 OK");
	
	UsartPrintf(USART_DEBUG, "5. CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		delay_ms(500);
	//LCD_ShowString(54,150,150,16,16,"CWJAP OK");
	wififlag1=1;//��־WiFi1�����ӣ���Ļ��ʾWiFi��ɫͼ��
	UsartPrintf(USART_DEBUG, "6. (miniapp)CIPSTART\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_ms(500);
	//LCD_ShowString(54,160,150,16,16,"CIPSTART OK");
	
	UsartPrintf(USART_DEBUG, "6. ESP8266 Init OK\r\n");

}

//esp32����
void esp32_start_trans(void)
{
	while(esp32_send_cmd((u8*)"AT",(u8*)"OK",20))//���WIFIģ���Ƿ�����
	{
	} 
	//���ù���ģʽ 1��stationģʽ   2��APģʽ  3������ AP+stationģʽ
	DEBUG_LOG("send:AT+CWMODE=1\r\n");	
	esp32_send_cmd((u8*)"AT+CWMODE=1",(u8*)"OK",50);
	//Wifiģ������
	DEBUG_LOG("send:AT+RST\r\n");	
	esp32_send_cmd((u8*)"AT+RST",(u8*)"OK",20);
	delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);	
	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	DEBUG_LOG("send:AT+CIPMUX=0\r\n");	
	esp32_send_cmd((u8*)"AT+CIPMUX=0",(u8*)"OK",20);   //0�������ӣ�1��������		
	while(esp32_send_cmd((u8*)ESP8266_WIFI_INFO,(u8*)"WIFI GOT IP",300));					//����Ŀ��·����,���һ��IP
	//myfree(SRAMIN,p);
	wififlag2=1;//��־WiFi2�����ӣ���Ļ��ʾWiFi��ɫͼ��
}

//��ESP8266��������
//cmd:���͵������ַ���;ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��;waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����);1,����ʧ��
u8 esp32_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART1_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART1_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(esp32_check_cmd(ack))
				{
					DEBUG_LOG("receive:%s\r\n",(u8*)ack);
					break;//�õ���Ч���� 
				}
					USART1_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����;����,�ڴ�Ӧ������λ��(str��λ��)
u8* esp32_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART1_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART1_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}





//��ȡһ��ʵʱ����
//���أ�0---��ȡ�ɹ���1---��ȡʧ��
u8 get_current_weather(void)
{
	u8 res;
	res = esp32_send_cmd((u8*)WEATHER_SERVERIP,(u8*)"OK",200);//���ӵ�Ŀ��TCP������
	if(res==1)
		return 1;
	delay_ms(300);
	DEBUG_LOG("send:AT+CIPMODE=1\r\n");	
	esp32_send_cmd((u8*)"AT+CIPMODE=1",(u8*)"OK",100);      //����ģʽΪ��͸��	

	USART1_RX_STA=0;
	DEBUG_LOG("send:AT+CIPSEND\r\n");	
	esp32_send_cmd((u8*)"AT+CIPSEND",(u8*)"OK",100);         //��ʼ͸��
	DEBUG_LOG("start trans...\r\n");
	
	u3_printf("GET https://api.seniverse.com/v3/weather/now.json?key=Savcdz4uhjRLsSUOF&location=maoming&language=en&unit=c\r\n");	
	delay_ms(20);//��ʱ20ms���ص���ָ��ͳɹ���״̬
	USART1_RX_STA=0;	//���㴮��3����
	delay_ms(1000);
	if(USART1_RX_STA&0X8000)		//��ʱ�ٴνӵ�һ�����ݣ�Ϊ����������
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//��ӽ�����
	} 
	DEBUG_LOG("USART3_RX_BUF=%s\r\n",USART1_RX_BUF);
	
	cJSON_WeatherParse((char*)USART1_RX_BUF, results);		//������������
	//��ӡ�ṹ��������
//	printf("��ӡ�ṹ�����������£� \r\n");
//	printf("%s \r\n",results[0].now.text);
//	printf("%s \r\n",results[0].now.temperature);
	
	esp32_quit_trans();//�˳�͸��
	
	DEBUG_LOG("send:AT+CIPCLOSE\r\n");	
	esp32_send_cmd((u8*)"AT+CIPCLOSE",(u8*)"OK",50);         //�ر�����
	return 0;
}

u8 get_threedays_weather(void)
{
	u8 res;
	res = esp32_send_cmd((u8*)WEATHER_SERVERIP,(u8*)"OK",200);//���ӵ�Ŀ��TCP������
	if(res==1)
		return 1;
	delay_ms(300);
	
	DEBUG_LOG("send:AT+CIPMODE=1\r\n");	
	esp32_send_cmd((u8*)"AT+CIPMODE=1",(u8*)"OK",100);      //����ģʽΪ��͸��	

	USART1_RX_STA=0;
	DEBUG_LOG("send:AT+CIPSEND\r\n");	
	esp32_send_cmd((u8*)"AT+CIPSEND",(u8*)"OK",100);         //��ʼ͸��
	DEBUG_LOG("start trans...\r\n");
	
	u3_printf("GET https://api.seniverse.com/v3/weather/daily.json?key=Savcdz4uhjRLsSUOF&location=maoming&language=en&unit=c&start=0&days=3\r\n");	
	delay_ms(20);//��ʱ20ms���ص���ָ��ͳɹ���״̬
	USART1_RX_STA=0;	//���㴮��3����
	delay_ms(1000);
	if(USART1_RX_STA&0X8000)		//��ʱ�ٴνӵ�һ�����ݣ�Ϊ����������
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//��ӽ�����
	} 
	DEBUG_LOG("USART1_RX_BUF=%s\r\n",USART1_RX_BUF);
	
	cJSON_WeatherParse((char*)USART1_RX_BUF, results);		//������������
	esp32_quit_trans();//�˳�͸��
	
	DEBUG_LOG("send:AT+CIPCLOSE\r\n");	
	esp32_send_cmd((u8*)"AT+CIPCLOSE",(u8*)"OK",50);         //�ر�����
	//myfree(SRAMIN,p);
	return 0;
}

//ATK-ESP8266�˳�͸��ģʽ
//����ֵ:0,�˳��ɹ�;
//       1,�˳�ʧ��
u8 esp32_quit_trans(void)
{
	while((USART1->SR&0X40)==0);	//�ȴ����Ϳ�
	USART1->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART1->SR&0X40)==0);	//�ȴ����Ϳ�
	USART1->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART1->SR&0X40)==0);	//�ȴ����Ϳ�
	USART1->DR='+';      
	delay_ms(500);					//�ȴ�500ms
	return esp32_send_cmd((u8*)"AT",(u8*)"OK",20);//�˳�͸���ж�.
}



/*********************************************************************************
* Function Name    �� cJSON_WeatherParse,������������
* Parameter		   �� JSON���������ݰ�  results�����������õ������õ�����
* Return Value     �� 0���ɹ� ����:����
* Function Explain �� 
* Create Date      �� 2017.12.6 by lzn
**********************************************************************************/
int cJSON_WeatherParse(char *JSON, Results *results)
{
	u8 i;
	cJSON *json,*arrayItem,*object,*subobject,*item;
	
	json = cJSON_Parse(JSON); //����JSON���ݰ�
	if(json == NULL)		  //���JSON���ݰ��Ƿ�����﷨�ϵĴ��󣬷���NULL��ʾ���ݰ���Ч
	{
		DEBUG_LOG("Error before: [%s] \r\n",cJSON_GetErrorPtr()); //��ӡ���ݰ��﷨�����λ��
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //ƥ���ַ���"results",��ȡ��������
		{
			int size = cJSON_GetArraySize(arrayItem);     //��ȡ�����ж������
			DEBUG_LOG("cJSON_GetArraySize: size=%d \r\n",size); 
			
			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//��ȡ����������
			{
				/* ƥ���Ӷ���1 */
				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
				{
					DEBUG_LOG("---------------------------------subobject1-------------------------------\r\n");
					if((item = cJSON_GetObjectItem(subobject,"id")) != NULL)   //ƥ���Ӷ���1��Ա"id"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.id,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //ƥ���Ӷ���1��Ա"name"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.name,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"country")) != NULL)//ƥ���Ӷ���1��Ա"country"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.country,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"path")) != NULL)  //ƥ���Ӷ���1��Ա"path"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.path,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone")) != NULL)//ƥ���Ӷ���1��Ա"timezone"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.timezone,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone_offset")) != NULL)//ƥ���Ӷ���1��Ա"timezone_offset"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.timezone_offset,item->valuestring,strlen(item->valuestring));
					}
				}
				/* ƥ���Ӷ���2 */
				if((subobject = cJSON_GetObjectItem(object,"now")) != NULL)
				{
					DEBUG_LOG("---------------------------------subobject2-------------------------------\r\n");
					if((item = cJSON_GetObjectItem(subobject,"text")) != NULL)//ƥ���Ӷ���2��Ա"text"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.text,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"code")) != NULL)//ƥ���Ӷ���2��Ա"code"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.code,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"temperature")) != NULL) //ƥ���Ӷ���2��Ա"temperature"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.temperature,item->valuestring,strlen(item->valuestring));
					}
				}
				/* ƥ���Ӷ���2 */
				if((arrayItem = cJSON_GetObjectItem(object,"daily")) != NULL)
				{
					int size = cJSON_GetArraySize(arrayItem);     //��ȡ�����ж������
					DEBUG_LOG("cJSON_GetArraySize: size=%d \r\n",size); 
					for(i=0;i<size;i++)
					{
						subobject=cJSON_GetArrayItem(arrayItem,i);
						if((item=cJSON_GetObjectItem(subobject,"date")) != NULL)
						{
							DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,date=%s \r\n",item->type,item->string,item->valuestring);
							memcpy(results[0].daily[i].date,item->valuestring,strlen(item->valuestring));
						}
						if((item=cJSON_GetObjectItem(subobject,"text_day")) != NULL)
						{
							DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,text_day=%s \r\n",item->type,item->string,item->valuestring);
							memcpy(results[0].daily[i].text_day,item->valuestring,strlen(item->valuestring));
						}
						if((item=cJSON_GetObjectItem(subobject,"high")) != NULL)
						{
							DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,high=%s \r\n",item->type,item->string,item->valuestring);
							memcpy(results[0].daily[i].high,item->valuestring,strlen(item->valuestring));
							//results[0].daily[i].high=item->valueint;
						}
						if((item=cJSON_GetObjectItem(subobject,"low")) != NULL)
						{
							DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,low=%s \r\n",item->type,item->string,item->valuestring);
							memcpy(results[0].daily[i].low,item->valuestring,strlen(item->valuestring));
							//results[0].daily[i].low=item->valueint;
						}
						if((item=cJSON_GetObjectItem(subobject,"humidity")) != NULL)
						{
							DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,humidity=%s \r\n",item->type,item->string,item->valuestring);
							memcpy(results[0].daily[i].humidity,item->valuestring,strlen(item->valuestring));
						}
						if((item=cJSON_GetObjectItem(subobject,"wind_speed")) != NULL)
						{
							DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,wind_speed=%s \r\n",item->type,item->string,item->valuestring);
							memcpy(results[0].daily[i].wind_speed,item->valuestring,strlen(item->valuestring));
						}
					}
				}
				
				/* ƥ���Ӷ���3 */
				if((subobject = cJSON_GetObjectItem(object,"last_update")) != NULL)
				{
					DEBUG_LOG("---------------------------------subobject3-------------------------------\r\n");
					DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",subobject->type,subobject->string,subobject->valuestring);
					memcpy(results[0].last_update,item->valuestring,strlen(subobject->valuestring));
				}
			} 
		}
	}
	
	cJSON_Delete(json); //�ͷ�cJSON_Parse()����������ڴ�ռ�
	
	return 0;
}
//==========================================================
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	����2�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(weatherflag==0)//������ǻ�ȡ������Ϣ�������������Ϣ
		{
			if(esp8266_cnt >= sizeof(esp8266_buf))	
				esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
			esp8266_buf[esp8266_cnt++] = USART2->DR;
		}
//		else//����ǻ�ȡ�������������������Ϣ
//		{
//			esp8266_buf2[esp8266_cnt2++]=USART_ReceiveData(USART2);	//��ȡ���յ�������;
//			if(esp8266_buf2[esp8266_cnt2-2]=='\r' && esp8266_buf2[esp8266_cnt2-1]=='\n' && esp8266_cnt2>0)
//			{
//				esp8266_buf2[esp8266_cnt2-1]='\0';
//				esp8266_buf2[esp8266_cnt2-2]='\0';
//				esp8266_cnt2=0;
//				//USART_SendStr(USART3,(char*)USART_RX_BUF);//������Է��͵�����������ʾ
//				DEBUG_LOG("%s\r\n",(char*)USART_RX_BUF);//���Ҳ���Է��͵�����������ʾ
//			}
//		}
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}

}


//void USART1_IRQHandler(void)
//{
//	
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�
//	{
//			esp32_buf[esp32_cnt++]=USART_ReceiveData(USART1);	//��ȡ���յ�������;
//	}
//	if(esp32_buf[esp32_cnt-2]=='\r' && esp32_buf[esp32_cnt-1]=='\n' && esp32_cnt>0)
//	{
//		esp32_buf[esp32_cnt-1]='\0';
//		esp32_buf[esp32_cnt-2]='\0';
//		esp32_cnt=0;
//		if(weatherflag==1)
//		{
//			esp32_buf[0]=' ';
//			weatherflag=0;
//		}
//	}
//	USART_ClearFlag(USART1, USART_FLAG_RXNE);
//}
