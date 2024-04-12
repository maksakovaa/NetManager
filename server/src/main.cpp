#include "menu.h"
using namespace std;

Logger Log;
Settings* Config = new Settings;

int main()
{
#if defined (_WIN32) || defined (_WIN64)
    system("chcp 65001");
#endif
    menu netServer;
    return 0;
}