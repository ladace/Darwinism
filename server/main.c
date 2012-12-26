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

    httpdAddCContent(server, "/", "test",     HTTP_FALSE, NULL, test    );

    httpdSetFileBase(server, "./www");
    httpdAddFileContent(server, "/", "index.html", HTTP_TRUE, NULL, "index.html");

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
