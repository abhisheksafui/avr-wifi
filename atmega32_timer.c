#include<atmega32_timer.h>
#include<avr/io.h>
#include<util.h>

void  timer0_init(TIMER_CB * timer_cb)
{
        if((timer_cb->mode) == NORMAL)
        {
                CLRBIT(TCCR0,WGM00);
                CLRBIT(TCCR0,WGM01);
        }

}

void start_timer0(TIMER_CB * timer_cb)
{
        TCNT0 = timer_cb->start_val;
        TCCR0 |= (timer_cb->src);
}


void timer1_init(TIMER_CB *timer_cb)
{


}
