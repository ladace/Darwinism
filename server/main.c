#include <sys/time.h>
#include <stdio.h>
#include "httpd.h"
int main() {
    httpd* server = httpdCreate("127.0.0.1", 8090);
    httpdSetAccessLog(server, stdout);
    httpdSetErrorLog(server, stderr);
    httpdSetFileBase(server, "./www");
    httpdAddFileContent(server, "/", "index.html", HTTP_TRUE, NULL, "index.html");
    httpdAddStaticContent(server, "/", "index1.html", HTTP_TRUE, NULL, "<HTML><BODY>Hello World</BODY></HTML>\r\n");//Bug?
    while(httpdGetConnection(server, NULL) == 1) {
        if (httpdReadRequest(server) < 0) {
            httpdEndRequest(server);
            continue;
        }
        printf("%s\n", httpdRequestPath(server));
        httpdProcessRequest(server);
        httpdEndRequest(server);
    }
    return 0;
}
