#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "httpd.h"
#include "robot.h"
#include "video.h"

#define MAX_PATH_LEN  256

void httpd_send_data(httpd* server, const void* data, size_t size) {
    _httpd_net_write(server->clientSock, data, size);
}

void request_file(httpd* server) {
    const char* path = httpdRequestPath(server);
    const char* suffix = rindex(path, '.');

    if (suffix != NULL)
    {
        if (strcasecmp(suffix,".gif") == 0) 
            httpdSetContentType(server, "image/gif");
        if (strcasecmp(suffix,".jpg") == 0) 
            httpdSetContentType(server, "image/jpeg");
        if (strcasecmp(suffix,".xbm") == 0) 
            httpdSetContentType(server, "image/xbm");
        if (strcasecmp(suffix,".png") == 0) 
            httpdSetContentType(server, "image/png");
        if (strcasecmp(suffix,".css") == 0) 
            httpdSetContentType(server, "text/css");
        if (strcasecmp(suffix,".js") == 0) 
            httpdSetContentType(server, "text/javascript");
    }
    httpdSendHeaders(server);

    printf("path: %s\n", path);
    const char* file = index(path, '/');

    if (file && strcmp(file, "/") == 0) file = "/index.html";

    char lpath[MAX_PATH_LEN];
    strcpy(lpath, "www");
    strcat(lpath, file);

    FILE *f = fopen(lpath, "rb");
    if (!f) {
        httpdSetResponse(server, "404 Not found");
        return;
    }

    int n;
    static char buf[10240];
    do {
        n = fread(buf, 1, 10000, f);
        buf[n] = 0;
        _httpd_net_write(server->clientSock, buf, n);
    } while (n == 10000);
    fclose(f);
}

int main() {
    pthread_mutex_init(&rbt_mutex, NULL);
    if (pthread_create(&rbt_tid, NULL, robot_func, NULL) != 0) {
        fprintf(stderr, "Create sub thread error!\n");
        return -1;
    }
    httpd* server = httpdCreate(NULL, 80);
    httpdSetAccessLog(server, stdout);
    httpdSetErrorLog(server, stderr);

    httpdAddCContent(server, "/behav", "rest",       HTTP_FALSE, NULL, rest       );
    httpdAddCContent(server, "/behav", "walk",       HTTP_FALSE, NULL, walk       );
    httpdAddCContent(server, "/behav", "stop-walk",  HTTP_FALSE, NULL, walk_stop  );
    httpdAddCContent(server, "/behav", "action",     HTTP_FALSE, NULL, action     );
    httpdAddCContent(server, "/behav", "is-running", HTTP_FALSE, NULL, is_running );
    httpdAddCContent(server, "/behav", "speed-up",   HTTP_FALSE, NULL, speed_up   );

    httpdAddCContent(server, "/walk",  "start"    ,   HTTP_FALSE, NULL, walk_start);
    httpdAddCContent(server, "/walk",  "stop"     ,   HTTP_FALSE, NULL, walk_stop);
    httpdAddCContent(server, "/walk",  "get-param",   HTTP_FALSE, NULL, walk_get_param);
    httpdAddCContent(server, "/walk",  "set-param",   HTTP_FALSE, NULL, walk_set_param);
    httpdAddCContent(server, "/walk",  "load-parset",           HTTP_FALSE, NULL, walk_load_parset);
    httpdAddCContent(server, "/walk",  "save-parset",           HTTP_FALSE, NULL, walk_save_parset);
    httpdAddCContent(server, "/walk",  "save-new-parset",       HTTP_FALSE, NULL, walk_save_new_parset);
    httpdAddCContent(server, "/walk",  "del-parset",            HTTP_FALSE, NULL, walk_del_parset);
    httpdAddCContent(server, "/walk",  "get-cur-parset",        HTTP_FALSE, NULL, walk_get_cur_parset);
    httpdAddCContent(server, "/walk",  "get-cur-parset-norm",   HTTP_FALSE, NULL, walk_get_cur_parset_norm);
    httpdAddCContent(server, "/walk",  "get-parset-n",          HTTP_FALSE, NULL, walk_get_parset_num);
    httpdAddCContent(server, "/walk",  "get-par-minmax",        HTTP_FALSE, NULL, walk_get_par_minmax);

    httpdAddCContent(server, "/head",  "up",          HTTP_FALSE, NULL, head_up);
    httpdAddCContent(server, "/head",  "down",        HTTP_FALSE, NULL, head_down);
    httpdAddCContent(server, "/head",  "left",        HTTP_FALSE, NULL, head_left);
    httpdAddCContent(server, "/head",  "right",       HTTP_FALSE, NULL, head_right);

    httpdAddCContent(server, "/video", "snapshot",        HTTP_FALSE, NULL, send_snapshot);

    httpdSetFileBase(server, "./www");

    httpdAddCWildcardContent(server, "/", NULL, request_file);
    httpdAddCWildcardContent(server, "/js", NULL, request_file);
    httpdAddCWildcardContent(server, "/css", NULL, request_file);
    httpdAddCWildcardContent(server, "/css/custom-theme", NULL, request_file);
    httpdAddCWildcardContent(server, "/css/custom-theme/images", NULL, request_file);

    httpdAddCContent(server, "/", "index.html", HTTP_TRUE, NULL, request_file);

    printf("Server Start.\n");

    while(1) {
        if (httpdGetConnection(server, NULL) == 1) {
            if (httpdReadRequest(server) < 0) {
                httpdEndRequest(server);
                continue;
            }

            httpdProcessRequest(server);
            httpdEndRequest(server);
        }
    }

    printf("Server Exit.\n");

    exit(0);
}
