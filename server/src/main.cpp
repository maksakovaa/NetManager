#include "menu.h"
using namespace std;

Logger* Log = new Logger;
Settings* Config = new Settings;
ReqAPI* API = new ReqAPI; 

int main()
{
#if defined (_WIN32) || defined (_WIN64)
    system("chcp 65001");
#endif
    Menu netServer;
    return 0;
}