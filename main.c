/* LED Blink Example with USB Debug Channel for Teensy USB Development Board
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2008, 2010 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "sprintf.h"
#include "uart.h"
#include "timers.h"
//#include "usb_debug_only.h"
//#include "print.h"
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
void putc ( void* p, char c) { uart_putchar(c); }

int main(void)
{
	//timer0_init();
	timer1_init();
	uart_init(115200);
	init_printf((void*)0,putc);
	CPU_PRESCALE(0); // set for 16 MHz clock
	DDRB = 0xFF;
	PORTB = 0x00;
	DDRD = 0xFF;

    //usb_init();
	uint16_t last_print = 0;
	uint16_t last_led = 0;
	
	while (1) {
	    _delay_ms(100);
        uint16_t c = tics() >> 14;
	}
}
