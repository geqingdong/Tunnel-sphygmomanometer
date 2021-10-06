																					   /** 
  ****************************************************************************** 
  ��ȡAPDS9933 I2C �ļ� 
  APDS9933������д����������д�ְ�������:д�Ĵ�����д����(��д����ֱ�Ӵ�������) 
  ������CMD��ʾCOMMAND�Ĵ�����ʵ����һ������ֵ�����ڲ�ͬ�Ĳ��������庬������ֲᡣ 
  д�Ĵ�����˳����:S addr W A CMD(0x80)|reg A DataL (DataH) A P 
  д������˳����: S addr W A CMD(0b111xxxxx) ����x����ͬ������ж϶���������ֲ� 
  ���Ĳ���˳��:S addr W A CMD(0xA0)|reg A Sr addr R A DataL (DataH) NA P 
  ****************************************************************************** 
  */  
  
/* Includes ------------------------------------------------------------------*/  
#include "stm8s.h"  
#include "main.h"  
#include "stm8s_eval.h"  
#include "app_i2c.h"  
  
static __IO I2CDataTyp i2cTXData, i2cRXData;  
static uint8_t Rx_Idx, Tx_Idx;  
static __IO uint8_t txBuf[I2CTXBUFSIZE];  
static __IO uint8_t rxBuf[I2CRXBUFSIZE];  
static __IO I2CWRState I2C_RW;  
static __IO I2CLenType Rx_SafeLen;  
static __IO bool isI2C_Busy;  
static __IO bool isReadFinish;  
static __IO bool isWriteFinish;  
  
void I2C_APP_Init(void)  
{  
  i2cTXData.buf = txBuf;  
  i2cRXData.buf = rxBuf;  
  I2C_RW = I2C_RW_NULL;  
  isI2C_Busy = FALSE;  
  isReadFinish = FALSE;  
  isWriteFinish = FALSE;  
  /* I2C Initialize */  
  I2C_DeInit();  
  I2C_Init(I2C_SPEED, 0xA0, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);  
  /* Enable Buffer and Event Interrupt*/  
  I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_EVT /*| I2C_IT_BUF*/) , ENABLE);  
  //enableInterrupts();  
}  
  
ErrorStatus I2C_WriteRegData(APDS9933RegAddr reg, uint8_t *data, uint8_t len)  
{   
  uint8_t cnt = 0;  
  ErrorStatus err = ERROR;   
  isWriteFinish = FALSE;  
  while (isI2C_Busy == TRUE)  {  
    Delay(0x7FF);  
    cnt++;  
    if (cnt >= I2C_RW_BLOCK_CNT){  
      break;  
    }  
  }; /*����ʽ�ȴ�*/  
  if (cnt < I2C_RW_BLOCK_CNT){  
    i2cTXData.len = len;  
    i2cTXData.reg= 0x80|reg;  /*0x80:Repeated Byte protocol transaction*/  
    DEBUG_I2C("i2cTXData.buf:" );  
    for (uint8_t i=0; i<len; i++){  
      i2cTXData.buf[i] = data[i];  
      DEBUG_I2C(" 0x%x,", i2cTXData.buf[i]);  
    }  
    DEBUG_I2C(" \n" );  
    I2C_RW = I2C_WRITE;  
    isI2C_Busy = TRUE;  /*���������*/  
    //I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF) , ENABLE);  
    I2C_GenerateSTART(ENABLE);  
    cnt = 0;  
    while (isWriteFinish == FALSE){  
      Delay(0x7FF);  
      cnt++;  
      if (cnt >= I2C_RW_BLOCK_CNT){  
        break;  
      }  
    }  
    if (cnt < I2C_RW_BLOCK_CNT){  /*д�ɹ�*/  
      err = SUCCESS;  
    }  
    else{  
      err = ERROR;  
      DEBUG_I2C("I2C_WriteReg 0x%x Over time: Write over time!\n", reg);  
    }  
  }  
  else{  
    err = ERROR;  
    DEBUG_I2C("I2C_WriteReg 0x%x Over time: I2C bus busy!\n", reg);  
  }  
  return err;  
}  
  
ErrorStatus I2C_ReadRegData(APDS9933RegAddr reg, uint8_t *data, uint8_t len)   
{   
  uint8_t cnt = 0;  
  ErrorStatus err = ERROR;  
  isReadFinish = FALSE;  
  while (isI2C_Busy == TRUE)  {WAIT_FOR_OVERTIME;} /*����ʽ�ȴ�*/  
  if (cnt < I2C_RW_BLOCK_CNT){  
    if ((len>0) && (len<=I2CRXBUFSIZE)) {  
      I2C_AcknowledgeConfig(I2C_ACK_CURR);  
      if (len > 2) {  
        Rx_SafeLen = I2C_LEN_3X;  
      }  
      else if (len == 2){  
        Rx_SafeLen = I2C_LEN_2;  
      }  
      else {    /*len == 1*/  
        Rx_SafeLen = I2C_LEN_1;  
        /* Disable Acknowledgement */  
        I2C_AcknowledgeConfig(I2C_ACK_NONE);  
      }  
      i2cRXData.len = len;  
      i2cRXData.reg= 0xA0|reg; /*0xA0:Auto-Increment protocol transaction*/  
      I2C_RW = I2C_READ_W;  
      isReadFinish = FALSE;  
      if (Rx_SafeLen == I2C_LEN_2){  
        err = I2C_HandleRead2Bytes();  
      }  
      else{  /* LEN3X, LEN1 */  
        I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_EVT) , ENABLE);  
        I2C_GenerateSTART(ENABLE);  
        cnt = 0;  
        while (isReadFinish == FALSE){WAIT_FOR_OVERTIME;}  
        if (cnt < I2C_RW_BLOCK_CNT){  /*���ɹ�*/  
          err = SUCCESS;  
        }  
      }  
  
      if (err == SUCCESS){  /*���ɹ�*/  
        for (uint8_t i=0; i<len; i++){  
          data[i] = i2cRXData.buf[i];  
        }  
      }  
      else{         /* err == ERROR*/  
        DEBUG_I2C("I2C_ReadReg 0x%x Over time: Read over time!\n", reg);  
      }  
    }  
    else {  
      err = ERROR;  
      if (len == 0){  
        DEBUG_I2C("I2C_ReadReg 0x%x len=0 error!\n", reg);  
      }  
      else {  
        DEBUG_I2C("I2C_ReadReg 0x%x len overlength error!\n", reg);  
      }  
    }  
  }  
  else{  
    err = ERROR;  
    DEBUG_I2C("I2C_ReadReg 0x%x Over time: I2C bus busy!\n", reg);  
  }  
  return err;  
}  
  
ErrorStatus I2C_ReadWord(APDS9933RegAddr reg, uint16_t* data)  
{  
  ErrorStatus err = ERROR;  
  uint8_t barr[I2CRXBUFSIZE];  
  err = I2C_ReadRegData(reg, barr, I2CRXBUFSIZE);  
  if (err == SUCCESS){  
    *data = ((uint16_t)barr[1])<<8 | (uint16_t)barr[0];  
  }  
  return err;  
}  
  
ErrorStatus I2C_HandleRead2Bytes(void)  
{  
  ErrorStatus err = ERROR;  
  uint8_t cnt;  
  Rx_Idx = 0;  
  I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF) , ENABLE);  
  DEBUG_I2C("Begin to Read! \n");  
  I2C_GenerateSTART(ENABLE);  
  /*�ȴ�LEN2д�Ĵ�������*/  
  cnt = 0;  
  while (I2C_RW != I2C_READ_R){WAIT_FOR_OVERTIME;}      
  if (cnt < I2C_RW_BLOCK_CNT){  
    I2C_GenerateSTART(ENABLE); /*Sr*/  
    DEBUG_I2C("I2C_Generate ReSTART:Read \n");  
    /* Test on EV5 and clear it */  
    cnt = 0;  
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)){WAIT_FOR_OVERTIME;}  
    if (cnt < I2C_RW_BLOCK_CNT){  
      DEBUG_I2C("Send slave address: 0x%x\n",SLAVE_ADDRESS);  
      I2C_AcknowledgeConfig(I2C_ACK_NEXT);  
      I2C_Send7bitAddress(SLAVE_ADDRESS, I2C_DIRECTION_RX);  
      cnt = 0;  
      while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){WAIT_FOR_OVERTIME;}  
      if (cnt < I2C_RW_BLOCK_CNT){  
        I2C_AcknowledgeConfig(I2C_ACK_NONE);  
        DEBUG_I2C("Enter Receiver Mode!\n");  
        /*�������������ѭ*/  
        cnt = 0;  
        while ((I2C_GetFlagStatus(I2C_FLAG_RXNOTEMPTY) == RESET)){WAIT_FOR_OVERTIME;} /* Poll on RxNE */  
        if (cnt < I2C_RW_BLOCK_CNT){  
          /*1���ֽ��Ѿ��յ�*/  
          I2C_AcknowledgeConfig(I2C_ACK_NONE);  /*�����ر�ACK*/  
          I2C_GenerateSTOP(ENABLE);  /*��������ֹͣ�źţ����ֽڽ������Զ�ֹͣ*/  
          i2cRXData.buf[Rx_Idx++] = I2C_ReceiveData(); /*����1�ֽ�*/  
          cnt = 0;  
          while ((I2C_GetFlagStatus(I2C_FLAG_RXNOTEMPTY) == RESET)){WAIT_FOR_OVERTIME;} /* Poll on RxNE */  
          if (cnt < I2C_RW_BLOCK_CNT){  
            i2cRXData.buf[Rx_Idx++] = I2C_ReceiveData(); /*����2�ֽ�*/   
            cnt = 0;  
            while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY)){WAIT_FOR_OVERTIME;}  
            if (cnt < I2C_RW_BLOCK_CNT){  
              isI2C_Busy = FALSE;  
              isReadFinish = TRUE;  
              I2C_RW = I2C_RW_NULL;  
              err = SUCCESS;  
            }  
          }  
        }  
      }  
    }  
  }  
  return err ;  
}  
  
void I2C_APP_Interrupt(void)  
{  
  I2C_Direction_TypeDef rw;  
  I2C_Event_TypeDef event = I2C_GetLastEvent();  
  DEBUG_I2C("interrupt event: 0x%x, ", event);  
  DEBUG_I2C("SR2= 0x%x => ", I2C->SR2);  
  switch (event)  
  {  
    case I2C_EVENT_MASTER_MODE_SELECT_RESTART: //(0x0300)  
      DEBUG_I2C("I2C_EVENT_MASTER_MODE_SELECT_RESTART \n");  
      break;  
      /* EV5 */  
    case I2C_EVENT_MASTER_MODE_SELECT :   //SB (ITEVTEN)       
      if (I2C_RW != I2C_RW_NULL) {  
        DEBUG_I2C("I2C_EVENT_MASTER_MODE_SELECT \n");  
        if (I2C_RW != I2C_READ_R){  /*���ͺͽ��յ�һ�׶�*/  
          rw = I2C_DIRECTION_TX;  
          //I2C_ITConfig(I2C_IT_BUF, ENABLE);  
        }  
        else {   /*���յڶ��׶�,ֻ�ᴦ��LEN3X,LEN1*/  
          rw = I2C_DIRECTION_RX;  
          I2C_RW = I2C_RW_NULL;  
        }  
      /* Send slave Address for write */  
        DEBUG_I2C("RW=%d (0:tx, 1:rx)\n",rw);  
        I2C_Send7bitAddress(SLAVE_ADDRESS, rw);  
      }  
      else {  
        DEBUG_I2C("I2C_EVENT_MASTER_MODE_SELECT error:I2C_RW=%d \n",I2C_RW);  
      }  
      break;  
  
      /* EV6 TRA, BUSY, MSL, TXE and ADDR  flags*/  
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:  //0x0782 (TXE(ITBUFEN) ADDR) (ITEVTEN)  
      DEBUG_I2C("I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED \n");  
      switch (I2C_RW)  
      {  
        case I2C_WRITE:  
          /* Send the first Data */  
          DEBUG_I2C("I2C_WRITE=>i2cTXData.reg:0x%x",i2cTXData.reg);  
          DEBUG_I2C("(%d) \n",i2cTXData.reg);  
          Tx_Idx = 0;  
          I2C_SendData(i2cTXData.reg);  
          break; /*next: I2C_EVENT_MASTER_BYTE_TRANSMITTED*/  
        case I2C_READ_W:  
          /* Send the first Data */  
          DEBUG_I2C("I2C_READ_W=>i2cRXData.reg:0x%x",i2cRXData.reg);  
          DEBUG_I2C("(%d) \n",i2cRXData.reg);  
          I2C_SendData(i2cRXData.reg);  
          break; /*next: I2C_EVENT_MASTER_BYTE_TRANSMITTED*/  
        default:  
          DEBUG_I2C("TRANSMITTER_MODE_SELECTED error:I2C_RW=%d \n",I2C_RW);  
          break;  
      }  
      break;  
        
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: //RX:ADDR; 0x0302 TRA=0, BUSY MSL (ADDR) (ITEVTEN)  
      //DEBUG_I2C("I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED \n");  
      Rx_Idx = 0;  
#if 0      /*STM8S�洢�ռ䲻�㣬1�ֽڴ�������ʹ����*/  
      switch (Rx_SafeLen)  
      {       
        case I2C_LEN_1:  
          /* Disable Acknowledgement */  
          I2C_AcknowledgeConfig(I2C_ACK_NONE);  
          I2C_ITConfig(I2C_IT_BUF, ENABLE);  
           /* Send STOP Condition */  
          I2C_GenerateSTOP(ENABLE);  
          break;  
        default:  
          break;  
      }  
#endif        
      break;  
      /* EV7 */  
#if 0   /*STM8S�洢�ռ䲻�㣬3�ֽڴ�������ʹ����*/  
    case I2C_EVENT_MASTER_BYTE_RECEIVED_FULL: /*RX:RXNE&BTF; 0x0344, RXNE(ITBUFEN) BTF(ITEVTEN) */  
      DEBUG_I2C("I2C_EVENT_MASTER_BYTE_RECEIVED_FULL, Rx_Idx:%d\n",Rx_Idx);  
      switch (Rx_SafeLen)  
      {  
        case I2C_LEN_3X:  
          if (Rx_Idx == (i2cRXData.len-3)){  
            /* Disable Acknowledgement */  
            I2C_AcknowledgeConfig(I2C_ACK_NONE);  
            i2cRXData.buf[Rx_Idx++] = I2C_ReceiveData(); /*Read DataN-2 in DR*/  
            DEBUG_I2C("N=3xN-2 RXData:0x%x\n",i2cRXData.buf[Rx_Idx-1]);  
            //I2C_ITConfig(I2C_IT_BUF, ENABLE);  
            /* Send STOP Condition */  
            I2C_GenerateSTOP(ENABLE);  
            i2cRXData.buf[Rx_Idx++] = I2C_ReceiveData(); /*Read DataN-1*/  
            DEBUG_I2C("N=3xN-1 RXData:0x%x\n",i2cRXData.buf[Rx_Idx-1]);  
            /*����RXNE�жϣ�׼���������һ���ֽ�*/  
            I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_BUF) , ENABLE);  
          }  
          else {  
            i2cRXData.buf[Rx_Idx++] = I2C_ReceiveData();  
            DEBUG_I2C("N<3x RXData:0x%x\n",i2cRXData.buf[Rx_Idx-1]);  
          }  
          break;           
        case I2C_LEN_2:  /*LEN2��NACK�����������жϴ���ֻ�ܲ���while��ʽ*/  
          break;  
        case I2C_LEN_1:  
          break;  
        default:  
          break;            
      }  
      break;  
#endif   
      
    case I2C_EVENT_MASTER_BYTE_RECEIVED: /*RX:RXNE; 0x0340 !< BUSY, MSL and RXNE flags*/   
    case I2C_EVENT_MASTER_BYTE_RECEIVE_FINISHED: /*0x44*/  
    case I2C_EVENT_MASTER_BYTE_RECEIVE_RXNE:  /*0x40 RX:RXNE*/  
      DEBUG_I2C("I2C_EVENT_MASTER_BYTE_RECEIVE_RXNE, Rx_Idx:%d\n",Rx_Idx);      
      if (Rx_SafeLen != I2C_LEN_2){  
        i2cRXData.buf[Rx_Idx] = I2C_ReceiveData(); /*Read DataN*/    
        I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_BUF) , DISABLE);  
        isI2C_Busy = FALSE;  /*����������������������*/  
        isReadFinish = TRUE;  
      }  
      else {  
        /*LEN2ʱ����������RXNE�жϣ�ֻ��ʹ��while�ȴ�ģʽ*/  
        DEBUG_I2C("error: I2C_LEN_2 enter RXNE!!");  
      }  
      break;  
         
      /* EV8 */  
    case I2C_EVENT_MASTER_BYTE_TRANSMITTING:  //(TXE(ITBUFEN)  ) (ITEVTEN)  
      DEBUG_I2C("I2C_EVENT_MASTER_BYTE_TRANSMITTING \n");  
      break;  
      /* EV8_2 */  
    case I2C_EVENT_MASTER_BYTE_TRANSMITTED: //(TXE(ITBUFEN) BTF) (ITEVTEN)  
      DEBUG_I2C("I2C_EVENT_MASTER_BYTE_TRANSMITTED \n");  
      switch (I2C_RW)  
      {  
        case I2C_WRITE:  
          if (i2cTXData.len != 0)  
          {  
            /* Send the first Data */  
            DEBUG_I2C("Send TxBuffer[%d]:",Tx_Idx);  
            DEBUG_I2C("0x%x",i2cTXData.buf[Tx_Idx]);  
            DEBUG_I2C("(%d) \n",i2cTXData.buf[Tx_Idx]);  
            I2C_SendData(i2cTXData.buf[Tx_Idx]);  
            Tx_Idx++;  
            /* Decrement number of bytes */  
            i2cTXData.len--;  
          }  
          else  
          {  
            /* Send STOP condition */  
            I2C_GenerateSTOP(ENABLE);  
            DEBUG_I2C("i2cTXData.buf:" );  
            for (uint8_t i=0; i<10; i++){  
              DEBUG_I2C(" 0x%x,", i2cTXData.buf[i]);  
            }  
            DEBUG_I2C(" \n" );  
            //I2C_ITConfig(I2C_IT_EVT, DISABLE);  
            isI2C_Busy = FALSE;  /*д��������������������*/  
            isWriteFinish = TRUE;  
            I2C_RW = I2C_RW_NULL;  
          }  
          break;  
        case I2C_READ_W:  
          I2C_RW = I2C_READ_R; /*�л������ڶ��׶�*/  
          if (Rx_SafeLen == I2C_LEN_2) {  /*LEN2�����ж�֮�ã��ع�while��ʽ*/  
            I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF) , DISABLE);  
          }          
          else {  
            I2C_GenerateSTART(ENABLE); /*Sr*/  
          }  
          break;  
        default:  
          DEBUG_I2C("I2C_RW err: 0x%x\n",I2C_RW);  
          break;   
      }  
      break;      
    case I2C_EVENT_SLAVE_ACK_FAILURE:  
      DEBUG_I2C("I2C_EVENT_SLAVE_ACK_FAILURE\n");  
      DEBUG_I2C("IIC Disabled \n");  
      I2C_Cmd(DISABLE);  
      //I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF) , DISABLE);  
      I2C_SoftwareResetCmd(ENABLE);  
      I2C_APP_Init();  
      DEBUG_I2C("IIC restart\n");  
      //I2C_GenerateSTART(ENABLE);  
      break;        
    default:  
      DEBUG_I2C("�Ƿ� event: %x, ", event);  
      DEBUG_I2C("SR2= %x\n", I2C->SR2);  
      I2C->SR2 = 0;  
      I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF) , DISABLE);  
      break;  
  }  
}  
  
ErrorStatus APDS9933_Init(void)  
{  
  ErrorStatus err = SUCCESS;  
  APDS_RegVal key, Atime, Ptime, Wtime, Ppluse;  
  key = 0x0;  
  Atime = 0xff; // 2.7 ms �C minimum ALS integration time  
  Wtime = 0xff; // 2.7 ms �C minimum Wait time  
  Ptime = 0xff; // 2.7 ms �C minimum Prox integration time  
  Ppluse = 8; // Minimum prox pulse count  
  err &= I2C_WriteRegData(APDS_ENABLE_REG, &key, sizeof(key));  
  err &= I2C_WriteRegData(APDS_ATIME_REG, &Atime, sizeof(Atime));  
  err &= I2C_WriteRegData(APDS_PTIME_REG, &Ptime, sizeof(Ptime));  
  err &= I2C_WriteRegData(APDS_WTIME_REG, &Wtime, sizeof(Wtime));  
  err &= I2C_WriteRegData(APDS_PPULSE_REG, &Ppluse, sizeof(Ppluse));  
  /*APDS_CONTROL_REG*/  
  APDS_RegVal CtrlVal;  
  CtrlVal = CTRLVAL;  
  err &= I2C_WriteRegData(APDS_CONTROL_REG, &CtrlVal, sizeof(CtrlVal));  
  /*APDS_ENABLE_REG*/  
  APDS_RegVal power;  
  power = POWER;  
  err &= I2C_WriteRegData(APDS_ENABLE_REG, &power, sizeof(power));  
    
  if (err == SUCCESS){  
    Delay(0x5FF);  /*wait for stable*/  
  }  
  return err;  
}  
/** 
  * @} 
  */  
  
/** 
  * @} 
  */  
  
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/  