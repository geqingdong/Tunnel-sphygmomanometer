#ifndef _NIBP_UART_H
#define _NIBP_UART_H



void UART_Init(void);
void UART_Tx_ADC(void);
void UART_Tx_Byte(uint8 Tx_data);
void UART_1st_Tx(void);
void UART_Init(void);
void UART_Tx_String(uint8 *s);
void UART_Tx_Data(uint32 Data);

void UART_Tx_Data2x(uint16 Data);
void UART_Tx_Data3x(uint16 Data);
void UART_Tx_Data5x(uint16 Data);
void UART_Tx_Data8x(uint32 Data);

#endif


