#include <stdio.h>
#include <ioCC2530.h>
#include "dht11.h"
#include "sys_init.h"
#include "uart.h"
#include "info.h"

__near_func int putchar(int ch)
{
    U0DBUF = ch;
    while(UTX0IF == 0);
    UTX0IF = 0;
    return ch;
}


/*主函数
-------------------------------------------------------*/
void main(void)    
{ 
  xtal_init();
  led_init();  
  dht11_io_init(); 
  uart0_init(0x00, 0x00);                                        //初始化串口
#ifdef SPI_LCD
  lcd_dis();                                                    //在屏幕上显示相关信息
#endif

  while(1)
  {
    halWait(250);
    halWait(250);
    halWait(250);
    halWait(250);
    dht11_update();
    D7 = !D7;
  }
}
