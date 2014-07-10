/*
	library for configuring the on-board timers
*/

void T2_init(unsigned long period)
{
	TMR2CN = 0x04;
	TMR2RLL = 0x30;
	TMR2RLH = 0xF8;
	IE |= 0x20;
}
