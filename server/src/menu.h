#pragma once
#include "net.h"
class menu
{
private:
    struct server
    {
        net Server;
        std::future<void> ServThread;
    };
    server srvStruct;
    void menuMain();
    void menuSettings();
    void menuConnections();
    void menuLogViewer();
    void servStatus();
    void servRun();
    void servStop();
    void exitApp();
    void pause();
    void clearConsole();
    int choice;
public:
    menu();
    ~menu();
};
