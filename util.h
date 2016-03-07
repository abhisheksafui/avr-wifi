#ifndef UTIL_H
#define UITL_H


#define NULL	0
#define SETBIT(R,B)     R |= (1<<B)
#define CLRBIT(R,B)     R &= ~(1<<B)
#define CHKBIT(R,B)     (R & (1<<B))
#define TOGGLEBIT(R,B)  (R = (R ^ (1<<B)))


#endif
