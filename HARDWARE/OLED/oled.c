#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"
#include "stdio.h"
#include "sr301.h"
#include "lsens.h"
#include "beep.h"
#include "rtc.h"

u8 OLED_GRAM[144][8];
int position=0;
extern char temp[20],humi[20],ADC_data[20],ADC_vol[20],date[20],clock[20];
extern u8 temperatureH,temperatureL,humidityH,humidityL;
extern char MQ135_data[20];
extern char MQ135_vol[20];
extern u8 flag;
extern u16 adc_data;
extern float adc_vol;
extern u16 mq135_data;
extern float mq135_vol;
extern u8 Mq135_Max;//������ֵ
extern u8 Temp_Max;//�¶���ֵ
extern u8 Humi_Max;//ʪ����ֵ
extern u8 Light_Max;//�����ֵ

extern u8 Led_Status;//LED�Ʊ�־λ
extern u8 Windows_Status;//�����򿪱�־
extern u8 Door_Status;//�Ŵ򿪱�־
extern u8 SR301_Status;//�Ƿ����˱�־
extern u8 Beep_Status;//�������־
extern float mq135_ppm;
extern u8 ClearOled;//Ϊ�˽������ң�ص��µ�OLEDˢ��BUG���趨�ı�־λ
extern u8 HomeMode;//�Զ�ģʽ��־λ 0���ֶ�ģʽ 1���Զ�ģʽ 2:���ģʽ 3��˯��ģʽ
u8 tim=0;	

//���Ժ���
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//������ʾ
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//��ɫ��ʾ
		}
}

//��Ļ��ת180��
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//������ʾ
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//��ת��ʾ
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

//��ʱ
void IIC_delay(void)
{
	u8 t=3;
	while(t--);
}

//��ʼ�ź�
void I2C_Start(void)
{
	OLED_SDA_Set();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Clr();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}

//�����ź�
void I2C_Stop(void)
{
	OLED_SDA_Clr();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Set();
}

//�ȴ��ź���Ӧ
void I2C_WaitAck(void) //�������źŵĵ�ƽ
{
	OLED_SDA_Set();
	IIC_delay();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}

//д��һ���ֽ�
void Send_Byte(u8 dat)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)//��dat��8λ�����λ����д��
		{
			OLED_SDA_Set();
    }
		else
		{
			OLED_SDA_Clr();
    }
		IIC_delay();
		OLED_SCL_Set();
		IIC_delay();
		OLED_SCL_Clr();//��ʱ���ź�����Ϊ�͵�ƽ
		dat<<=1;
  }
}

//����һ���ֽ�
//mode:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 mode)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if(mode){Send_Byte(0x40);}
  else{Send_Byte(0x00);}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}

//����OLED��ʾ 
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�ʹ��
	OLED_WR_Byte(0x14,OLED_CMD);//������ɱ�
	OLED_WR_Byte(0xAF,OLED_CMD);//������Ļ
}

//�ر�OLED��ʾ 
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�ʹ��
	OLED_WR_Byte(0x10,OLED_CMD);//�رյ�ɱ�
	OLED_WR_Byte(0xAE,OLED_CMD);//�ر���Ļ
}

//�����Դ浽OLED	
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xb0+i,OLED_CMD); //��������ʼ��ַ
		OLED_WR_Byte(0x00,OLED_CMD);   //���õ�����ʼ��ַ
		OLED_WR_Byte(0x10,OLED_CMD);   //���ø�����ʼ��ַ
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		for(n=0;n<128;n++)
		{
			Send_Byte(OLED_GRAM[n][i]);
			I2C_WaitAck();
		}
		I2C_Stop();
  }
}
//��������
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//�����������
			}
  }
	OLED_Refresh();//������ʾ
}

/************************�Լ�д�ģ���������֮ǰ��>***************************/
//���ͷ>��������ͷ>����ָʾĿǰ������
void OLED_ClearPos(void)
{
		u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<8;n++)
			{
			 OLED_GRAM[n][i]=0;//�����������
			}
  }
}

//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���	
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}

//����
//x1,y1:�������
//x2,y2:��������
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1;
	uRow=x1;//�����������
	uCol=y1;
	if(delta_x>0)incx=1; //���õ������� 
	else if (delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//ˮƽ�� 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode);//����
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}
//x,y:Բ������
//r:Բ�İ뾶
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//���㻭�ĵ���Բ�ĵľ���
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//size1:ѡ������ 6x8/6x12/8x16/12x24
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode)
{
	u8 i,m,temp,size2,chr1;
	u8 x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr1=chr-' ';  //����ƫ�ƺ��ֵ
	for(i=0;i<size2;i++)
	{
		if(size1==8)
			  {temp=asc2_0806[chr1][i];} //����0806����
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //����1206����
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //����1608����
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //����2412����
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}


//��ʾ�ַ���
//x,y:�������  
//size1:�����С 
//*chr:�ַ�����ʼ��ַ 
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode)
{
	while((*chr>=' ')&&(*chr<='~'))//�ж��ǲ��ǷǷ��ַ�!
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}

//m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

//��ʾ����
//x,y :�������
//num :Ҫ��ʾ������
//len :���ֵ�λ��
//size:�����С
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode)
{
	u8 t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
			}
  }
}

//��ʾ����
//x,y:�������
//num:���ֶ�Ӧ�����
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode)
{
	u8 m,temp;
	u8 x0=x,y0=y;
	u16 i,size3=(size1/8+((size1%8)?1:0))*size1;  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
	for(i=0;i<size3;i++)
	{
		if(size1==16)
				{temp=Hzk1[num][i];}//����16*16����
		else if(size1==24)
				{temp=Hzk2[num][i];}//����24*24����
		else if(size1==32)       
				{temp=Hzk3[num][i];}//����32*32����
		else if(size1==64)
				{temp=Hzk4[num][i];}//����64*64����
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((x-x0)==size1)
		{x=x0;y0=y0+8;}
		y=y0;
	}
}

//num ��ʾ���ֵĸ���
//space ÿһ����ʾ�ļ��
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode)
{
	u8 i,n,t=0,m=0,r;
	while(1)
	{
		if(m==0)
		{
	    OLED_ShowChinese(128,24,t,16,mode); //д��һ�����ֱ�����OLED_GRAM[][]������
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //��ʾ���
				 {
					for(i=1;i<144;i++)
						{
							for(n=0;n<8;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=1;i<144;i++)   //ʵ������
		{
			for(n=0;n<8;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

//x,y���������
//sizex,sizey,ͼƬ����
//BMP[]��Ҫд���ͼƬ����
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode)
{
	u16 j=0;
	u8 i,n,temp,m;
	u8 x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
     }
	 }
}
//OLED�ĳ�ʼ��
void OLED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG|RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��A�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //��ʼ��GPIOG12
 	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //��ʼ��GPIOD1,5,15
 	GPIO_SetBits(GPIOB,GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	  //��ʼ��GPIOD1,5,15
 	GPIO_SetBits(GPIOD,GPIO_Pin_4);
	
	OLED_RES_Clr();
	delay_ms(200);
	OLED_RES_Set();
	
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD);
}


//OLED��ҳ����ʾ
void OLED_Show_mode(u8 mode)
{
	if(mode==0)
	{
		//OLED_Clear();
		if(ClearOled==1)
		{
			OLED_Clear();
			ClearOled=0;
		}
		OLED_Refresh();	
		OLED_ShowString(10,0,(u8*)"1.Clock",16,1);
		OLED_ShowString(10,16,(u8*)"2.SetMax",16,1);
		OLED_ShowString(10,32,(u8*)"3.HomeStatus",16,1);
		OLED_ShowString(10,48,(u8*)"4.SlectMode",16,1);
		//OLED_ShowString(10,48,(u8*)"5.Music",16,1);//Ԥ���һ��Music����
		//OLED_ClearPos();
		OLED_ShowString(0,position*16,(u8*)">",16,1);
	}
	else if(mode==1)
	{
		if(ClearOled==1)
		{
			OLED_Clear();
			ClearOled=0;
		}
		OLED_Refresh();
		if(tim!=calendar.sec)
		{
			tim=calendar.sec;
			sprintf(date,"Date:%d/%d/%d",calendar.w_year,calendar.w_month,calendar.w_date);
			OLED_ShowString(0,0,(u8*)date,16,1);
			switch(calendar.week)
			{
				case 0:
					OLED_ShowString(32,16,(u8*)"Sunday",16,1);
					break;
				case 1:
					OLED_ShowString(32,16,(u8*)"Monday",16,1);
					break;
				case 2:
					OLED_ShowString(32,16,(u8*)"Tuesday",16,1);
					break;
				case 3:
					OLED_ShowString(32,16,(u8*)"Wednesday",16,1);
					break;
				case 4:
					OLED_ShowString(32,16,(u8*)"Thursday",16,1);
					break;
				case 5:
					OLED_ShowString(32,16,(u8*)"Friday",16,1);
					break;
				case 6:
					OLED_ShowString(32,16,(u8*)"Saturday",16,1);
					break;  
			}
			sprintf(clock,"Time: %2d:%2d:%2d",calendar.hour,calendar.min,calendar.sec);
			OLED_ShowString(0,32,(u8*)clock,16,1);
			if(calendar.sec<10)
					OLED_ShowChar(96,32,'0',16,1);
			if(calendar.min<10)
					OLED_ShowChar(72,32,'0',16,1);
		}	
	}
	else if(mode==2)
	{
		//OLED_Clear();
		if(ClearOled==1)
		{
			OLED_Clear();
			ClearOled=0;
		}
		OLED_Refresh();	
		OLED_ShowString(8,0,(u8*)"1.Mq135_Max:",16,1);
		OLED_ShowString(8,16,(u8*)"2.Temp_Max :",16,1);
		OLED_ShowString(8,32,(u8*)"3.Humi_Max :",16,1);
		OLED_ShowString(8,48,(u8*)"4.Light_Max:",16,1);
		
		OLED_ShowNum(104,0,Mq135_Max,3,16,1);
		OLED_ShowNum(104,16,Temp_Max,3,16,1);
		OLED_ShowNum(104,32,Humi_Max,3,16,1);
		OLED_ShowNum(104,48,Light_Max,3,16,1);
		//OLED_ClearPos();
		OLED_ShowString(0,position*16,(u8*)">",16,1);
	}
	else if(mode==3)
	{
		//OLED_Clear();
		if(ClearOled==1)
		{
			OLED_Clear();
			ClearOled=0;
		}
		OLED_Refresh();	
		OLED_ShowString(8,0,(u8*)"1.WindSta :",16,1);
		OLED_ShowString(8,16,(u8*)"2.Door_Sta:",16,1);
		OLED_ShowString(8,32,(u8*)"3.LED_Sta :",16,1);
		OLED_ShowString(8,48,(u8*)"4.Beep_Sta:",16,1);
		//OLED_ClearPos();
		OLED_ShowString(0,position*16,(u8*)">",16,1);
		if(Windows_Status)
			OLED_ShowString(96,0,(u8*)"on ",16,1);
		else
			OLED_ShowString(96,0,(u8*)"off",16,1);
		if(Door_Status)
			OLED_ShowString(96,16,(u8*)"on ",16,1);
		else
			OLED_ShowString(96,16,(u8*)"off",16,1);
		if(Led_Status==0)
			OLED_ShowString(96,32,(u8*)"on ",16,1);
		else
			OLED_ShowString(96,32,(u8*)"off",16,1);
		if(Beep_Status)
			OLED_ShowString(96,48,(u8*)"on ",16,1);
		else
			OLED_ShowString(96,48,(u8*)"off",16,1);
	}
	else if(mode==4)
	{
		if(ClearOled==1)
		{
			OLED_Clear();
			ClearOled=0;
		}
		OLED_Refresh();	
		OLED_ShowString(10,0,(u8*)"1.ManMode  :",16,1);
		OLED_ShowString(10,16,(u8*)"2.AutoMode :",16,1);
		OLED_ShowString(10,32,(u8*)"3.LeaveMode:",16,1);
		OLED_ShowString(10,48,(u8*)"4.MoveMode :",16,1);
		//OLED_ClearPos();
		OLED_ShowString(0,position*16,(u8*)">",16,1);
		if(HomeMode==0)
		{
			OLED_ShowString(104,0,(u8*)"on ",16,1);
			OLED_ShowString(104,16,(u8*)"off",16,1);
			OLED_ShowString(104,32,(u8*)"off",16,1);
			OLED_ShowString(104,48,(u8*)"off",16,1);
		}
		else if(HomeMode==1)
		{
			OLED_ShowString(104,0,(u8*)"off",16,1);
			OLED_ShowString(104,16,(u8*)"on ",16,1);
			OLED_ShowString(104,32,(u8*)"off",16,1);
			OLED_ShowString(104,48,(u8*)"off",16,1);
		}
		else if(HomeMode==2)
		{
			OLED_ShowString(104,0,(u8*)"off",16,1);
			OLED_ShowString(104,16,(u8*)"off",16,1);
			OLED_ShowString(104,32,(u8*)"on ",16,1);
			OLED_ShowString(104,48,(u8*)"off",16,1);
		}
		else if(HomeMode==3)
		{
			OLED_ShowString(104,0,(u8*)"off",16,1);
			OLED_ShowString(104,16,(u8*)"off",16,1);
			OLED_ShowString(104,32,(u8*)"off",16,1);
			OLED_ShowString(104,48,(u8*)"on ",16,1);
		}
	}
//	else if(mode==5)
//	{
//		//OLED_Clear();
//		if(ClearOled==1)
//		{
//			OLED_Clear();
//			ClearOled=0;
//		}
//		OLED_Refresh();		
//		sprintf(ADC_data,"1.ADC_VAL:%d",adc_data);
//		sprintf(ADC_vol,"2.ADC_VOL:%.2f",adc_vol);
//		sprintf(MQ135_data,"3.MQ_VAL:%d",mq135_data);
//		sprintf(MQ135_vol,"4.MQ_VOL:%.2f",mq135_ppm);
//		OLED_ShowString(0,0,(u8*)ADC_data,16,1);
//		OLED_ShowString(0,16,(u8*)ADC_vol,16,1);
//		OLED_ShowString(0,32,(u8*)MQ135_data,16,1);
//		OLED_ShowString(0,48,(u8*)MQ135_vol,16,1);
//		//OLED_ShowString(0,position*16,(u8*)">",16,1);
//	}
	
	else if(mode==6)
	{
		OLED_Refresh();
		OLED_ShowString(16,16,(u8*)"Closing",16,1);
	}
}
