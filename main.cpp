#include <unistd.h>
#include "WebServer.h"

int main() {
    int port=1316;
    int trigMode=3;
    int timeoutMS = 60000;
    bool optLinger=false;
    int mysqlPort=3306;
    const char* user="hao";
    const char* pwd="hao";
    const char* dbName="webserver";
    int sqlNum=12;
    int threadNum=6;
    bool isOpenLog=true;
    int logLevel=1;
    int asyncQueueSize=1024;
    WebServer server(
        port, trigMode, timeoutMS, optLinger,            
        mysqlPort, user, pwd, dbName, 
        sqlNum, threadNum, isOpenLog, logLevel, asyncQueueSize);             
    server.start();
} 
  