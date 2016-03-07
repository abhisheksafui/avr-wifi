/* ledblink.c, an LED blinking program */
#include<avr/io.h>
#include<util/delay.h>
#include<uart.h>
#include<avr/interrupt.h>
#include<avr/pgmspace.h>
#include<math.h>
#include<string.h>
#include <util.h>
#include<avr-wifi.h>




const char *page[] = {
  web_page_title,
  web_page_body1,
  web_page_body2,
  web_page_body3,
  web_page_end
};

void
sleep (int millisec)
{
  while (millisec)

    {
      _delay_ms (1);		/* 1 ms delay */
      millisec--;
    }
}

void
flash_led (int n)
{
  while ((n--) > 0)
    {
      PORTC |= (1 << PC7);
      sleep (100);
      PORTC &= ~(1 << PC7);
      sleep (600);
    }

}

void
status_led_update (void)
{
  static uint8_t state = 0;
  switch (state)
    {
    case 0:
      state = 1;
      SETBIT (PORTC, PC7);

      break;
    case 1:
      state = 3;
      SETBIT (PORTC, PC6);

      break;

    case 3:
      state = 2;
      CLRBIT (PORTC, PC7);

      break;

    case 2:
      state = 0;
      CLRBIT (PORTC, PC6);

    }


}

void
adc_init (void)
{
  ADMUX = _BV (REFS1) | _BV (REFS0) | _BV (ADLAR);	/*ADC0 and internal 2.56V left adjusted */
  ADCSRA = _BV (ADEN) | _BV (ADIE) | _BV (ADPS2) | _BV (ADPS1) | _BV (ADPS0);	/*ADC enable, Interrupt Enable, F_CPU/128 ADC Clock */


}

void
start_adc_conversion (uint8_t channel)
{
  ADMUX &= ~((1 << 5) - 1);
  ADMUX |= ((1 << 5) - 1) & channel;
//      _delay_ms(50);
  ADCSRA |= _BV (ADSC);

}

volatile uint32_t timer_int1 = 0;
void
timer0_init (void)
{
/*CTC mode for generating interrupt every X us*/
  TCCR0 = _BV (WGM01);
#define X 10
#define PRE 8.0
  OCR0 = (X / 1000000.0) * (F_CPU / PRE);
  //OCR0 = 10;
  TCNT0 = 0;

  TIMSK = _BV (OCIE0);		/*Enable Compare match interrupt */

}

void
start_timer0 (void)
{
  //TCCR0 |= _BV(CS00) | _BV(CS02);
  TCCR0 |= _BV (CS01);

}

void
stop_timer0 (void)
{
  //TCCR0 &= ~(_BV(CS00) | _BV(CS02));
  TCCR0 &= ~_BV (CS01);
}

volatile uint8_t sel_channel = 0;
ISR (TIMER0_COMP_vect)		//Interrupt will be called every Xus
{

#define Y 500			//event every Y ms
  if (timer_int1++ == 50000)
    {
      timer_int1 = 0;
      //event function
      //channel = (channel+1)%2;
      // start_adc_conversion(channel); 
      sel_channel++;
      if (sel_channel == 4)
	{
	  channel = (channel + 1) % 2;
	  sel_channel = 0;
	}
      start_adc_conversion (channel);
      status_led_update ();
    }
}

void
timer1_init ()
{

  DDRD |= _BV (PD4);
  TCCR1A = _BV (COM1B1) | _BV (WGM10) | _BV (WGM11);
  TCCR1B = _BV (WGM13) | _BV (WGM12);
  TCNT1 = 0;
  OCR1A = ceil ((F_CPU / 8.0) / 16.35);
  OCR1B = 0.4 * OCR1A;
//ceil((F_CPU/8.0)/262.0);

}

void
start_timer1 (void)
{

  TCCR1B |= 2;


}

void
pause_timer1 (void)
{
  TCCR1B &= ~0x02;
}

void
tone (double n)
{
  uint16_t i = ceil ((F_CPU / 8.0) / n);
  OCR1A = i;
  OCR1B = 65;
  start_timer1 ();
}

void
setup_leds (void)
{

  SETBIT (DDRC, LED1_PIN);
  SETBIT (DDRC, LED2_PIN);

}

void
uart_gets (char *s, uint8_t len)
{
  char c = 0;
  char m = 0;
  uint8_t i = 0;
  do
    {
      /*if buffer is full-1 then end string and exit */
      if (i == (len - 1))
	{
	  s[i] = 0;
	  break;
	}

      /*other wise proceed to get the char */
      c = (char) uart_getc ();
      /* if it is \n or \r then it can be either at begining or at end */
      if (c == '\r' || c == '\n')
	{
	  /* this case is when 1st character is \r or \n */
	  if (m == 0)
	    continue;
	  else			/* this is when we get \r or \n at end of line */
	    {
	      s[i] = 0;
	      break;
	    }
	}

      /* if its not \r or \n and it is not end of buffer then store it and increment */
      s[i] = c;
      m = 0x5a;

      i++;
    }
  while (1);

}

void
uart_puts_pgm (PGM_P p)
{
  unsigned char *c;
  uint8_t i = 0;
  while ((c = pgm_read_byte (p + i)) != 0)
    {
      uart_putc (c);
      i++;
    }
}

void
process_cmds ()
{
  char cmd[MAXSTRLEN];
  uint8_t i = 0, data_len = 0;
  char tmp[40];
  char *handle = NULL, char_p;
  uint16_t data_0 = adc_data[0];
  uint16_t data_1 = adc_data[1];
  float temp = ((data_0 / 1024.0) * 2.56) * 100;
  float pot = ((data_1 / 1024.0) * 2.56);
  snprintf (tmp, sizeof (tmp), "%.2f Celcius</p>", temp);
  data_0 = strlen (tmp);
  snprintf (tmp, sizeof (tmp), "%.2f Volts</p>", pot);
  data_1 = strlen (tmp);
  snprintf (tmp, sizeof (tmp), "%s </p>", ip);
  uint16_t data_2 = strlen (tmp);

  uart_gets (cmd, MAXSTRLEN);
  if (!strncmp (cmd, "+IPD", 4))
    {
      strtok (cmd, ",");
      handle = strtok (NULL, ",");
      strtok (NULL, ":");
      char_p = strtok (NULL, " ");
      if (!strncmp (char_p, "GET", 3))
	{
	  char_p = strtok (NULL, " ");
	  if (!strcmp (char_p, "/"))
	    {
		//Here you serve index.html
	      //flash_led(4); 
	      data_len = (uint8_t) (strlen_P (web_page_title) +
				    strlen_P (web_page_body1) +
				    strlen_P (web_page_body2) +
				    strlen_P (web_page_body3) +
				    strlen_P (web_page_body4) +
				    data_0 + data_1 + data_2 +
				    strlen_P (web_page_end));
	      strtok (cmd, ",");
	      handle = strtok (NULL, ",");
	      snprintf (tmp, sizeof (tmp), "AT+CIPSEND=%s,%d\r\n", handle,
			data_len);
	      uart_puts (tmp);
	      /*wait for prompt */
	      //while(uart_getc()!='>'); 
	      uart_puts_pgm (web_page_title);
	      uart_puts_pgm (web_page_body1);
	      uart_puts_pgm (web_page_body2);
	      snprintf (tmp, sizeof (tmp), "%.2f Celcius</p>", temp);
	      uart_puts (tmp);
	      uart_puts_pgm (web_page_body3);
	      snprintf (tmp, sizeof (tmp), "%.2f Volts</p>", pot);
	      uart_puts (tmp);
	      uart_puts_pgm (web_page_body4);
	      snprintf (tmp, sizeof (tmp), "%s </p>", ip);
	      uart_puts (tmp);
	      uart_puts_pgm (web_page_end);

	      _delay_ms (500);
	      snprintf (tmp, sizeof (tmp), "AT+CIPCLOSE=%s\r\n", handle);
	      uart_puts (tmp);
	      _delay_ms (500);
	    }
	  else
	    {
	      //Here you serve according to requested page
	      char_p = strtok (NULL, "?");
	      if (!strcmp (char_p, "/ap"))
		{

		  char_p = strtok (NULL, "=");
		  if (strcmp (char_p, "id"))
		    {
		      //error
		    }
		  char_p = strtok (NULL, "&");
		  //store id to eeprom

		  char_p = strtok (NULL, "=");
		  if (strcmp (char_p, "passwd"))
		    {
		      //error
		    }
		  char_p = strtok (NULL, "\0");
		  //store id to eeprom


		}

	    }
	}
	else	
	{
		//Here you process other custom commands
	}
    }

}

void
get_ip_address (void)
{
  char line[50];
  char *p;
  uart_flush ();
  uart_puts ("AT+CIFSR\r\n");
  *ip = 0;
  while (!uart_available ());
  while (uart_available ())
    {
      _delay_ms (80);
      uart_gets (line, 50);
      if (strstr (line, "IP"))
	{
	  p = strstr (line, "\"");
	  strcat (ip, strtok (p + 1, "\""));
	  strcat (ip, " ");
	}

    }
  uart_flush ();
  return;

}

int
main ()
{
  int i = 0;
  timer1_init ();		//FAST PWM with OCRA1 as top and OCR1B determining the DUTY CYCLE. OC1B pin as output
  // Timer 1 used for BUZZER TONE generation
  timer0_init ();
  //start_timer0();
  adc_init ();
  uart_init (UART_BAUD_SELECT_DOUBLE_SPEED (115200, F_CPU));
  setup_leds ();
  sei ();
  flash_led (2);
  _delay_ms (2000);

  uart_puts ("ATE0\r\n");
  _delay_ms (500);
  uart_flush ();

  uart_puts ("AT\r\n");
  _delay_ms (500);
//      tone(pgm_read_float(&notes[6]));
//      _delay_ms(1000);
//      pause_timer1();
//      PORTD &= ~_BV(PD4);
//      do
//      {
//         while((i==0) && ((data[i]=(char)uart_getc()) == 0)); //wait till data arrives
//         data[i]=(char)uart_getc();
//      }while(data[i++]!= 0);
//      i--;    
  flash_led (uart_available ());

  uart_puts ("AT+CWMODE=3\r\n");
  _delay_ms (500);
  uart_puts ("AT+CWJAP=\"Abhishek\",\"85128512\r\n");
  _delay_ms (500);
  //uart_puts("AT+CIFSR\r\n");
  //_delay_ms(500);
//      uart_puts("AT+PING=\"192.168.0.1\"\r\n");
//      _delay_ms(500);
  uart_flush ();
  uart_puts ("AT+CWSAP=\"Safui\",\"kittycaddy\",11,2\r\n");
  _delay_ms (500);
  uart_flush ();
  _delay_ms (500);
  uart_puts ("AT+CIPMUX=1\r\n");
  _delay_ms (500);
  uart_puts ("AT+CIPSERVER=1,80\r\n");
  uart_flush ();
  _delay_ms (2000);
  get_ip_address ();
  /*for(i=0; i<12; i++)
     {
     tone(notes[i]);
     _delay_ms(1000);

     }            
     pause_timer1(); */
  start_timer0 ();
  while (1)
    {
      if (uart_available ())
	{
	  stop_timer0 ();
	  while (uart_available ())
	    {
	      //flash_led(5);
	      _delay_ms (100);	//let the data flow in first
	      //uart_gets(cmd,MAXSTRLEN);
	      process_cmds ();
	    }
	  start_timer0 ();
	}
    }
}

ISR (ADC_vect)
{
  uint8_t datal = ADCL;
  uint8_t datah = ADCH;
  adc_data[channel] = (datah << 2) | (datal >> 6);
//       channel = (channel+1)%2;
//      start_adc_conversion(channel); 

}
