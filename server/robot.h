#pragma once
#include <stdio.h>
#include "httpd.h"

#ifdef __cplusplus
extern "C" {
#endif

extern pthread_t rbt_tid;
extern pthread_mutex_t rbt_mutex;

/* Behaviour functions */
void start(httpd* server);
void rest(httpd* server);
void walk(httpd* server);
void walkstop(httpd* server);
void action(httpd* server);

void* robot_func(void*);

#ifdef __cplusplus
}
#endif
