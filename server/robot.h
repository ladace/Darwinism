#pragma once
#include <stdio.h>
#include "httpd.h"

#ifdef __cplusplus
extern "C" {
#endif

extern pthread_t rbt_tid;
extern pthread_mutex_t rbt_mutex;

void* robot_func(void*);

/* Behaviour functions */
void start(httpd* server);
void rest(httpd* server);
void walk(httpd* server);
void walkstop(httpd* server);
void action(httpd* server);

/* Walktuning functions */
void set_walk_param(httpd* server);
void get_walk_param(httpd* server);

#ifdef __cplusplus
}
#endif
