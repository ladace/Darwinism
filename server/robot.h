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
void walk_stop(httpd* server); // also is walktuning functions
void action(httpd* server);

/* Walktuning functions */
void walk_get_param(httpd* server);
void walk_set_param(httpd* server);
void walk_load_parset(httpd* server);
void walk_save_parset(httpd* server);
void walk_save_new_parset(httpd* server);
void walk_del_parset(httpd* server);
void walk_start(httpd* server);
void walk_get_cur_parset(httpd* server);
void walk_get_cur_parset_norm(httpd* server);
void walk_get_parset_num(httpd* server);
void walk_get_par_minmax(httpd* server);

/* helper function */
void httpd_send_data(httpd* server, const void* data, size_t size);

#ifdef __cplusplus
}
#endif
