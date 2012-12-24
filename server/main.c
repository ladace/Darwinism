#include <sys/time.h>
#include <stdio.h>
#include "httpd.h"
void callback(httpd* server) {
    httpdOutput(server, "CContent");
}
int main() {
    httpd* server = httpdCreate("127.0.0.1", 8090);
    httpdSetAccessLog(server, stdout);
    httpdSetErrorLog(server, stderr);
    httpdSetFileBase(server, "./www");
    httpdAddCContent(server, "/", "test", HTTP_FALSE, NULL, callback);
    httpdAddFileContent(server, "/", "index.html", HTTP_TRUE, NULL, "index.html");
    while(httpdGetConnection(server, NULL) == 1) {
        if (httpdReadRequest(server) < 0) {
            httpdEndRequest(server);
            continue;
        }

        printf("requested: %s\n", httpdRequestPath(server));
        fflush(stdout);

        httpdProcessRequest(server);
        httpdEndRequest(server);
    }
    return 0;
}
