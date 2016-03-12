#ifndef UTIL_MACRO
#define UTIL_MACRO

#define SETBIT(R,B)     R |= (1<<B)
#define CLRBIT(R,B)     R &= ~(1<<B)
#define CHKBIT(R,B)     (R & (1<<B))
#define TOGGLEBIT(R,B)  (R = (R ^ (1<<B)))

void sleep_ms(int);

#endif
