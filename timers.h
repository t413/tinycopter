//
//  pwm.h
//  teensy_copter
//
//  Created by Tim O'Brien on 4/13/11.
//  Copyright 2011 t413.com. All rights reserved.
//

#ifndef PWM_H_
#define PWM_H_
//allow for easy C++ compile
#ifdef __cplusplus
extern "C" {
#endif

void timer0_init(void);


unsigned long tics(void);

unsigned long millis(void);


#ifdef __cplusplus
}
#endif
#endif