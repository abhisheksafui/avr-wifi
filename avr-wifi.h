#ifndef AVR_WIFI_H
#define AVR_WIFI_H

#define MAXSTRLEN       100
#define MAXCMDS         15
#define LED1_PORT PORTC
#define LED1_PIN  PC7
#define LED2_PORT PORTC
#define LED2_PIN  PC6
#define MAX_SSID_LEN 20

extern char ssid[MAX_SSID_LEN];
extern char passwd[MAX_SSID_LEN];
extern char ip[32];
extern volatile uint16_t adc_data[2];
extern volatile uint8_t channel;
extern const float notes[12];

extern const char web_page_title[];
extern const char web_page_body1[];
extern const char web_page_body2[];
extern const char web_page_body3[];
extern const char web_page_body4[];
extern const char web_page_body5[];
extern const char web_page_end[];
extern const char error_page_1[];
#endif
