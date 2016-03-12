#include<led_matrix.h>
#include<util/delay.h>

volatile uint8_t scroll = 8;
volatile uint8_t count = 0;
volatile uint16_t  display_rows[MATRIX_ROW_COUNT] = {0};
volatile uint8_t len;
volatile uint8_t col_count = 7;
volatile uint8_t i =  0;
volatile uint8_t scanned_row=0;
volatile char message[MAX_MESSAGE_LEN];
//volatile uint8_t *character;
SHIFT_595_CB latch = {
        .port = &PORTD,
        .port_dir = &DDRD, 
        .pin_d = PD5,
        .pin_c = PD6,
        .pin_l = PB7

};


void scan_row(uint16_t data,SHIFT_595_CB *latch)
{
        ROW_SCAN_PORT = 0;
        shift_data(data,16,latch);
        LATCH_DATA(latch);
        ROW_SCAN_PORT = (1<<(7-scanned_row));
        scanned_row = (scanned_row+1)%8;
}

