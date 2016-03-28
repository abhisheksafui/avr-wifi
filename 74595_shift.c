#include<74595_shift.h>
#include<util/delay.h>
#include<util/delay_basic.h>

/*void shift_byte(uint8_t data, SHIFT_595_CB  *latch)
{
        uint8_t  i=0;
	
        for(i=0;i<8;i++)
        {
                if(CHKBIT(data,i))
                {
			*(latch->port) |= (1<<(latch->pin_d));
                }
                else
                {
			*(latch->port) &= ~(1<<(latch->pin_d));
                }
		*(latch->port) |= (1<<(latch->pin_c));
		_delay_us(1);		
		*(latch->port) &= ~(1<<(latch->pin_c));
        }
}*/
void shift_data(uint32_t data,uint8_t len,SHIFT_595_CB  *latch)
{

        uint8_t  i=0;
	
        for(i=0;i<len;i++)
        {
                if(CHKBIT(data,i))
                {
			*(latch->port) |= (1<<(latch->pin_d));
                }
                else
                {
			*(latch->port) &= ~(1<<(latch->pin_d));
                }
		*(latch->port) |= (1<<(latch->pin_c));
		_delay_loop_1(1);		
		//NOP();
		*(latch->port) &= ~(1<<(latch->pin_c));
        }
}
void init_595(SHIFT_595_CB *latch)
{
			*(latch->port) |= (1<<(latch->pin_d));
			*(latch->port) |= (1<<(latch->pin_c));
			*(latch->port) |= (1<<(latch->pin_l));
}
