#include "menu.h"
menu::menu()
{
    menuMain();
}
menu::~menu(){}

void menu::menuMain()
{
    clearConsole();
    servStatus();
    std::cout << "Главное меню\n\n";
    std::cout << "1 - Настройки\n";
    std::cout << "2 - Просмотр подключений\n";
    std::cout << "3 - Запуск\n";
    std::cout << "4 - Остановка\n";
    std::cout << "5 - Обновить\n";
    std::cout << "6 - Посмотреть лог\n";
    std::cout << "7 - Выход\n\n";
    std::cin >> choice;
    switch (choice)
    {
    case 1:
        menuSettings();
        break;
    case 2:
        menuConnections();
        break;
    case 3:
        servRun();
        break;
    case 4:
        servStop();
        break;
    case 5:
        menuMain();
        break;
    case 6:
        menuLogViewer();
        break;
    case 7:
        exit(0);
        break;    
    default:
        break;
    }
}

void menu::menuSettings()
{
    clearConsole();
    std::cout << "Текущие настройки:\n\n";
    std::cout << "Порт: " << srvStruct.Server.port << std::endl;
    std::cout << "Максимальное количество клиентов: " << srvStruct.Server.max_clients << std::endl;
    pause();
    menuMain();
}

void menu::menuConnections()
{
    clearConsole();
    if (srvStruct.Server.cliObj.empty())
    {
        std::cout << "Нет подключенных клиентов.\n";
    }
    else
    {
        std::cout << "Подключенные клиенты:\n";
        int c = 0;
        for (auto i: srvStruct.Server.cliObj)
        {
            std::cout << c++ << " " << i->cli_id << std::endl;
        }
        
    }
    pause();
    menuMain();
}

void menu::servStatus()
{
    std::string status;
    if (srvStruct.Server.statusMainThread)
    {
        status = "Запущен";
    }
    else status = "Выключен";   
    std::cout << "Статус сервера: \"" << status << "\"" << std::endl;
}

void menu::servRun()
{
    if (srvStruct.Server.statusMainThread)
    {
        std::cout << "Сервер уже запущен!" << std::endl;
        pause();
        menuMain();
    }
    else
    {
        srvStruct.ServThread = std::async(std::launch::async, &net::run, &srvStruct.Server);
        menuMain();
    }
}

void menu::servStop()
{
    if (!srvStruct.Server.statusMainThread)
    {
        std::cout << "Сервер уже выключен!" << std::endl;
        pause();
        menuMain();
    }
    else
    {
        srvStruct.Server.stop();
        menuMain();
    }
}

void menu::menuLogViewer()
{
    clearConsole();
    Log.readAllEntry();
    pause();
    menuMain();
}

void menu::clearConsole()
{
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#elif defined(__linux__)
    system("clear");
#endif    
}

void menu::pause()
{
#if defined(_WIN32) || defined(_WIN64)
    system("pause");
#elif defined(__linux__)
    system("read -p \"Press any key to continue ...\" n");
#endif
}

void menu::exitApp()
{

}