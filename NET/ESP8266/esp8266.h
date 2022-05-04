#ifndef _ESP8266_H_
#define _ESP8266_H_
#include "stdio.h"	
#include "sys.h" 

#define REV_OK		0	//������ɱ�־
#define REV_WAIT	1	//����δ��ɱ�־

//�Ӷ���1�ṹ��--Location
typedef struct
{
	char id[32];
	char name[32];
	char country[32];
	char path[64];
	char timezone[32];
	char timezone_offset[32];
}Location;
 
//�Ӷ���2�ṹ��--Now
typedef struct
{
	char text[20];
	char code[32];
	char temperature[32];
}Now;

//�Ӷ���3�ṹ��--Daily
typedef struct
{
	char date[12];
	char text_day[20];	//��������
	char high[32];	//����¶�
	char low[32];	//����¶�
	char humidity[32];//ʪ��
	char wind_speed[32]; //����
}Daily;

typedef struct
{
	Location location;		//�Ӷ���1
	Now now;				//�Ӷ���2
	Daily daily[3];		//����3
	char last_update[64];	//�Ӷ���4
}Results;

void ESP8266_Init(void);

void ESP8266_Clear(void);

void ESP8266_SendData(unsigned char *data, unsigned short len);

unsigned char *ESP8266_GetIPD(unsigned short timeOut);

u8 esp32_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8* esp32_send_data(u8 *cmd,u16 waittime);
u8* esp32_check_cmd(u8 *str);
void esp32_start_trans(void);
u8 esp32_quit_trans(void);
u8 get_current_weather(void);
u8 get_threedays_weather(void);

int cJSON_WeatherParse(char *JSON, Results *results);

#endif
