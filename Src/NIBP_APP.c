#include <SZC909.H>
#include "SystemConstant.h"
#include "NIBP_Lcd.h"
#include "NIBP_APP.h"
#include "NIBP_Timer.h"
#include "NIBP_ADC.h"


 void Delay_10us(uint32 i)			// 1->10us  16->150us 870->8ms  
{
	unsigned int j;
	do
	{
		j=15;
		do{j--;}while(j);
		i--;
	}
    while(i);
}									   

//void Delay1000ms()		//@33.000MHz
//{
//	unsigned char i, j, k;

//	_nop_();
//	_nop_();
//	i = 126;
//	j = 101;
//	k = 169;
//	do
//	{
//		do
//		{
//			while (--k);
//		} while (--j);
//	} while (--i);
//}

/***********************************************
端口初始化
//P07-PUMP  P06-EMV   P05-RXD   P04-TXD   P03-NC    P02-50Hz  P01-NC    P00-NC 
//P17-SWAT  P16-TSH   P15-BUZ   P14-IO    P13-M     P12-NC    P11-NC    P10-NC
//P3 P4 P52-P57  LCD function	P19-vout+ P20-vout-

M1 = P0^7
M2 = P3^5
M3 = P3^4

EMV1 = P0^6
EMV2 = P3^3
EMV3 = P3^2
EMV4 = P3^1
************************************************/



void First_IO_Init(void)
{
	P0M = 0x00;                     //输出 1111 0000
	P0UR = 0xFF;	                //上拉 0000 1111						 	
	P0=0xFF;                                
	
	P1M = 0x00;					    //输入 0000 0000
	P1UR = 0xFF;				    //上拉 1111	1111
	P1 = 0xFF;				
	P1W = 0x00;		
	
	P2M=0x00;				    //0000 0000	
	P2UR=0xFF;				    //1111 1111		
	P2=0xFF;		            //1111 1111
  
	P3M=0x00;
  P3UR=0xFF;
  P3=0xFF;
  
	P4M=0x00;
  P4UR=0xFF;
  P4=0xFF;
  
	P5M=0x00;                   //0000 0000
	P5UR=0xFF;                  //1111 1111
	P5=0xFF;
  
	P3CON=0x00;	                //LCD function
	P4CON=0x00;                 //LCD function										
	P5CON=0x00;	                //LCD function	
}

void Second_IO_Init(void)
{
    P0M = 0x70;                 //输入 0000 0000       
    P0UR = 0x8F;                //上拉 1111 1111
    P0 = 0xFF;
    
    P1M = 0x78;                 //输出 0111 1000
    P1UR = 0xFF;                //上拉 1000 0111
    P1 = 0xFF;												
}


void NIBP_Process1(void)
{
		
	/*	    P0M = 0xF0;
			Delay_10us(1800);
			P0M = 0x00;
			Delay_10us(1620);
			P0M = 0x40;
			Delay_10us(96840);		  //  
			P0M = 0xF0;
			Delay_10us(1940);
			P0M = 0x00;
			Delay_10us(1760);
			P0M = 0x40;
			Delay_10us(96480);		  //  111/79		  */
		    P0M = 0xF0;
			Delay_10us(1770);
			P0M = 0x00;
			Delay_10us(1530);
			P0M = 0x40;
			Delay_10us(65469);		  //  
			P0M = 0xF0;
			Delay_10us(1875);
			P0M = 0x00;
			Delay_10us(1635);
			P0M = 0x40;
			Delay_10us(65219);		  //
		    P0M = 0xF0;
			Delay_10us(1975);
			P0M = 0x00;
			Delay_10us(1745);
			P0M = 0x40;
			Delay_10us(65019);		  //
			P0M = 0xF0;
			Delay_10us(2185);
			P0M = 0x00;
			Delay_10us(1965);
			P0M = 0x40;
			Delay_10us(64599);		  //
	/*        P0M = 0xF0;
			Delay_10us(2180);
			P0M = 0x00;
			Delay_10us(2040);
			P0M = 0x40;
			Delay_10us(95800);		  //  	  			   */
            P0M = 0xF0;
			Delay_10us(2380);
			P0M = 0x00;
			Delay_10us(2190);
			P0M = 0x40;
			Delay_10us(64209);		  //
			P0M = 0xF0;
			Delay_10us(2500);
			P0M = 0x00;
			Delay_10us(2310);
			P0M = 0x40;
			Delay_10us(63969);		  //
			P0M = 0xF0;
			Delay_10us(2690);
			P0M = 0x00;
			Delay_10us(2520);
			P0M = 0x40;
			Delay_10us(63589);		  //
			P0M = 0xF0;
			Delay_10us(2890);
			P0M = 0x00;
			Delay_10us(2700);
			P0M = 0x40;
			Delay_10us(63189);			//
			P0M = 0xF0;
			Delay_10us(3000);
			P0M = 0x00;
			Delay_10us(2810);
			P0M = 0x40;
			Delay_10us(62969);			//
			P0M = 0xF0;
			Delay_10us(3120);
			P0M = 0x00;
			Delay_10us(2930);
			P0M = 0x40;
			Delay_10us(62729);			//
			P0M = 0xF0;
			Delay_10us(3280);
			P0M = 0x00;
			Delay_10us(3090);
			P0M = 0x40;
			Delay_10us(62409);			//	 
			P0M = 0xF0;
			Delay_10us(3380);
			P0M = 0x00;
			Delay_10us(3190);
			P0M = 0x40;
			Delay_10us(62209);			//
			P0M = 0xF0;
			Delay_10us(3480);
			P0M = 0x00;
			Delay_10us(3290);
			P0M = 0x40;
			Delay_10us(62009);			//
			P0M = 0xF0;
			Delay_10us(3620);
			P0M = 0x00;
			Delay_10us(3420);
			P0M = 0x40;
			Delay_10us(61720);		   //		  max
			P0M = 0xF0;
			Delay_10us(3480);
			P0M = 0x00;
			Delay_10us(3290);
			P0M = 0x40;
			Delay_10us(62000);		   //		  
			P0M = 0xF0;
			Delay_10us(3300);
			P0M = 0x00;
			Delay_10us(3110);
			P0M = 0x40;
			Delay_10us(62360);			//
			P0M = 0xF0;
			Delay_10us(3100);
			P0M = 0x00;
			Delay_10us(2910);
			P0M = 0x40;
			Delay_10us(62760);			//
			P0M = 0xF0;
			Delay_10us(2900);
			P0M = 0x00;
			Delay_10us(2710);
			P0M = 0x40;
			Delay_10us(63160);			//
			P0M = 0xF0;
			Delay_10us(2780);
			P0M = 0x00;
			Delay_10us(2590);
			P0M = 0x40;
			Delay_10us(63400);			//
			P0M = 0xF0;
			Delay_10us(2580);
			P0M = 0x00;
			Delay_10us(2390);
			P0M = 0x40;
			Delay_10us(63420);			//
			P0M = 0xF0;
			Delay_10us(2380);
			P0M = 0x00;
			Delay_10us(2190);
			P0M = 0x40;
			Delay_10us(63820);			//
			P0M = 0xF0;
			Delay_10us(2180);
			P0M = 0x00;
			Delay_10us(1990);
			P0M = 0x40;
			Delay_10us(64220);			//
			P0M = 0xF0;
			Delay_10us(2080);
			P0M = 0x00;
			Delay_10us(1890);
			P0M = 0x40;
			Delay_10us(64420);			//
			P0M = 0xF0;
			Delay_10us(1800);
			P0M = 0x00;
			Delay_10us(1610);
			P0M = 0x40;
			Delay_10us(64980);			//
			
}

