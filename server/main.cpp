#include "net.h"

using namespace std;

int main()
{
    system("chcp 65001");
    net server;
    server.runServer();
    return 0;
}