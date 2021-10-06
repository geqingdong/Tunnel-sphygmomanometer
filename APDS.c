																					   /** 
  ****************************************************************************** 
  读取APDS9933 I2C 文件 
  APDS9933包括读写两个操作，写又包括两种:写寄存器、写动作(即写操作直接触发动作) 
  下文中CMD表示COMMAND寄存器，实际是一个命令值，用于不同的操作，具体含义详见手册。 
  写寄存器的顺序是:S addr W A CMD(0x80)|reg A DataL (DataH) A P 
  写动作的顺序是: S addr W A CMD(0b111xxxxx) 其中x代表不同的清除中断动作，详见手册 
  读的操作顺序:S addr W A CMD(0xA0)|reg A Sr addr R A DataL (DataH) NA P 
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
  }; /*阻塞式等待*/  
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
    isI2C_Busy = TRUE;  /*做阻塞标记*/  
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
    if (cnt < I2C_RW_BLOCK_CNT){  /*写成功*/  
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
  while (isI2C_Busy == TRUE)  {WAIT_FOR_OVERTIME;} /*阻塞式等待*/  
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
        if (cnt < I2C_RW_BLOCK_CNT){  /*读成功*/  
          err = SUCCESS;  
        }  
      }  
  
      if (err == SUCCESS){  /*读成功*/  
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
  /*等待LEN2写寄存器结束*/  
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
        /*立即进入接收轮循*/  
        cnt = 0;  
        while ((I2C_GetFlagStatus(I2C_FLAG_RXNOTEMPTY) == RESET)){WAIT_FOR_OVERTIME;} /* Poll on RxNE */  
        if (cnt < I2C_RW_BLOCK_CNT){  
          /*1个字节已经收到*/  
          I2C_AcknowledgeConfig(I2C_ACK_NONE);  /*立即关闭ACK*/  
          I2C_GenerateSTOP(ENABLE);  /*立即发送停止信号，本字节接收完自动停止*/  
          i2cRXData.buf[Rx_Idx++] = I2C_ReceiveData(); /*读第1字节*/  
          cnt = 0;  
          while ((I2C_GetFlagStatus(I2C_FLAG_RXNOTEMPTY) == RESET)){WAIT_FOR_OVERTIME;} /* Poll on RxNE */  
          if (cnt < I2C_RW_BLOCK_CNT){  
            i2cRXData.buf[Rx_Idx++] = I2C_ReceiveData(); /*读第2字节*/   
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
        if (I2C_RW != I2C_READ_R){  /*发送和接收第一阶段*/  
          rw = I2C_DIRECTION_TX;  
          //I2C_ITConfig(I2C_IT_BUF, ENABLE);  
        }  
        else {   /*接收第二阶段,只会处理LEN3X,LEN1*/  
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
#if 0      /*STM8S存储空间不足，1字节处理方案忍痛砍掉*/  
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
#if 0   /*STM8S存储空间不足，3字节处理方案忍痛砍掉*/  
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
            /*开启RXNE中断，准备接收最后一个字节*/  
            I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_BUF) , ENABLE);  
          }  
          else {  
            i2cRXData.buf[Rx_Idx++] = I2C_ReceiveData();  
            DEBUG_I2C("N<3x RXData:0x%x\n",i2cRXData.buf[Rx_Idx-1]);  
          }  
          break;           
        case I2C_LEN_2:  /*LEN2的NACK来不及进入中断处理，只能采用while方式*/  
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
        isI2C_Busy = FALSE;  /*读操作结束，清除阻塞标记*/  
        isReadFinish = TRUE;  
      }  
      else {  
        /*LEN2时来不及进入RXNE中断，只能使用while等待模式*/  
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
            isI2C_Busy = FALSE;  /*写操作结束，清除阻塞标记*/  
            isWriteFinish = TRUE;  
            I2C_RW = I2C_RW_NULL;  
          }  
          break;  
        case I2C_READ_W:  
          I2C_RW = I2C_READ_R; /*切换到读第二阶段*/  
          if (Rx_SafeLen == I2C_LEN_2) {  /*LEN2结束中断之旅，回归while方式*/  
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
      DEBUG_I2C("非法 event: %x, ", event);  
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
  Atime = 0xff; // 2.7 ms – minimum ALS integration time  
  Wtime = 0xff; // 2.7 ms – minimum Wait time  
  Ptime = 0xff; // 2.7 ms – minimum Prox integration time  
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