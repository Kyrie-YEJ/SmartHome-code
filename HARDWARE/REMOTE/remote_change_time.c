#include "remote.h"
#include "stdio.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "string.h"
#include "remote_change_time.h"




u8 key_get=0;		//���ü�ֵ
char str[12];	
int n=-1;		//����ң������ʱ��λ
//static int x=3,y=0;//ʱ������
//int i,j,k=0;
extern u8 position;//OLED��>��������

void remote_change_time()
{
	key_get=Remote_Scan();	
	if(key_get)
	{	 
		switch(key_get)
		{
			case 0:strcpy(str,"ERROR");break;			   
			case 162:strcpy(str,"POWER");break;	    
			case 98:strcpy(str,"UP");break;	    
			case 2:strcpy(str,"PLAY");break;		 
			case 226:strcpy(str,"ALIENTEK");break;		  
			case 194:strcpy(str,"RIGHT");break;	   
			case 34:strcpy(str,"LEFT");break;		  
			case 224:strcpy(str,"VOL-");break;		  
			case 168:strcpy(str,"DOWN");break;		   
			case 144:strcpy(str,"VOL+");break;		    
			case 104:n=1;break;		  
			case 152:n=2;break;	   
			case 176:n=3;break;	    
			case 48 :n=4;break;		    
			case 24 :n=5;break;		    
			case 122:n=6;break;		  
			case 16 :n=7;break;			   					
			case 56 :n=8;break;	 
			case 90 :n=9;break;
			case 66 :n=0;break;
			case 82 :strcpy(str,"DELETE");break;
			default :strcpy(str," ");break;
		}
		if (n>=0&&n<=9)
			strcpy(str," ");
		if(strcmp(str,"ALIENTEK")==0)
		{
			DEBUG_LOG("Key=ALIENTEK\r\n");
		}
		else if(strcmp(str,"UP")==0)//����
		{
			DEBUG_LOG("Key=UP\r\n");
		}
		else if(strcmp(str,"DOWN")==0)//����
		{	
			DEBUG_LOG("Key=DOWN\r\n");
		}
		else if(strcmp(str,"LEFT")==0)//����
		{
			DEBUG_LOG("Key=LEFT\r\n");
		}
		else if(strcmp(str,"RIGHT")==0)//����
		{
			DEBUG_LOG("Key=RIGHT\r\n");
		}
		else if(strcmp(str,"PLAY")==0)//ȷ�ϼ�
		{
			DEBUG_LOG("Key=PLAY\r\n");
		}
		else if(strcmp(str,"VOL+")==0)//���Ӽ�
		{
			DEBUG_LOG("Key=VOL+\r\n");
		}
		else if(strcmp(str,"VOL-")==0)//���ټ�
		{
			DEBUG_LOG("Key=VOL-\r\n");
		}
		else if(strcmp(str,"DELETE")==0)//���ؼ�
		{
			DEBUG_LOG("Key=DELETE\r\n");
		}	
		else if (n!=-1)
		{ 
			n=-1;
		}
	}
}

