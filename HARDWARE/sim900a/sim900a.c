#include "sys.h"
#include "delay.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include "sim900a.h"

extern u8 Usart1_buff[128];     /*static和extern的区别*/
char dispbuf[64];


/**************************************************************************/
//函数作用：字符串匹对函数
//函数名称：Find_char(char *a,char *b) ;
//内部参数：数组a   数组b
//修改日期：2022年1月26日  凌晨2：40
//作者：       (CSDN搜)大屁桃
/**************************************************************************/
u8 Find_char(char *a,char *b)  //b为子串
{ 
  if(strstr(a,b)!=NULL)
	    return 0;
	else
			return 1;
}

/**************************************************************************/
//函数作用：发送英文短信函数
//函数名称：sim900a_send_English_message(char *message,char *phonenumber)(uint8_t number);
//内部参数：message phonenumber
//修改日期：2022年1月26日  凌晨2：40、
//作者：       (CSDN搜)大屁桃
/**************************************************************************/
void sim900a_send_English_message(char *message,char *phonenumber)
{

	Usart_SendString2(USART1,"AT\r\n");                            //SIM900A是否与单片机来连接成功
	delay_ms(200);	
	while(Find_char((char*)Usart1_buff,"OK"));                      
	DEBUG_LOG("0/6.AT\r\n");
	
	Usart_SendString2(USART1,"AT&F\r\n");                           //SIM900A复位
	delay_ms(200);	
	while(Find_char((char*)Usart1_buff,"OK"));                     //字符串匹对函数   
	DEBUG_LOG("1/6.AT&F\r\n");	
	
	Usart_SendString2(USART1,"AT+CSCS=\"GSM\"\r\n");               //英文短信指令1
	delay_ms(200);	
	while(Find_char((char*)Usart1_buff,"OK"));                     
	DEBUG_LOG("2/6.AT+CSCS=\"GSM\"\r\n");
	
	
	Usart_SendString2(USART1,"AT+CMGF=1\r\n");                     //英文短信指令2
	delay_ms(200);	
	while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("3/6.AT+CMGF=1\r\n");
	
	sprintf(dispbuf,"AT+CMGS=\"%s\"\r\n",phonenumber);
	Usart_SendString2(USART1,dispbuf);                             //英文短信指令3
	delay_ms(200);
	while(Find_char((char*)Usart1_buff,"OK")); 
	DEBUG_LOG("4/6.AT+CMGS=\"%s\"\r\n",phonenumber);
	
	Usart_SendString2(USART1,message);                              //英文短信指令4
	delay_ms(200);
	while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("5/6.Send_English_message\r\n");
	
	Usart_SendHalfWord(USART1,0x1a);                                //结束指令
	delay_ms(2000);  //延时两秒
	while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("6/6.Sent_OK\r\n");
} 

/**************************************************************************/
//函数作用：发送中文短信函数
//函数名称：sim900a_send_Chinese_message(char *message,char *phonenumber)(uint8_t number);
//内部参数：message phonenumber
//修改日期：2022年1月26日  凌晨2：40
//作者：       (CSDN搜)大屁桃
/**************************************************************************/
void sim900a_send_Chinese_message(char *message,char *phonenumber)
{ 
	
	Usart_SendString2(UART5,"AT\r\n");                           //SIM900A是否与单片机来连接成功
	delay_ms(200);	
	//while(Find_char((char*)Usart1_buff,"OK"));                     //字符串匹对函数   
	DEBUG_LOG("0/7.AT\r\n");
	
	Usart_SendString2(UART5,"AT&F\r\n");                          //SIM900A复位
	delay_ms(200);	
	//while(Find_char((char*)Usart1_buff,"OK"));                    
	DEBUG_LOG("1/7.AT&F\r\n");	
	
	Usart_SendString2(UART5,"AT+CSCS=\"UCS2\"\r\n");               //中文短信指令1
	delay_ms(200);	
	//while(Find_char((char*)Usart1_buff,"OK"));                       
	DEBUG_LOG("2/7.AT+CSCS=\"UCS2\"\r\n");
	
	
	Usart_SendString2(UART5,"AT+CMGF=1\r\n");                     //中文短信指令2
	delay_ms(200);	
	//while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("3/7.AT+CMGF=1\r\n");
	
	Usart_SendString2(UART5,"AT+CSMP=17,167,0,8\r\n");            //中文短信指令2
	delay_ms(200);	
	//while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("4/7.AT+CSMP=17,167,0,8\r\n");
	
	sprintf(dispbuf,"AT+CMGS=\"%s\"\r\n",phonenumber);
	Usart_SendString2(UART5,dispbuf);                             //中文短信指令3
	delay_ms(200);
	//while(Find_char((char*)Usart1_buff,"OK")); 
	DEBUG_LOG("5/7.AT+CMGS=\"%s\"\r\n",phonenumber);
	
	Usart_SendString2(UART5,message);                              //中文短信指令4
	delay_ms(200);
	//while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("6/7.Sent_message\r\n");
	
	Usart_SendHalfWord(UART5,0x1a);                                //中文结束指令
	delay_ms(2000);  //延时两秒
	//while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("7/7.Sure_Sent\r\n");
} 

void sim900a_call_phone(char *phonenumber)
{
	sprintf(dispbuf,"ATD%s;\r\n",phonenumber);
	Usart_SendString2(UART5,dispbuf);
	DEBUG_LOG("%s\r\n",dispbuf);	//英文短信指令3
	//delay_ms(200);
	//while(Find_char((char*)Usart1_buff,"OK")); 
	DEBUG_LOG("Phone_call_ok\r\n");
}



