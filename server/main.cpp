#include "net.h"

using namespace std;

int main()
{
#if defined (_WIN32) || defined (_WIN64)
    system("chcp 65001");
#endif
    net server;
    server.runServer();
    return 0;
}