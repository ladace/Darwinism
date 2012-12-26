#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "httpd.h"
#include "robot.h"

void test(httpd* server) {
    httpVar* v = httpdGetVariableByName(server, "name");
    if (v) {
        printf("Name value: %s\n", v->value);
        httpdOutput(server, "Hello, $name.");
    } else {
        printf("No value provided.\n");
        httpdOutput(server, "Hello, anonymous.");
    }
}

#define MAX_PATH_LEN  256

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
            httpdSetContentType(server, "application/x-javascript");
    }
    printf("path: %s\n", path);
    const char* file = index(path, '/');
    if (file && strcmp(file, "/") != 0) printf("%s\n", file);
    else file = "/index.html";

    char lpath[MAX_PATH_LEN];
    strcpy(lpath, "www");
    strcat(lpath, file);

    FILE *f = fopen(lpath, "r");
    if (!f) {
        httpdSetResponse(server, "404 Not found");
        return;
    }

    int n;
    char buf[10240];
    do {
        n = fread(buf, 1, 10239, f);
        buf[n] = 0;
        //printf(buf);
        httpdPrintf(server, "%s", buf);
    } while (n == 10239);
    fclose(f);
}

int main() {
    pthread_mutex_init(&rbt_mutex, NULL);
    if (pthread_create(&rbt_tid, NULL, robot_func, NULL) != 0) {
        fprintf(stderr, "Create sub thread error!\n");
        return -1;
    }
    httpd* server = httpdCreate(NULL, 8090);
    httpdSetAccessLog(server, stdout);
    httpdSetErrorLog(server, stderr);

    httpdAddCContent(server, "/behav", "rest",      HTTP_FALSE, NULL, rest    );
    httpdAddCContent(server, "/behav", "walk",      HTTP_FALSE, NULL, walk    );
    httpdAddCContent(server, "/behav", "stop-walk", HTTP_FALSE, NULL, walk_stop);
    httpdAddCContent(server, "/behav", "action",    HTTP_FALSE, NULL, action  );

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

    httpdAddCContent(server, "/", "test",     HTTP_FALSE, NULL, test    );

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
