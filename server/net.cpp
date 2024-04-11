#include "net.h"

net::net()
{
#if defined(_WIN32) || defined(_WIN64)
    convert_ip();
    initWinsock();
#endif
    createSocket();
    bindSocket();
}

net::~net()
{
    disconnect();
}

#if defined(_WIN32) || defined(_WIN64)
void net::convert_ip()
{
    opStatus = inet_pton(AF_INET, server_ip.data(), &ip_to_num);
    if (opStatus <= 0)
    {
        std::cout << "[NET] ERROR: Ошибка преобразования IP адреса в специальный цифровой формат." << std::endl;
        exit(1);
    }
}

void net::initWinsock()
{
    if ((opStatus = WSAStartup(MAKEWORD(2, 2), &WSAData)) != 0)
    {
        std::cout << "[NET] ERROR: Ошибка инициализации WSAStartup" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "[NET] WSAStartup: " << WSAData.szSystemStatus << std::endl;
    }
}
#endif

void net::disconnect()
{
#if defined(_WIN32) || defined(_WIN64)
   	closesocket(srvObj.sock);
	WSACleanup();
#elif defined(__linux__)
    close(srvObj.sock);
#endif
}

void net::createSocket()
{
    srvObj.sock = socket(AF_INET, SOCK_STREAM, 0);
#if defined(_WIN32) || defined(_WIN64)
    if (srvObj.sock == INVALID_SOCKET)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка создания сокета." << std::endl;
        disconnect();
        exit(1);
    }
#elif defined(__linux__)    
    if (srvObj.sock == -1)
    {
        std::cout << "[NET] ERROR: Ошибка создания сокета." << std::endl;
        disconnect();
        exit(1);
    }
#endif
    else
    {
        std::cout << "[NET] Инициализация сокета успешна." << std::endl;
    }
}

void net::bindSocket()
{
#if defined(_WIN32) || defined(_WIN64)    
    srvObj.addr.sin_addr = ip_to_num;
#elif defined(__linux__)
    srvObj.addr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
    srvObj.addr.sin_family = AF_INET;
    srvObj.addr.sin_port = htons(stoi(port));
    srvObj.addrSize = sizeof(srvObj.addr);
    srvObj.thread = std::async(std::launch::async, &net::netManager, this);
#if defined(_WIN32) || defined(_WIN64)
    opStatus = bind(srvObj.sock, (sockaddr*)&srvObj.addr, srvObj.addrSize);
    if (opStatus != 0)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка привязки сокета сервера" << std::endl;
        disconnect();
        exit(1);
    }
#elif defined(__linux__)
    opStatus = bind(srvObj.sock, (struct sockaddr*)&srvObj.addr, srvObj.addrSize);
    if (opStatus == -1)
    {
        std::cout << "[NET] ERROR: Ошибка привязки сокета сервера" << std::endl;
        disconnect();
        exit(1);
    }
#endif
    else
    {
        std::cout << "[NET] Привязка сокета выполнена." << std::endl;
    }
}

void net::servListen()
{
#if defined(_WIN32) || defined(_WIN64)
    opStatus = listen(srvObj.sock, SOMAXCONN);    
    if (opStatus != 0)
    {
        std::cout << "[NET] ERROR: Ошибка при постановке на приём данных # " << WSAGetLastError() << std::endl;
        disconnect();
        exit(1);
    }
#elif defined(__linux__)
    opStatus = listen(srvObj.sock, 4096);
    if (opStatus == -1)
    {
        std::cout << "[NET] ERROR: Ошибка при постановке на приём данных " << std::endl;
        disconnect();
        exit(1);
    }
#endif
    else
    {
        std::cout << "Количество подключенных клиентов: " << cliObj.size() << std::endl;
        std::cout << "[NET] Ожидание подключений..." << std::endl;
    }
    
}

void net::acceptConn()
{
    netObj* client = new netObj();
#if defined(_WIN32) || defined(_WIN64)
    client->sock = accept(srvObj.sock, (sockaddr*)&client->addr, &client->addrSize);
    if (client->sock == INVALID_SOCKET)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Сервер несмог принять данные от клиента." << std::endl;
        closesocket(client->sock);
        disconnect();
    }
    else
    {
        std::lock_guard<std::mutex> lock(m_connThreads);
        cliObj.push_back(client);
        int id = cliObj.size() - 1;
        DWORD dwSizeOfStr = sizeof(cliObj[id]->cli_id);
        WSAAddressToStringA((LPSOCKADDR)&cliObj[id]->addr, cliObj[id]->addrSize, NULL, cliObj[id]->cli_id, &dwSizeOfStr);
        cliObj[id]->thread = std::async(std::launch::async, &net::threadCycle, this, cliObj[id]);
        std::cout << "[NET] Соединение успешно установлено" << std::endl;
    }
#elif defined(__linux__)
    client->sock = accept(srvObj.sock, (struct sockaddr*)&client->addr, &client->addrSize);
    if (client->sock == -1)
    {
        std::cout << "[NET] ERROR: Сервер несмог принять данные от клиента." << std::endl;
        close(client->sock);
        disconnect();
    }
    else
    {
        cliObj.push_back(client);
        int id = cliObj.size() - 1;
        cliObj[id]->cli_id = inet_ntoa(cliObj[id]->addr.sin_addr);
        cliObj[id]->thread = std::async(std::launch::async, &net::threadCycle, this, cliObj[id]);
        std::cout << "[NET] " << *cliObj[id]->cli_id << ":" << cliObj[id]->sock << " Соединение успешно установлено" << std::endl;
    }
#endif
}

bool net::sendReq(netObj* cli)
{
    strcpy(cli->package, "test_send_request");

#if defined(_WIN32) || defined(_WIN64)    
    opStatus = send(cli->sock, cli->package, sizeof(cli->package), 0);
    if (opStatus == SOCKET_ERROR)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка отправки сообщения клиенту " << cli->cli_id << std::endl;
        closesocket(cli->sock);
        return false;
    }
    else return true;
#elif defined (__linux__)
    ssize_t bytes = write(cli->sock, cli->package, sizeof(cli->package));
    if (bytes >= 0)
    {
        return true;
    }
    else return false;
#endif
    
}

bool net::getReq(netObj* cli)
{
#if defined (_WIN32) || defined (_WIN64)
    ZeroMemory(&cli->package, sizeof(cli->package));
    opStatus = recv(cli->sock, cli->package, sizeof(cli->package), 0);
    if (opStatus == SOCKET_ERROR)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка получения сообщения от клиента " << cli->cli_id << std::endl;
        closesocket(cli->sock);
        return false;
    }
    else return true;
#elif defined (__linux__)
    bzero(&cli->package, sizeof(cli->package));
    opStatus = read(cli->sock, cli->package, sizeof(cli->package));
    if (opStatus < 0)
    {
        std::cout << "[NET] ERROR: Ошибка получения сообщения от клиента" << std::endl;
        close(cli->sock);
        return false;
    }
    else return true;
#endif
}

void net::threadCycle(netObj* cli)
{
    bool online = true;
    while (online)
	{
		online = getReq(cli);
		online = sendReq(cli);
	}
}

void net::connChecker()
{
    if (!cliObj.empty())
    {
        std::lock_guard<std::mutex> lock(m_connThreads);
        for (int i = 0; i < cliObj.size(); ++i)
        {
            if (cliObj[i]->thread.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                std::vector<netObj*>::iterator delCli = cliObj.begin() + i;
                std::cout << "Thread for " << (*delCli)->cli_id << ":" << (*delCli)->sock << " ended, removing connection..." << std::endl;
                delete *delCli;
                cliObj.erase(delCli);
                i--;
            }
        }
    }
}

void net::netManager()
{
    while (true)
    {
        connChecker();
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void net::runServer()
{
    while (true)
    {
		servListen();
		acceptConn();
    }
}