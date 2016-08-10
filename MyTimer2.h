#ifndef MyTimer2_h
#define MyTimer2_h

#ifdef __AVR__
#include <avr/interrupt.h>
#elif defined(__arm__) && defined(TEENSYDUINO)
#include <Arduino.h>
#else
#error MyTimer2 library only works on AVR architecture
#endif

namespace MsTimer2 {
	extern unsigned long msecs;
	extern void * param;
	extern void (*func)(void*);
	extern volatile unsigned long count;
	extern volatile char overflowing;
	extern volatile unsigned int tcnt2;
	
	void set(unsigned long ms, void (*f)(void*),void *);
	void start();
	void stop();
	void _overflow();
}

#endif
