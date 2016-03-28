#include<util/delay.h>
#include<avr/pgmspace.h>
#include<avr-wifi.h>
#include<stdio.h>
#include<web_server.h>
#include<string.h>
#include<uart.h>


int urlDecode(char *src, char *tmp)
{
    unsigned int i;
    uint8_t j= 0;
    memset(tmp, 0, sizeof(tmp));

    for (i=0; i < strlen(src); i++)
    {
        if(src[i] == '+')
        {
            tmp[j++] =  ' ';
            continue;
        }
        if (src[i] != '%')
        {
            tmp[j++] = src[i];
            continue;
        }

     /*   if (!isdigit(src[i+1]) || !isdigit(src[i+2]))
        {
            *ptr++ = src[i];
            continue;
        }
      */

        printf("\n ASCII to nibble from %c to %d\n",src[i+1], ASCII_TO_NIBBLE(src[i+1]));
        tmp[j++] = (ASCII_TO_NIBBLE(src[i+1]) << 4) | (ASCII_TO_NIBBLE(src[i+2]));
        i += 2;
    }
    tmp[j] = '\0';
    strcpy(src, tmp);
    return j;
}


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
