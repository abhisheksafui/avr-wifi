#ifndef SHIFT_H
#define SHIFT_H

#include<utilmacro.h>
#include<avr/io.h>
#include<util/delay_basic.h>

#define CLK_DATA(LATCH)  do{\
				SETBIT(*(LATCH->port),(LATCH->pin_c));\
				_delay_loop_1(1);\
				CLRBIT(*(LATCH->port),LATCH->pin_c);\
			  }while(0)
#define LATCH_DATA(LATCH) do{\
                                SETBIT(*(LATCH->port),LATCH->pin_l);\
				_delay_loop_1(1);\
                                CLRBIT(*(LATCH->port),LATCH->pin_l);\
                          }while(0) 


typedef struct shift_595_cb {
	volatile uint8_t *port;	
	volatile uint8_t *port_dir;	
	uint8_t pin_d;
	uint8_t pin_c;
	uint8_t pin_l;

}SHIFT_595_CB;


//inline void shift_byte(uint8_t data, SHIFT_595_CB *latch);
inline void shift_data(uint32_t data,uint8_t len, SHIFT_595_CB *latch);
#endif
