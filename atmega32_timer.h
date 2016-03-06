
typedef enum {
        NORMAL,
        PWM_PC,
        CTC,
        FAST_PWM
}TIMER_0_MODE;
typedef enum {
        NO_CLK,
        NO_PRE,
        PRE8,
        PRE64,
        PRE256,
        PRE1024=5,
        T0_FE,
        T0_RE
}CLK_SRC;
typedef struct {

        union {
	  TIMER_0_MODE  mode0;
	  TIMER_1_MODE  mode1;
	}mode;
	  
        CLK_SRC src;
        uint8_t start_val;


} TIMER_CB;

typedef struct {
	
	NORMAL,
	PWM_PC_8BIT,
	PWM_PC_9BIT,
	PWM_PC_10BIT,
	CTC_OCR1A,
	F_PWM_8BIT,
	F_PWM_9BIT,
	F_PWM_10BIT,
	PWM_PFC_ICR1,
	PWM_PFC_OCR1A,
	PWM_PC_ICR1,
	PWM_PC_OCR1A,
	CTC_ICR1,
	F_PWM_ICR1=0x0E,
	F_PWM_OCR1A

}TIMER_1_MODE; 
