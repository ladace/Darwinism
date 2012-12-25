#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "httpd.h"
#include "behav.h"

int main() {
    pthread_mutex_init(&beh_mutex, NULL);
    if (pthread_create(&beh_tid, NULL, behaviour_func, NULL) != 0) {
        fprintf(stderr, "Create sub thread error!\n");
        return -1;
    }
    httpd* server = httpdCreate(NULL, 8090);
    httpdSetAccessLog(server, stdout);
    httpdSetErrorLog(server, stderr);

    httpdAddCContent(server, "/", "start",    HTTP_FALSE, NULL, start   );
    httpdAddCContent(server, "/", "end",      HTTP_FALSE, NULL, end     );
    httpdAddCContent(server, "/", "walk",     HTTP_FALSE, NULL, walk    );
    httpdAddCContent(server, "/", "walkstop", HTTP_FALSE, NULL, walkstop);
    httpdAddCContent(server, "/", "action",   HTTP_FALSE, NULL, action  );

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
