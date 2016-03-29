/* ledblink.c, an LED blinking program */
#include<avr/io.h>
#include<util/delay.h>
#include<uart.h>
#include<avr/interrupt.h>
#include<avr/pgmspace.h>
#include<avr/eeprom.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <util.h>
#include<avr-wifi.h>
#include<web_server.h>
#include<led_matrix.h>
#include<font.h>
#include<string.h>


#define UPDATE_SCROLL_TIME(s) ({ s = (adc_data[1]/1024.0 *100) + 5; })



/*const char *page[] = {
  web_page_title,
  web_page_body1,
  web_page_body2,
  web_page_body3,
  web_page_end
};
*/

/*void update_message_buffer(char *message)
{
	eeprom_read_blo


}*/
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
#define X 1000
#define PRE 64.0
  OCR0 = (X / 1000000.0) * (F_CPU / PRE);
  //OCR0 = 10;
  TCNT0 = 0;

  TIMSK = _BV (OCIE0);		/*Enable Compare match interrupt */

}

void
start_timer0 (void)
{
  TCCR0 |= _BV (CS00) | _BV (CS01);	// PRE 64
  //TCCR0 |= _BV (CS01);         //8

}

void
stop_timer0 (void)
{
  TCCR0 &= ~(_BV (CS00) | _BV (CS01));	//stop PRE 64
  //TCCR0 &= ~_BV (CS01);
}

volatile uint8_t sel_channel = 0;
ISR (TIMER0_COMP_vect)		//Interrupt will be called every Xus
{
#define Y 500			//event every Y ms
  if (timer_int1++ == 500)
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

  if (count % 2 == 1)
    refresh_matrix_row = 1;

  if (count == scroll_time*5)
    {
      //indicate that its time to scroll
      scroll = 0;
      //restart scroll interval 
      count = 0;
      col_count--;
      if (col_count == 1)
	{
	  //When I have completed scroll of one character I should start next character
	  
	  if(i != len)
	  i++;
	  else
	  {
	    if(blank_feed)
	      blank_feed--;
	    else
	    {
	      i=0;
	      blank_feed = MAX_BLANK_CHAR_FEED_COUNT;
	    }
	  }
	  

	  col_count = 7;
	}
    }				//end if count == 100
  else
    count++;

#if 0
  if (scroll < 8)
    {
      display_rows[scanned_row] =
	(display_rows[scanned_row] << 1) |
	(pgm_read_byte (&(font_5x7_col[message[i]][scanned_row])) >>
	 col_count);
      scroll++;
    }

  //scan_row (display_rows[scanned_row], &latch);       //every timer overflow scan a row                   
  if (count == 100)
    {
      //every 1 sec mark a flag to indicate updation of display_row data     
      count = 0;
      scroll = 0;
      col_count--;
      if (col_count == 1)
	{
	  i = (i + 1) % len;
	  col_count = 7;
	}
      // TOGGLEBIT(PORTC,PC6);        
      // TOGGLEBIT(PORTC,PC7);        
    }
  else
    {
      count += 1;
    }
#endif

}

void
timer1_init ()
{

  DDRD |= _BV (PD4);
  TCCR1A = _BV (COM1B1) | _BV (WGM10) | _BV (WGM11);
  TCCR1B = _BV (WGM13) | _BV (WGM12);
  TCNT1 = 0;
  OCR1A = ceil ((F_CPU / 1024.0) / 16.35);
  OCR1B = 0.4 * OCR1A;
//ceil((F_CPU/8.0)/262.0);

}

void
start_timer1 (void)
{

  TCCR1B |= _BV (CS12) | _BV (CS10);	//1024 PRE

}

void
pause_timer1 (void)
{
  TCCR1B &= ~0x02;
}

void
tone (double n)
{
  uint16_t i = ceil ((F_CPU / 1024.0) / n);
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
  unsigned char c;
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
  uint16_t data_len = 0;
  char tmp[40];
  char *handle = NULL, *char_p;
  uint16_t data_0 = adc_data[0];
  uint16_t data_1 = adc_data[1];
  uint16_t data_2;
  float temp = ((data_0 / 1024.0) * 2.56) * 100;
  float pot = ((data_1 / 1024.0) * 2.56);
  
  snprintf (tmp, sizeof (tmp), "%.2f Celcius</p>", temp);
  data_0 = strlen (tmp);
  snprintf (tmp, sizeof (tmp), "%.2f Volts</p>", pot);
  data_1 = strlen (tmp);
  snprintf (tmp, sizeof (tmp), "%s </p>", ip);
  data_2 = strlen (tmp);

  uart_gets (cmd, MAXSTRLEN);	//+IPD,0,278:GET / HTTP/1.0
  if (!strncmp (cmd, "+IPD", 4))
    {
      strtok (cmd, ",");	//skip +IPD
      handle = strtok (NULL, ",");	//get the client handle
      strtok (NULL, ":");	//skip data len
      char_p = strtok (NULL, " ");	//get the command name
      if (!strncmp (char_p, "GET", 3))	//Match the command 
	{
	  char_p = strtok (NULL, " ");	//get the command args 
	  if (!strcmp (char_p, "/"))	//Serving index.html
	    {
	      //Here you serve index.html
	      //flash_led(4); 
	      data_len = (strlen_P (web_page_title) +
			  strlen_P (web_page_body1) +
			  strlen_P (web_page_body2) +
			  strlen_P (web_page_body3) +
			  strlen_P (web_page_body4) +
			  strlen_P (web_page_body5) +
			  data_0 + data_1 + data_2 + strlen_P (web_page_end));
	      // strtok (cmd, ",");
	      // handle = strtok (NULL, ",");
	      snprintf (tmp, sizeof (tmp), "AT+CIPSEND=%s,%d\r\n", handle,
			data_len);
	      uart_puts (tmp);
	      /*wait for prompt */
	      while (uart_getc () != '>');
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
	      uart_puts_pgm (web_page_body5);
	      uart_puts_pgm (web_page_end);

	      _delay_ms (500);
	      snprintf (tmp, sizeof (tmp), "AT+CIPCLOSE=%s\r\n", handle);
	      uart_puts (tmp);
	      _delay_ms (500);
	      //flash_led (3);
	    }			//end serving index.html
	  else if (!strncmp (char_p, "/ap", 3))

	    {
	      //Here you serve according to requested page
	      //uptill here char_p contain starting of args but cant use strtok as it has reached HTTP
	      //+IPD,0,278:GET /ap?id=abhi&passwd=1234 HTTP/1.0

	      strtok (char_p, "?");
	      char_p = strtok (NULL, "=");
	      if (strcmp (char_p, "id"))
		{
		  goto ERROR_EXIT;
		}
	      char_p = strtok (NULL, "&");
	      eeprom_write_block ((const void *) char_p, (void *) ssid,
				  strlen (char_p) + 1);

	      char_p = strtok (NULL, "=");
	      if (strcmp (char_p, "passwd"))
		{
		  goto ERROR_EXIT;
		}
	      char_p = strtok (NULL, "\0");
	      //store id to eeprom
	      eeprom_write_block ((const void *) char_p, (void *) passwd,
				  strlen (char_p) + 1);


	      join_AP_from_EEPROM ();
	      _delay_ms (2000);
	      get_ip_address ();

	      data_len = strlen_P (ssid_update);
	      snprintf (tmp, sizeof (tmp), "AT+CIPSEND=%s,%d\r\n", handle,
			data_len);
	      _delay_ms (100);
	      uart_puts_pgm (ssid_update);
	      _delay_ms (500);
	      snprintf (tmp, sizeof (tmp), "AT+CIPCLOSE=%s\r\n", handle);
	      _delay_ms (500);
	    }			//End serving ssid and passwd request

	  else if (!strncmp (char_p, "/str", 4))
	    {
	      strtok (char_p, "?");
	      char_p = strtok (NULL, "=");
	      if (strcmp (char_p, "message"))
		{
		  goto ERROR_EXIT;
		}
	      char_p = strtok (NULL, " ");
	      data_len = urlDecode(char_p,tmp);
	      
	      if (data_len >= MAX_MESSAGE_LEN)
		{
		  goto ERROR_EXIT;
		}

	      eeprom_update_block ((const char *) tmp,
				   (void *) message_bak, strlen (tmp) + 1);
	      eeprom_read_block ((void *) message,
				 (const void *) message_bak, MAX_MESSAGE_LEN);

	      //start over 
	      len = strlen (tmp);
	      scroll = 8;
	      count = 0;
	      col_count = 7;
	      i = 0;
	      scanned_row = 0;
	      refresh_matrix_row = 0;
	      memset((void *)display_rows,0,MATRIX_ROW_COUNT*4);
 
	      serve_static_html ((const char *) string_update,
				 (const char *) handle, tmp, sizeof (tmp));
	    }			//End String update	
	    else
	    {
		  goto ERROR_EXIT;

	    }

	}			//End of matching GET 
      else
	{
	  goto ERROR_EXIT;
	}
    }

  return;
ERROR_EXIT:

  data_len = strlen_P (error_page_1);
  snprintf (tmp, sizeof (tmp), "AT+CIPSEND=%s,%d\r\n", handle, data_len);
  uart_puts(tmp);
  _delay_ms (100);
  uart_puts_pgm (error_page_1);
  _delay_ms (500);
  snprintf (tmp, sizeof (tmp), "AT+CIPCLOSE=%s\r\n", handle);
  uart_puts(tmp);
  _delay_ms (500);

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

void
join_AP_from_EEPROM (void)
{
  char buff[MAX_SSID_LEN];
  uart_flush ();
  eeprom_read_block ((void *) buff, (const void *) ssid, MAX_SSID_LEN);
  uart_puts ("AT+CWJAP=\"");
  uart_puts (buff);
  uart_puts ("\",\"");
  eeprom_read_block ((void *) buff, (const void *) passwd, MAX_SSID_LEN);
  uart_puts (buff);
  uart_puts ("\"\r\n");

  while (!uart_available ());
  while (uart_available ())
    {
      while (!uart_available ());
      _delay_ms (100);		//let uart data flow in to buffer
      uart_gets (buff, MAX_SSID_LEN);
      if (!strcmp ("OK", buff))
	{
	  flash_led (5);
	  break;
	}
      flash_led (3);
      _delay_ms (2000);
    }
}

int
main ()
{
  timer1_init ();		//FAST PWM with OCRA1 as top and OCR1B determining the DUTY CYCLE. OC1B pin as output
  // Timer 1 used for BUZZER TONE generation
  timer0_init ();
  //start_timer0();
  DDRB = 0xFF;
  PORTB = 0x00;
  DDRD = _BV (PD7) | _BV (PD6) | _BV (PD5);
  PORTD = 0x00;

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

//  uart_puts ("AT+CWJAP=\"Abhishek\",\"85128512\r\n");
//  _delay_ms (500);
  //uart_puts("AT+CIFSR\r\n");
  //_delay_ms(500);
//      uart_puts("AT+PING=\"192.168.0.1\"\r\n");
//      _delay_ms(500);
  uart_flush ();
  uart_puts ("AT+CWSAP=\"Safui\",\"12561256\",11,2\r\n");
  _delay_ms (500);
  uart_flush ();
  _delay_ms (500);
  uart_puts ("AT+CIPMUX=1\r\n");
  _delay_ms (500);
  uart_puts ("AT+CIPSERVER=1,80\r\n");
  uart_flush ();
  join_AP_from_EEPROM ();
  _delay_ms (2000);
  get_ip_address ();
  /*for(i=0; i<12; i++)
     {
     tone(notes[i]);
     _delay_ms(1000);

     }            
     pause_timer1(); */
  //snprintf (message, sizeof (message), "abhishek");
  //len = 8;
	eeprom_read_block ((void *) message,
				 (const void *) message_bak, MAX_MESSAGE_LEN);
	len = strlen(message);
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
      //status_led_update();
#if 1
      if (refresh_matrix_row)
	{
	  if (scroll < 8)	// interrupt counts interval to start scroll by setting scroll =0 at every 200ms
	    {
	      //display buffer is updated per row before displaying/refreshing it
	      UPDATE_SCROLL_TIME(scroll_time);
	      display_rows[scanned_row] = (display_rows[scanned_row] << 1) |
		(pgm_read_byte (&(font_5x7_col[message[i]][scanned_row])) >>
		 col_count);
	      scroll++;
	    }
	  scan_row (display_rows[scanned_row], &latch);	//every 2ms          
	  refresh_matrix_row = 0;
	}
#endif
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
