#pragma once
#include "httpd.h"

#ifdef __cplusplus
extern "C" {
#endif

void video_initialize();
void send_snapshot(httpd* server);

#ifdef __cplusplus
}
#endif

