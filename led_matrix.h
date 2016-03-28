#ifndef LED_MATRIX_H
#define LED_MATRIX_H
#include<avr/io.h>
#include<74595_shift.h>

#define MATRIX_ROW_COUNT 8
#define MAX_MESSAGE_LEN 40
#define ROW_SCAN_PORT	PORTB
extern volatile uint8_t scroll_time;
//extern volatile uint8_t *character;
extern volatile uint8_t scanned_row;
extern volatile uint8_t scroll;
extern volatile uint16_t count;
extern volatile uint32_t  display_rows[MATRIX_ROW_COUNT];
extern volatile uint8_t len;
extern volatile uint8_t col_count;
extern volatile uint8_t scroll;
extern volatile uint8_t i;
extern volatile uint8_t scanned_row;
volatile uint8_t refresh_matrix_row;
extern SHIFT_595_CB latch; 
extern volatile char message[MAX_MESSAGE_LEN];
extern char message_bak[MAX_MESSAGE_LEN];

void scan_row(uint32_t byte,SHIFT_595_CB *latch);

#endif
