#ifndef RTC_H
#define RTC_H

#include <stdint.h>

void rtc_init(void);
void rtc_read_time(uint8_t* h, uint8_t* m, uint8_t* s);

#endif
