#include "dht11.h"
#include "sys_init.h"
#include "uart.h"

unsigned char sTemp;
unsigned char sHumidity;

//void Delay10us(void)
//{ 
//  unsigned short i;
//  for(i=0;i<106;i++)
//  {
//    asm("NOP");
//  }
//}

#pragma optimize=none
static char dht11_read_bit(void)
{
  int i = 0;
  
  while (!COM_R);
  for (i=0; i<200; i++) {
    if (COM_R == 0) break;
  }
  if (i<30)return 0;  //30us
  return 1;
}

#pragma optimize=none
static unsigned char dht11_read_byte(void)
{
  unsigned char v = 0, b;
  int i;
  for (i=7; i>=0; i--) {
    b = dht11_read_bit();
    v |= b<<i;
  }
  return v; 
}



void dht11_io_init(void)
{
  P0SEL  &= ~0x20;          //P1为普通 I/O 口
  COM_OUT;
  COM_SET;  
}
unsigned char dht11_temp(void)
{
  return sTemp;
}
unsigned char dht11_humidity(void)
{
  return sHumidity;
}

void dht11_update(void)
{
  int flag = 1;
  unsigned char dat1, dat2, dat3, dat4, dat5, ck;
  
  //主机拉低18ms 
  COM_CLR;
  halWait(18);
  COM_SET;
  
  flag = 0;
  while (COM_R && ++flag);
  if (flag == 0) return;
  
  //总线由上拉电阻拉高 主机延时20us
  //主机设为输入 判断从机响应信号  
  //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  	    
  flag = 0;
  while (!COM_R && ++flag);
  if (flag == 0) return;
  flag = 0;
  while (COM_R && ++flag);
  if (flag == 0) return;
  
  
  dat1 = dht11_read_byte();
  
  dat2 = dht11_read_byte();
  
  dat3 = dht11_read_byte();
   
  dat4 = dht11_read_byte();  
  
  dat5 = dht11_read_byte();            
  
  ck = dat1 + dat2 + dat3 + dat4;
  
  if (ck == dat5) {
    sTemp = dat3;
    sHumidity = dat1;        
  }
 
#ifdef SPI_LCD
  char  dbuf[20] = {0};
  sprintf(dbuf,"{A0=%u,A1=%u}",dat1,dat3); //A0表示湿度，A1表示温度
  Uart_Send_String(dbuf);
#else
  printf("湿度: %u.%u%% 温度: %u.%u℃ \r\n", dat1,dat2, dat3,dat4);  
#endif
  // 
}
