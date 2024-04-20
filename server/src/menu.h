#pragma once
#include "net.h"
class Menu
{
private:
    struct server
    {
        Net Server;
        std::future<void> ServThread;
    };
    server srvStruct;
    void menuMain();
    void menuSettings();
    void menuConnections();
    void menuLogViewer();
    bool servStatus();
    void servRun();
    void servStop();
    void exitApp();
    void pause();
    void clearConsole();
    char choice;
    template<typename... Args>
    void checkInput(char from, char to, Args... args);
public:
    Menu();
    ~Menu();
};

template<typename... Args> void Menu::checkInput(char from, char to, Args... args)
{
    if (!(choice >= from && choice <= to) && ([&]{return choice != args;}()&& ...))
    {
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        std::cout << "Некорректный ввод, повторите:\n";
        std::cin >> choice;
    }
}