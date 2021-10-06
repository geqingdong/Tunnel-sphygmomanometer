#include <SZC909.H>
#include "SystemConstant.h"
#include "NIBP_Lcd.h"
#include "NIBP_APP.h"
#include "NIBP_Timer.h"
#include "NIBP_ADC.h"


uint8 xdata	*pLCDRam;

uint8 xdata LCD_RAM _at_ 0xF00E;		//add 14 offset   在0xF00E的地址

//*****************************************************
//LCD初始化
//*****************************************************
void LCD_Init(void)
{
//	SetDisplay();
	ClearDisplay();
    
	P4CON &= 0x00;	
	P3CON &= 0x00;
	P5CON  = 0x00;
    
    LCDM1 &= 0x00;          // Clear LCDM1   
    LCDM1 |= 0x02;          // Enable LCD Pump
    
	//======== LCD Frame selection ======================	
    LCDM1 &= 0xF3;        // LCD Clock=256HZ,Frame=64Hz	@4-COM	
  //LCDM1 |= 0x08;        // LCD Clock=256HZ,Frame=43Hz	@6-COM	
  //LCDM1 |= 0x04;        // LCD Clock=512HZ,Frame=128Hz	@4-COM	
  //LCDM1 |= 0x0C;        // LCD Clock=512HZ,Frame=85Hz	@6-COM	
 
    LCDM2 &= 0x00;        // Clear LCDM2  
  //======== set Low power mode =======================			
  //LCDM2 &= 0xDF;        // Enable Low power Mode @STOP MODE
  //LCDM2 |= 0x20;        // Disable LCD low power mode.
    LCDM2 |= 0x00;        // Enable LCD low power mode.
    
	//======== VLCD  selection ==========================		
  //LCDM2 &= 0xF8;          // VLCD=2.6V
  //LCDM2 |= 0x01;          // VLCD=2.8V
  //LCDM2 |= 0x02;          // VLCD=3.0V
  //LCDM2 |= 0x03;          // VLCD=3.2V
    LCDM2 |= 0x04;          // VLCD=3.4V
  //LCDM2 |= 0x05;          // VLCD=4.5V
		
	//======== Turn on duty  selection @Low Power Mode====	
    LCDM2 |= 0x18;     
  //LCDM2 &= 0xE7;      // Set Duty = 4*(1/32k)@Frame=128Hz&85Hz , 8*(1/32k)@Frame=64Hz&43Hz
  //LCDM2 |= 0x08;      // Set Duty = 6*(1/32k)@Frame=128Hz&85Hz , 12*(1/32k)@Frame=64Hz&43Hz
  //LCDM2 |= 0x10;      // Set Duty = 8*(1/32k)@Frame=128Hz&85Hz , 16*(1/32k)@Frame=64Hz&43Hz
  //LCDM2 |= 0x18;      // Set Duty = 16*(1/32k)@Frame=128Hz&85Hz , 32*(1/32k)@Frame=64Hz&43Hz

    LCDM1 |= 0x01;      // Enable LCD 	
}

/************************************************************
LCD全关
************************************************************/
void ClearDisplay(void)
{
    uint8 i;
    
    pLCDRam=&LCD_RAM;					
    
    for(i=0;i<LCD_RAM_SIZE;i++)
    {
        *pLCDRam=0x00;
        pLCDRam++;
    }	
}

/***********************************************************
LCD全显
***********************************************************/
void SetDisplay(void)
{
    uint8 i;
    
    pLCDRam=&LCD_RAM;					
    
    for(i=0;i<LCD_RAM_SIZE;i++)
    {
        *pLCDRam=0xFF;
        pLCDRam++;
    }	
}

void LCD_Display1(uint8 i)				  //LCD第六位
{
	if(i==1)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 7;
        *pLCDRam = 0x03;               	  //1
    }
	else if(i==2)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 7;
        *pLCDRam = 0x0D;             	  //2
    }
}

void LCD_Display2(uint8 i)				  //LCD第五位
{
	if(i==0)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 6;
        *pLCDRam = 0x0F;  
		pLCDRam--;
		*pLCDRam = 0x05;               	  //0
    }

	else if(i==1)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 5;
        *pLCDRam = 0x05;             	  //1
    }
	else if(i==2)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 6;
        *pLCDRam = 0x0D;  
		pLCDRam--;
		*pLCDRam = 0x03;               	  //2
    }
    else if(i==3)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 6;
        *pLCDRam = 0x09;  
		pLCDRam--;
		*pLCDRam = 0x07;               	  //3
    }
    else if(i==4)
    {
       pLCDRam = &LCD_RAM;	
        pLCDRam += 6;
        *pLCDRam = 0x02;  
		pLCDRam--;
		*pLCDRam = 0x07;                  //4
    }
    else if(i==5)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 6;
        *pLCDRam = 0x0B;  
		pLCDRam--;
		*pLCDRam = 0x06;                  //5      
    }
    else if(i==6)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 6;
        *pLCDRam = 0x0F;  
		pLCDRam--;
		*pLCDRam = 0x06;                  //6
    }
    else if(i==7)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 6;
        *pLCDRam = 0x01;  
		pLCDRam--;
		*pLCDRam = 0x05;                   //7
    }
    else if(i==8)
    {
		 pLCDRam = &LCD_RAM;
		 pLCDRam += 6;
		 *pLCDRam = 0x0F;
		 pLCDRam--;
		 *pLCDRam = 0x07;			//8

    }
	else if(i==9)
    {
		 pLCDRam = &LCD_RAM;
		 pLCDRam += 6;
		 *pLCDRam = 0x0B;
		 pLCDRam--;
		 *pLCDRam = 0x07;			//9

    }

}
void LCD_Display3(uint8 i)				  //LCD第四位
{

	if(i==0)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 4;
        *pLCDRam = 0x0F;  
		pLCDRam--;
		*pLCDRam = 0x05;               	  //0
    }
	else if(i==1)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 3;
        *pLCDRam = 0x05;             	  //1
    }
	else if(i==2)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 4;
        *pLCDRam = 0x0D;  
		pLCDRam--;
		*pLCDRam = 0x03;               	  //2
    }
    else if(i==3)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 4;
        *pLCDRam = 0x09;  
		pLCDRam--;
		*pLCDRam = 0x07;               	  //3
    }
    else if(i==4)
    {
       pLCDRam = &LCD_RAM;	
        pLCDRam += 4;
        *pLCDRam = 0x02;  
		pLCDRam--;
		*pLCDRam = 0x07;                  //4
    }
    else if(i==5)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 4;
        *pLCDRam = 0x0B;  
		pLCDRam--;
		*pLCDRam = 0x06;                  //5     
    }
    else if(i==6)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 4;
        *pLCDRam = 0x0F;  
		pLCDRam--;
		*pLCDRam = 0x06;                  //6
    }
    else if(i==7)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 4;
        *pLCDRam = 0x01;  
		pLCDRam--;
		*pLCDRam = 0x05;                   //7
    }
    else if(i==8)
    {
		 pLCDRam = &LCD_RAM;
		 pLCDRam += 4;
		 *pLCDRam = 0x0F;
		 pLCDRam--;
		 *pLCDRam = 0x07;			//8

    }
	    else if(i==9)
    {
		 pLCDRam = &LCD_RAM;
		 pLCDRam += 4;
		 *pLCDRam = 0x0B;
		 pLCDRam--;
		 *pLCDRam = 0x07;			//9

    }
}
void LCD_Display4(uint8 i)				  //LCD第一位
{
	if(i==1)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 12;
        *pLCDRam = 0x03;               	  //1
    }
	else if(i==2)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 12;
        *pLCDRam = 0x0D;             	  //2
    }
}
void LCD_Display5(uint8 i)				  //LCD第二位
{
	if(i==0)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 11;
        *pLCDRam = 0x0F;  
		pLCDRam--;
		*pLCDRam = 0x05;               	  //0
    }

	else if(i==1)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 10;
        *pLCDRam = 0x05;             	  //1
    }
	else if(i==2)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 11;
        *pLCDRam = 0x0D;  
		pLCDRam--;
		*pLCDRam = 0x03;               	  //2
    }
    else if(i==3)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 11;
        *pLCDRam = 0x09;  
		pLCDRam--;
		*pLCDRam = 0x07;               	  //3
    }
    else if(i==4)
    {
       pLCDRam = &LCD_RAM;	
        pLCDRam += 11;
        *pLCDRam = 0x02;  
		pLCDRam--;
		*pLCDRam = 0x07;                  //4
    }
    else if(i==5)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 11;
        *pLCDRam = 0x0B;  
		pLCDRam--;
		*pLCDRam = 0x06;                  //5      
    }
    else if(i==6)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 11;
        *pLCDRam = 0x0F;  
		pLCDRam--;
		*pLCDRam = 0x06;                  //6
    }
    else if(i==7)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 11;
        *pLCDRam = 0x01;  
		pLCDRam--;
		*pLCDRam = 0x05;                   //7
    }
    else if(i==8)
    {
		 pLCDRam = &LCD_RAM;
		 pLCDRam += 11;
		 *pLCDRam = 0x0F;
		 pLCDRam--;
		 *pLCDRam = 0x07;			//8

    }
	else if(i==9)
    {
		 pLCDRam = &LCD_RAM;
		 pLCDRam += 11;
		 *pLCDRam = 0x0B;
		 pLCDRam--;
		 *pLCDRam = 0x07;			//9

    }

}
void LCD_Display6(uint8 i)				  //LCD第三位
{

	if(i==0)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 9;
        *pLCDRam = 0x0F;  
		pLCDRam--;
		*pLCDRam = 0x05;               	  //0
    }
	else if(i==1)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 8;
        *pLCDRam = 0x05;             	  //1
    }
	else if(i==2)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 9;
        *pLCDRam = 0x0D;  
		pLCDRam--;
		*pLCDRam = 0x03;               	  //2
    }
    else if(i==3)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 9;
        *pLCDRam = 0x09;  
		pLCDRam--;
		*pLCDRam = 0x07;               	  //3
    }
    else if(i==4)
    {
       pLCDRam = &LCD_RAM;	
        pLCDRam += 9;
        *pLCDRam = 0x02;  
		pLCDRam--;
		*pLCDRam = 0x07;                  //4
    }
    else if(i==5)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 9;
        *pLCDRam = 0x0B;  
		pLCDRam--;
		*pLCDRam = 0x06;                  //5     
    }
    else if(i==6)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 9;
        *pLCDRam = 0x0F;  
		pLCDRam--;
		*pLCDRam = 0x06;                  //6
    }
    else if(i==7)
    {
        pLCDRam = &LCD_RAM;	
        pLCDRam += 9;
        *pLCDRam = 0x01;  
		pLCDRam--;
		*pLCDRam = 0x05;                   //7
    }
    else if(i==8)
    {
		 pLCDRam = &LCD_RAM;
		 pLCDRam += 9;
		 *pLCDRam = 0x0F;
		 pLCDRam--;
		 *pLCDRam = 0x07;			//8

    }
	    else if(i==9)
    {
		 pLCDRam = &LCD_RAM;
		 pLCDRam += 9;
		 *pLCDRam = 0x0B;
		 pLCDRam--;
		 *pLCDRam = 0x07;			//9

    }
}

void LCD_heartup(void)				   //心跳
{
   pLCDRam = &LCD_RAM;	
   pLCDRam += 2;
   *pLCDRam = 0x08;  
}
void LCD_heartdown(void)
{
   pLCDRam = &LCD_RAM;
   pLCDRam += 2;	
   *pLCDRam = 0x00;  
}	  
void AD_display(void)
{
		uint8  i,j,k;
        if(FILTER_M()<10)
		{
		    i = FILTER_M();
		    LCD_Display3(i);
			Delay_10us(2000);
		}
		else if((FILTER_M()>=10)&&(FILTER_M()<100))
		{
		    i = FILTER_M()%10;
		    LCD_Display3(i);
		  	j = FILTER_M()/10;
		    LCD_Display2(j);
		    Delay_10us(2000);
		}
	    else if(FILTER_M()>99) 
		{ 
	 	    i = (FILTER_M()%100)%10;
		    LCD_Display3(i);
		    j = (FILTER_M()%100)/10;
		    LCD_Display2(j);
		    k = FILTER_M()/100;
		    LCD_Display1(k);
		    Delay_10us(2000);		 
		}
	if(FILTER_L()<10)
		{
		    i = FILTER_L();
		    LCD_Display6(i);
			Delay_10us(2000);
		}
		else if((FILTER_L()>=10)&&(FILTER_L()<100))
		{
		    i = FILTER_L()%10;
		    LCD_Display6(i);
		  	j = FILTER_L()/10;
		    LCD_Display5(j);
		    Delay_10us(2000);
		}
	    else if(FILTER_L()>99) 
		{ 
	 	    i = (FILTER_L()%100)%10;
		    LCD_Display6(i);
		    j = (FILTER_L()%100)/10;
		    LCD_Display5(j);
		    k = FILTER_L()/100;
		    LCD_Display4(k);
		    Delay_10us(2000);		 
		}					  
		Delay_10us(20000);	
}