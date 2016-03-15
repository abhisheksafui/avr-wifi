#include<util/delay.h>
#include<avr/pgmspace.h>
#include<avr-wifi.h>
#include<stdio.h>



void serve_static_html(const char *page, const char *handle,char *tmp, uint16_t len)
{
  uint16_t data_len = strlen_P (page);
  snprintf (tmp, len, "AT+CIPSEND=%s,%d\r\n", handle, data_len);
  uart_puts(tmp);
  _delay_ms (100);
  uart_puts_pgm (page);
  _delay_ms (500);
  snprintf (tmp, len, "AT+CIPCLOSE=%s\r\n", handle);
  uart_puts(tmp);
  _delay_ms (500);
	
}
