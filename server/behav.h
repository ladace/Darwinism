#pragma once
#include "httpd.h"

#ifdef __cplusplus
extern "C" {
#endif

extern pthread_t beh_tid;
extern pthread_mutex_t beh_mutex;

void start(httpd* server);
void end(httpd* server);
void walk(httpd* server);
void walkstop(httpd* server);
void action(httpd* server);

void* behaviour_func(void*);

#ifdef __cplusplus
}
#endif
