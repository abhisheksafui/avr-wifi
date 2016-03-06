
volatile uint32_t timer_int1=0;
void timer0_init(void)
{
/*CTC mode for generating interrupt every X us*/
  TCCR0= _BV(WGM01);
  //OCR0 = (X/1000000)*(F_CPU/PRE);
#define X 10000
  OCR0 = (X/1000000)*(F_CPU/PRE);
  TCNT0 = 0;

  TIMSK = _BV(OCIE0);  /*Enable Compare match interrupt*/

}

ISR(TIMER0_OVF_vect) //Interrupt will be called every Xus
{

#define Y 100   //event every Y ms 
  if(timer_int1++ == (Y*1000)/X)
  {
    timer_int = 0;
    //event function      
  }
}
