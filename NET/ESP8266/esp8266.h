#ifndef _ESP8266_H_
#define _ESP8266_H_
#include "stdio.h"	
#include "sys.h" 

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

//子对象1结构体--Location
typedef struct
{
	char id[32];
	char name[32];
	char country[32];
	char path[64];
	char timezone[32];
	char timezone_offset[32];
}Location;
 
//子对象2结构体--Now
typedef struct
{
	char text[20];
	char code[32];
	char temperature[32];
}Now;

//子对象3结构体--Daily
typedef struct
{
	char date[12];
	char text_day[20];	//白天天气
	char high[32];	//最高温度
	char low[32];	//最低温度
	char humidity[32];//湿度
	char wind_speed[32]; //风速
}Daily;

typedef struct
{
	Location location;		//子对象1
	Now now;				//子对象2
	Daily daily[3];		//对象3
	char last_update[64];	//子对象4
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
