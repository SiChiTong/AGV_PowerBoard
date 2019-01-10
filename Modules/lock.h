#ifndef __MODULE_LOCK_H__
#define __MODULE_LOCK_H__
#include "sys.h"

#define LOCK_STATUS_LOCK        1
#define LOCK_STATUS_UNLOCK      0
void init_lock(void);
void lock_on(void);
void lock_off(void);
uint8_t get_lock_status(void);
#endif
