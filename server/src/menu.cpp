#include "menu.h"
Menu::Menu()
{
    menuMain();
}
Menu::~Menu(){}

void Menu::menuMain()
{
    clearConsole();
    servStatus();
    std::cout << "Главное меню\n\n";
    std::cout << "[1] - Настройки\n";
    std::cout << "[2] - Просмотр подключений\n";
    std::cout << "[3] - Запуск\n";
    std::cout << "[4] - Остановка\n";
    std::cout << "[5] - Обновить\n";
    std::cout << "[6] - Посмотреть лог\n";
    std::cout << "[Q] - Выход\n\n";
    std::cin >> choice;
    checkInput('1', '6', 'q', 'Q');
    if (choice == '1') { menuSettings(); return; }
    else if (choice == '2') { menuConnections(); return; }
    else if (choice == '3') { servRun(); }
    else if (choice == '4') { servStop(); }
    else if (choice == '5') { menuMain(); }
    else if (choice == '6') { menuLogViewer(); }
    else if (choice == 'Q') { exit(0); }
}

void Menu::menuSettings()
{
    clearConsole();
    std::cout << "Текущие настройки:\n\n";
    std::cout << "[0] Порт: " << Config->getChatPort() << std::endl;
    std::cout << "[1] Максимальное количество клиентов: " << Config->getMaxClients() << std::endl;
    std::cout << "[2] MySQL Server ip: " << Config->getMysqlSrvIP() << std::endl;
    std::cout << "[3] MySQL User: " << Config->getMysqlLogin() << std::endl;
    std::cout << "[4] MySQL Pass: " << Config->getMysqlPass() << std::endl;
    std::cout << "'M' - вернуться в главное меню. X - измененить параметр X: \n";
    std::cin >> choice;
    checkInput('0', '4', 'm', 'M');
    if (choice == 'm' || choice == 'M') { menuMain(); return; }
    else { Config->changeParam(choice); pause(); menuSettings(); }
}

void Menu::menuConnections()
{
    clearConsole();
    servStatus();
    if (srvStruct.Server.cliObj.empty())
    {
        std::cout << "Нет подключенных клиентов.\n";
    }
    else
    {
        std::cout << "Подключенные клиенты:\n";
        int c = 1;
        for (auto i: srvStruct.Server.cliObj)
        {
            std::cout << c++ << " " << i->cli_id << std::endl;
        }
        
    }
    pause();
    menuMain();
}

bool Menu::servStatus()
{
    std::string status;
    if (srvStruct.Server.statusMainThread)
    {
        status = "Запущен";
    }
    else status = "Выключен";   
    std::cout << "Статус сервера: \"" << status << "\"" << std::endl;
    return srvStruct.Server.statusMainThread;
}

void Menu::servRun()
{
    if (srvStruct.Server.statusMainThread)
    {
        std::cout << "Сервер уже запущен!" << std::endl;
        pause();
        menuMain();
    }
    else
    {
        srvStruct.ServThread = std::async(std::launch::async, &Net::run, &srvStruct.Server);
        menuMain();
    }
}

void Menu::servStop()
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

void Menu::menuLogViewer()
{
    clearConsole();
    Log->readAllEntry();
    pause();
    menuMain();
}

void Menu::clearConsole()
{
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#elif defined(__linux__)
    system("clear");
#endif    
}

void Menu::pause()
{
#if defined(_WIN32) || defined(_WIN64)
    system("pause");
#elif defined(__linux__)
    system("read -p \"Press any key to continue ...\" n");
#endif
}

void Menu::exitApp()
{

}