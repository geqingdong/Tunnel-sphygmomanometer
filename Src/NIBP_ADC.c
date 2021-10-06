#include <SZC909.H>
#include "SystemConstant.h"
#include "NIBP_ADC.h"
#include "NIBP_Timer.h"
#define N 22 
uint8 xdata ADCL_Temp;
uint8 xdata ADCM_Temp;
void ADC_Meas_CH1_Init(void)
{
    ADCM1 &= 0xFE;				//Disable ADC			
	VREG = 0xD8;				//Enable Band Gap & AVDDR & ACM + AVDDR = 3.3v + Charge pump clock = 500k Hz + Charge pump mode = Disable Mode
	AMPM = 0x23;				//Disable GX Buf + PGIA chopper clock = 7.8kHz(/32) + PGIA Gain = x1 + Enable PGIA Chopper & PGIA 	
	ADCM1= 0x56;				//Disable GR Buf + Vref = 0.99v + ADC Gain x1 + OP1/OP2 Chopper Enable + "Disable ADC	"(last enable)
	ADCM2= 0x14;				//ADC Clock = 250kHz + OSR = 1953(2k)Hz(250k/128) + DRDY flag clear
	CHS	 = 0x10;				//AIN1 + AIN2
	OPM = 0x1F;	
	ADCM1 |= 0x01;			    //Enable ADC	
}

void Read_ADC_Data(void)
{
    if(ADCM2 & 0x01)
    {
        ADCM2 = ADCM2 & 0xFE;	
        ADCL_Temp = ADCDL;
		    ADCM_Temp = ADCDM;
    }
}
 unsigned short int FILTER_M()                                //����ƽ���˲�  NΪ���г���
{
        unsigned short int sum1=0;
        unsigned char count=0;
        static unsigned char i=0,num=0;
        static unsigned short int value_temp[N];
        do{
        if(i<N)                                                     //ÿ�β�����һ�������ݷ����β�����ӵ�ԭ�����׵�һ�����ݡ����Ƚ��ȳ�ԭ��
        {                                                           //�Ѷ����е�N�����ݽ�������ƽ�����㣬�Ϳɻ���µ��˲����
                value_temp[i] = ADCM_Temp;
                i++;
                i=(i==N)?0:i;                                        //��Ŀ���� i����
        }
        num++;
        num=num<N?num:N;
}while(num<N);
        for(count=0;count<N;count++)                
                sum1 += value_temp[count];
      
        sum1 = sum1/N;        
        return sum1;
}
 unsigned short int FILTER_L()                                //����ƽ���˲�  NΪ���г���
{
        unsigned short int sum2=0;
        unsigned char count=0;
        static unsigned char i=0,num=0;
        static unsigned short int value_temp[N];
        do{
        if(i<N)                                                     //ÿ�β�����һ�������ݷ����β�����ӵ�ԭ�����׵�һ�����ݡ����Ƚ��ȳ�ԭ��
        {                                                           //�Ѷ����е�N�����ݽ�������ƽ�����㣬�Ϳɻ���µ��˲����
                value_temp[i] = ADCL_Temp;
                i++;
                i=(i==N)?0:i;                                        //��Ŀ���� i����
        }
        num++;
        num=num<N?num:N;
}while(num<N);
        for(count=0;count<N;count++)                
                sum2 += value_temp[count];
      
        sum2 = sum2/N;        
        return sum2;
}









