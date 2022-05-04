/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	esp8266.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.0
	*
	*	说明： 		ESP8266的简单驱动
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"
#include "usart1.h"
#include "lcd.h"

//C库
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
unsigned char esp32_buf[500];//用来存储天气信息的
unsigned short esp32_cnt = 0;//用来计算天气信息数组大小的
extern u8 weatherflag;//获取天气标志 1：正在获取天气 0：没有获取天气
extern u8 wififlag1;//wifiesp8266连接标志
extern u8 wififlag2;//wifiesp32连接标志
Results results[] = {{0}};
//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		Usart_SendString(USART2, data, len);		//发送设备连接请求数据
	}

}

//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//延时等待
	} while(timeOut--);
	
	return NULL;														//超时还未找到，返回空指针

}

//==========================================================
//	函数名称：	ESP8266_Init
//
//	函数功能：	初始化ESP8266
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Init(void)
{
	
//	GPIO_InitTypeDef GPIO_Initure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

//	//ESP8266复位引脚
//	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Initure.GPIO_Pin = GPIO_Pin_1;					//GPIOE1-复位
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
	wififlag1=1;//标志WiFi1已连接，屏幕显示WiFi蓝色图标
	UsartPrintf(USART_DEBUG, "6. (miniapp)CIPSTART\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_ms(500);
	//LCD_ShowString(54,160,150,16,16,"CIPSTART OK");
	
	UsartPrintf(USART_DEBUG, "6. ESP8266 Init OK\r\n");

}

//esp32函数
void esp32_start_trans(void)
{
	while(esp32_send_cmd((u8*)"AT",(u8*)"OK",20))//检查WIFI模块是否在线
	{
	} 
	//设置工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
	DEBUG_LOG("send:AT+CWMODE=1\r\n");	
	esp32_send_cmd((u8*)"AT+CWMODE=1",(u8*)"OK",50);
	//Wifi模块重启
	DEBUG_LOG("send:AT+RST\r\n");	
	esp32_send_cmd((u8*)"AT+RST",(u8*)"OK",20);
	delay_ms(1000);         //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);	
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	DEBUG_LOG("send:AT+CIPMUX=0\r\n");	
	esp32_send_cmd((u8*)"AT+CIPMUX=0",(u8*)"OK",20);   //0：单连接，1：多连接		
	while(esp32_send_cmd((u8*)ESP8266_WIFI_INFO,(u8*)"WIFI GOT IP",300));					//连接目标路由器,并且获得IP
	//myfree(SRAMIN,p);
	wififlag2=1;//标志WiFi2已连接，屏幕显示WiFi蓝色图标
}

//向ESP8266发送命令
//cmd:发送的命令字符串;ack:期待的应答结果,如果为空,则表示不需要等待应答;waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果);1,发送失败
u8 esp32_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART1_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART1_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp32_check_cmd(ack))
				{
					DEBUG_LOG("receive:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART1_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果;其他,期待应答结果的位置(str的位置)
u8* esp32_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART1_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART1_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}





//获取一次实时天气
//返回：0---获取成功，1---获取失败
u8 get_current_weather(void)
{
	u8 res;
	res = esp32_send_cmd((u8*)WEATHER_SERVERIP,(u8*)"OK",200);//连接到目标TCP服务器
	if(res==1)
		return 1;
	delay_ms(300);
	DEBUG_LOG("send:AT+CIPMODE=1\r\n");	
	esp32_send_cmd((u8*)"AT+CIPMODE=1",(u8*)"OK",100);      //传输模式为：透传	

	USART1_RX_STA=0;
	DEBUG_LOG("send:AT+CIPSEND\r\n");	
	esp32_send_cmd((u8*)"AT+CIPSEND",(u8*)"OK",100);         //开始透传
	DEBUG_LOG("start trans...\r\n");
	
	u3_printf("GET https://api.seniverse.com/v3/weather/now.json?key=Savcdz4uhjRLsSUOF&location=maoming&language=en&unit=c\r\n");	
	delay_ms(20);//延时20ms返回的是指令发送成功的状态
	USART1_RX_STA=0;	//清零串口3数据
	delay_ms(1000);
	if(USART1_RX_STA&0X8000)		//此时再次接到一次数据，为天气的数据
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//添加结束符
	} 
	DEBUG_LOG("USART3_RX_BUF=%s\r\n",USART1_RX_BUF);
	
	cJSON_WeatherParse((char*)USART1_RX_BUF, results);		//解析天气数据
	//打印结构体内内容
//	printf("打印结构体内内容如下： \r\n");
//	printf("%s \r\n",results[0].now.text);
//	printf("%s \r\n",results[0].now.temperature);
	
	esp32_quit_trans();//退出透传
	
	DEBUG_LOG("send:AT+CIPCLOSE\r\n");	
	esp32_send_cmd((u8*)"AT+CIPCLOSE",(u8*)"OK",50);         //关闭连接
	return 0;
}

u8 get_threedays_weather(void)
{
	u8 res;
	res = esp32_send_cmd((u8*)WEATHER_SERVERIP,(u8*)"OK",200);//连接到目标TCP服务器
	if(res==1)
		return 1;
	delay_ms(300);
	
	DEBUG_LOG("send:AT+CIPMODE=1\r\n");	
	esp32_send_cmd((u8*)"AT+CIPMODE=1",(u8*)"OK",100);      //传输模式为：透传	

	USART1_RX_STA=0;
	DEBUG_LOG("send:AT+CIPSEND\r\n");	
	esp32_send_cmd((u8*)"AT+CIPSEND",(u8*)"OK",100);         //开始透传
	DEBUG_LOG("start trans...\r\n");
	
	u3_printf("GET https://api.seniverse.com/v3/weather/daily.json?key=Savcdz4uhjRLsSUOF&location=maoming&language=en&unit=c&start=0&days=3\r\n");	
	delay_ms(20);//延时20ms返回的是指令发送成功的状态
	USART1_RX_STA=0;	//清零串口3数据
	delay_ms(1000);
	if(USART1_RX_STA&0X8000)		//此时再次接到一次数据，为天气的数据
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//添加结束符
	} 
	DEBUG_LOG("USART1_RX_BUF=%s\r\n",USART1_RX_BUF);
	
	cJSON_WeatherParse((char*)USART1_RX_BUF, results);		//解析天气数据
	esp32_quit_trans();//退出透传
	
	DEBUG_LOG("send:AT+CIPCLOSE\r\n");	
	esp32_send_cmd((u8*)"AT+CIPCLOSE",(u8*)"OK",50);         //关闭连接
	//myfree(SRAMIN,p);
	return 0;
}

//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 esp32_quit_trans(void)
{
	while((USART1->SR&0X40)==0);	//等待发送空
	USART1->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART1->SR&0X40)==0);	//等待发送空
	USART1->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART1->SR&0X40)==0);	//等待发送空
	USART1->DR='+';      
	delay_ms(500);					//等待500ms
	return esp32_send_cmd((u8*)"AT",(u8*)"OK",20);//退出透传判断.
}



/*********************************************************************************
* Function Name    ： cJSON_WeatherParse,解析天气数据
* Parameter		   ： JSON：天气数据包  results：保存解析后得到的有用的数据
* Return Value     ： 0：成功 其他:错误
* Function Explain ： 
* Create Date      ： 2017.12.6 by lzn
**********************************************************************************/
int cJSON_WeatherParse(char *JSON, Results *results)
{
	u8 i;
	cJSON *json,*arrayItem,*object,*subobject,*item;
	
	json = cJSON_Parse(JSON); //解析JSON数据包
	if(json == NULL)		  //检测JSON数据包是否存在语法上的错误，返回NULL表示数据包无效
	{
		DEBUG_LOG("Error before: [%s] \r\n",cJSON_GetErrorPtr()); //打印数据包语法错误的位置
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //匹配字符串"results",获取数组内容
		{
			int size = cJSON_GetArraySize(arrayItem);     //获取数组中对象个数
			DEBUG_LOG("cJSON_GetArraySize: size=%d \r\n",size); 
			
			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//获取父对象内容
			{
				/* 匹配子对象1 */
				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
				{
					DEBUG_LOG("---------------------------------subobject1-------------------------------\r\n");
					if((item = cJSON_GetObjectItem(subobject,"id")) != NULL)   //匹配子对象1成员"id"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.id,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //匹配子对象1成员"name"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.name,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"country")) != NULL)//匹配子对象1成员"country"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.country,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"path")) != NULL)  //匹配子对象1成员"path"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.path,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone")) != NULL)//匹配子对象1成员"timezone"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.timezone,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone_offset")) != NULL)//匹配子对象1成员"timezone_offset"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.timezone_offset,item->valuestring,strlen(item->valuestring));
					}
				}
				/* 匹配子对象2 */
				if((subobject = cJSON_GetObjectItem(object,"now")) != NULL)
				{
					DEBUG_LOG("---------------------------------subobject2-------------------------------\r\n");
					if((item = cJSON_GetObjectItem(subobject,"text")) != NULL)//匹配子对象2成员"text"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.text,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"code")) != NULL)//匹配子对象2成员"code"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.code,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"temperature")) != NULL) //匹配子对象2成员"temperature"
					{
						DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.temperature,item->valuestring,strlen(item->valuestring));
					}
				}
				/* 匹配子对象2 */
				if((arrayItem = cJSON_GetObjectItem(object,"daily")) != NULL)
				{
					int size = cJSON_GetArraySize(arrayItem);     //获取数组中对象个数
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
				
				/* 匹配子对象3 */
				if((subobject = cJSON_GetObjectItem(object,"last_update")) != NULL)
				{
					DEBUG_LOG("---------------------------------subobject3-------------------------------\r\n");
					DEBUG_LOG("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",subobject->type,subobject->string,subobject->valuestring);
					memcpy(results[0].last_update,item->valuestring,strlen(subobject->valuestring));
				}
			} 
		}
	}
	
	cJSON_Delete(json); //释放cJSON_Parse()分配出来的内存空间
	
	return 0;
}
//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		if(weatherflag==0)//如果不是获取天气信息则用这个接收信息
		{
			if(esp8266_cnt >= sizeof(esp8266_buf))	
				esp8266_cnt = 0; //防止串口被刷爆
			esp8266_buf[esp8266_cnt++] = USART2->DR;
		}
//		else//如果是获取天气则用这个来接收信息
//		{
//			esp8266_buf2[esp8266_cnt2++]=USART_ReceiveData(USART2);	//读取接收到的数据;
//			if(esp8266_buf2[esp8266_cnt2-2]=='\r' && esp8266_buf2[esp8266_cnt2-1]=='\n' && esp8266_cnt2>0)
//			{
//				esp8266_buf2[esp8266_cnt2-1]='\0';
//				esp8266_buf2[esp8266_cnt2-2]='\0';
//				esp8266_cnt2=0;
//				//USART_SendStr(USART3,(char*)USART_RX_BUF);//这个可以发送到串口助手显示
//				DEBUG_LOG("%s\r\n",(char*)USART_RX_BUF);//这个也可以发送到串口助手显示
//			}
//		}
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}

}


//void USART1_IRQHandler(void)
//{
//	
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断
//	{
//			esp32_buf[esp32_cnt++]=USART_ReceiveData(USART1);	//读取接收到的数据;
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
