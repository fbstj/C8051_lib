/*
	library for configuring the on-board timers
*/

#ifndef __TIMERS_H
#define __TIMERS_H

struct timer {
	// number of counts to make
	unsigned long period;
	// current number of counts
	unsigned long index;
	// flag set after overflow
	char overflowed;
};

void timer_init(struct timer *, unsigned long);

void T0_init(unsigned long);
void T1_init(unsigned long);
void T2_init(unsigned long);
void T3_init(unsigned long);
void T4_init(unsigned long);
void T5_init(unsigned long);

#endif //__TIMERS_H