#ifndef	__NIBP_ADC_H
#define	__NIBP_ADC_H

extern unsigned short int FILTER_L();
extern unsigned short int FILTER_M();
extern uint8 xdata ADCL_Temp;
extern uint8 xdata ADCM_Temp;
void ADC_Meas_CH1_Init(void);

void Read_ADC_Data(void);

#endif

