//
//  pwm.c
//  teensy_copter
//
//  Created by Tim O'Brien on 4/13/11.
//  Copyright 2011 t413.com. All rights reserved.
//


#include <avr/io.h>
#include <avr/interrupt.h>
#include "timers.h"


#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;

void timer0_init(void){
    TCCR0A = 0x00; //!(WGM00) + !(WGM01) + !(WGM02) => normal mode.
    TCCR0B = (1<<CS00); //timer increments with clock (no prescale)
	
    TIMSK0 = (1<<TOIE0); //Timer/Counter0 Overflow interrupt is enabled (overflow in Timer/Counter0 == inturrupt)
    TIFR0 = (1<<TOV0);
    sei();
    
    DDRB |= (1<<6); //port B6 as output.
}

ISR(TIMER0_OVF_vect)
{
	timer0_overflow_count++;
    if (PORTB & (1<<6)) { PORTB &= ~(1<<6); } //off
    else { PORTB |= (1<<6); } //on
}

unsigned long tics(void) {
	unsigned long m;
    uint8_t counter;
    
	cli();
	m = timer0_overflow_count;
    counter = TCNT0;
	sei();
	return (m<<8)+counter;
}

unsigned long millis(void) {
	return tics() >> 11;
}






//  _ms values record the overflowed counter which in arduino counts milliseconds
//  _t0 values record the counter which overflows every millisecond.
volatile unsigned long pulse_start_ms;  // saves start of pulse
volatile unsigned char pulse_start_t0;

#define NUM_CHANNELS_TO_RECIEVE 6
volatile unsigned long timings[NUM_CHANNELS_TO_RECIEVE];
volatile signed char position = -1;
volatile uint8_t resets = 0;


/* 
 * PPM_Init  -  setup inturrupt for to read incoming ppm signal
 */
void ppm_timing_read_init(void) {
	//enable pin change inturrupt on Digital Pin 8
	PCMSK0 |= (1 << PCINT4); //enable port 8's interrupt spesifically
	PCICR |= (1 << PCIE0);  //Any change on any enabled PCINT7..0 pin will cause an interrupt now.
    
}


#define REAL_TIMER_FREQ (F_CPU >> ((CLKPR & 0x0F) + ((TCCR0B & 0x07)-1)*3))
#define tic_timing_to_us(x) REAL_TIMER_FREQ //TODO: finish me.
/* 
 * interrupt function runs when interrupt port 0 (which has P8) is changed
 * enabled by PCICR with individual ports enabled with PCMSK0
 */
ISR(PCINT0_vect) {
    if ( !(PINB & (1<<4))) {  // Pulse trailing edge for port B pin 4, B4. CHANGE ME if you change ports.
        unsigned long m = timer0_overflow_count;
        unsigned char t0 = TCNT0;
        
        if ((TIFR0 & _BV(TOV0)) && (t0 < 255)) { m++; } //fixes strange error, thanks arduino core's micros()!

        if ((m - pulse_start_ms) > (30)) {  //found sync pulse! TODO: change '30' to be dependent on REAL_TIMER_FREQ
            position = 0; 
            resets += 1;
        }
        else if (position < 0) {  //not found a sync pulse yet
            return; 
        }
        else {  //otherwise, increment position and record data!
            if (position >= NUM_CHANNELS_TO_RECIEVE) { return; }
            timings[position++] = ((m<<8)|t0) - ((pulse_start_ms<<8) | pulse_start_t0);
        }
        pulse_start_ms = m;
        pulse_start_t0 = t0;
	}
}


int8_t get_ppm_timings(unsigned long * timing_array) {
	cli();
    if (position < 0) { sei(); return -1; }
	for (int i=0; i<8; i++) {
        timing_array[i] = timings[i];
    }
    position = -1;
    uint8_t r = resets;
	sei();
    //scale by F_CPU/(CLKPR & 0x0F) == cpu freq
    //for (int i=0; i<8; i++) {
    //    timing_array[i] = ((timing_array[i] & (~0x0F)) << 2) + ((timing_array[i] & 0x0F) << 2)
    //}
    return r;
}



