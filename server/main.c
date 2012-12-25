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

    httpdAddCContent(server, "/behav", "start",    HTTP_FALSE, NULL, start   );
    httpdAddCContent(server, "/behav", "rest",     HTTP_FALSE, NULL, rest    );
    httpdAddCContent(server, "/behav", "walk",     HTTP_FALSE, NULL, walk    );
    httpdAddCContent(server, "/behav", "walkstop", HTTP_FALSE, NULL, walkstop);
    httpdAddCContent(server, "/behav", "action",   HTTP_FALSE, NULL, action  );

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
