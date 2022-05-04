#include "sys.h"
#include "delay.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include "sim900a.h"

extern u8 Usart1_buff[128];     /*static��extern������*/
char dispbuf[64];


/**************************************************************************/
//�������ã��ַ���ƥ�Ժ���
//�������ƣ�Find_char(char *a,char *b) ;
//�ڲ�����������a   ����b
//�޸����ڣ�2022��1��26��  �賿2��40
//���ߣ�       (CSDN��)��ƨ��
/**************************************************************************/
u8 Find_char(char *a,char *b)  //bΪ�Ӵ�
{ 
  if(strstr(a,b)!=NULL)
	    return 0;
	else
			return 1;
}

/**************************************************************************/
//�������ã�����Ӣ�Ķ��ź���
//�������ƣ�sim900a_send_English_message(char *message,char *phonenumber)(uint8_t number);
//�ڲ�������message phonenumber
//�޸����ڣ�2022��1��26��  �賿2��40��
//���ߣ�       (CSDN��)��ƨ��
/**************************************************************************/
void sim900a_send_English_message(char *message,char *phonenumber)
{

	Usart_SendString2(USART1,"AT\r\n");                            //SIM900A�Ƿ��뵥Ƭ�������ӳɹ�
	delay_ms(200);	
	while(Find_char((char*)Usart1_buff,"OK"));                      
	DEBUG_LOG("0/6.AT\r\n");
	
	Usart_SendString2(USART1,"AT&F\r\n");                           //SIM900A��λ
	delay_ms(200);	
	while(Find_char((char*)Usart1_buff,"OK"));                     //�ַ���ƥ�Ժ���   
	DEBUG_LOG("1/6.AT&F\r\n");	
	
	Usart_SendString2(USART1,"AT+CSCS=\"GSM\"\r\n");               //Ӣ�Ķ���ָ��1
	delay_ms(200);	
	while(Find_char((char*)Usart1_buff,"OK"));                     
	DEBUG_LOG("2/6.AT+CSCS=\"GSM\"\r\n");
	
	
	Usart_SendString2(USART1,"AT+CMGF=1\r\n");                     //Ӣ�Ķ���ָ��2
	delay_ms(200);	
	while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("3/6.AT+CMGF=1\r\n");
	
	sprintf(dispbuf,"AT+CMGS=\"%s\"\r\n",phonenumber);
	Usart_SendString2(USART1,dispbuf);                             //Ӣ�Ķ���ָ��3
	delay_ms(200);
	while(Find_char((char*)Usart1_buff,"OK")); 
	DEBUG_LOG("4/6.AT+CMGS=\"%s\"\r\n",phonenumber);
	
	Usart_SendString2(USART1,message);                              //Ӣ�Ķ���ָ��4
	delay_ms(200);
	while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("5/6.Send_English_message\r\n");
	
	Usart_SendHalfWord(USART1,0x1a);                                //����ָ��
	delay_ms(2000);  //��ʱ����
	while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("6/6.Sent_OK\r\n");
} 

/**************************************************************************/
//�������ã��������Ķ��ź���
//�������ƣ�sim900a_send_Chinese_message(char *message,char *phonenumber)(uint8_t number);
//�ڲ�������message phonenumber
//�޸����ڣ�2022��1��26��  �賿2��40
//���ߣ�       (CSDN��)��ƨ��
/**************************************************************************/
void sim900a_send_Chinese_message(char *message,char *phonenumber)
{ 
	
	Usart_SendString2(UART5,"AT\r\n");                           //SIM900A�Ƿ��뵥Ƭ�������ӳɹ�
	delay_ms(200);	
	//while(Find_char((char*)Usart1_buff,"OK"));                     //�ַ���ƥ�Ժ���   
	DEBUG_LOG("0/7.AT\r\n");
	
	Usart_SendString2(UART5,"AT&F\r\n");                          //SIM900A��λ
	delay_ms(200);	
	//while(Find_char((char*)Usart1_buff,"OK"));                    
	DEBUG_LOG("1/7.AT&F\r\n");	
	
	Usart_SendString2(UART5,"AT+CSCS=\"UCS2\"\r\n");               //���Ķ���ָ��1
	delay_ms(200);	
	//while(Find_char((char*)Usart1_buff,"OK"));                       
	DEBUG_LOG("2/7.AT+CSCS=\"UCS2\"\r\n");
	
	
	Usart_SendString2(UART5,"AT+CMGF=1\r\n");                     //���Ķ���ָ��2
	delay_ms(200);	
	//while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("3/7.AT+CMGF=1\r\n");
	
	Usart_SendString2(UART5,"AT+CSMP=17,167,0,8\r\n");            //���Ķ���ָ��2
	delay_ms(200);	
	//while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("4/7.AT+CSMP=17,167,0,8\r\n");
	
	sprintf(dispbuf,"AT+CMGS=\"%s\"\r\n",phonenumber);
	Usart_SendString2(UART5,dispbuf);                             //���Ķ���ָ��3
	delay_ms(200);
	//while(Find_char((char*)Usart1_buff,"OK")); 
	DEBUG_LOG("5/7.AT+CMGS=\"%s\"\r\n",phonenumber);
	
	Usart_SendString2(UART5,message);                              //���Ķ���ָ��4
	delay_ms(200);
	//while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("6/7.Sent_message\r\n");
	
	Usart_SendHalfWord(UART5,0x1a);                                //���Ľ���ָ��
	delay_ms(2000);  //��ʱ����
	//while(Find_char((char*)Usart1_buff,"OK"));  
	DEBUG_LOG("7/7.Sure_Sent\r\n");
} 

void sim900a_call_phone(char *phonenumber)
{
	sprintf(dispbuf,"ATD%s;\r\n",phonenumber);
	Usart_SendString2(UART5,dispbuf);
	DEBUG_LOG("%s\r\n",dispbuf);	//Ӣ�Ķ���ָ��3
	//delay_ms(200);
	//while(Find_char((char*)Usart1_buff,"OK")); 
	DEBUG_LOG("Phone_call_ok\r\n");
}



