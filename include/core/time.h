#ifndef CORE_TIME_H
#define CORE_TIME_H

#include <stdint.h>

void  time_init(void);
void  time_update(void);

float time_get_dt(void);
float time_get_seconds(void);

#endif // CORE_TIME_H
