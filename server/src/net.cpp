#include "net.h"

Net::Net()
{

}
Net::~Net()
{
    disconnect();
}

#if defined(_WIN32) || defined(_WIN64)
void Net::initWinsock()
{
    if ((opStatus = WSAStartup(MAKEWORD(2, 2), &WSAData)) != 0)
    {
        logger("[NET] ERROR: Ошибка инициализации WSAStartup");
    }
    else
    {
        logger("[NET] WSAStartup: " + std::string(WSAData.szSystemStatus));
    }
}
#endif

void Net::disconnect()
{
    if (!cliObj.empty())
    {
        for (int i = 0; i < cliObj.size(); i++)
        {
            std::vector<NetObj*>::iterator delCli = cliObj.begin() + i;
#if defined(_WIN32) || defined(_WIN64)
            closesocket((*delCli)->sock);
#elif defined(__linux__)
            close((*delCli)->sock);
#endif
            delete *delCli;
            cliObj.erase(delCli);
            i--;
        }
    }
    
#if defined(_WIN32) || defined(_WIN64)
   	closesocket(srvObj.sock);
	WSACleanup();
#elif defined(__linux__)
    close(srvObj.sock);
#endif
}

void Net::createSocket()
{
    srvObj.sock = socket(AF_INET, SOCK_STREAM, 0);
#if defined(_WIN32) || defined(_WIN64)
    if (srvObj.sock == INVALID_SOCKET)
    {
        logger("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Ошибка создания сокета.");
        disconnect();
        exit(1);
    }
#elif defined(__linux__)    
    if (srvObj.sock == -1)
    {
        logger("[NET] ERROR: Ошибка создания сокета.");
        disconnect();
        exit(1);
    }
#endif
    else
    {
        logger("[NET] Инициализация сокета успешна.");
    }
}

void Net::bindSocket()
{
    srvObj.addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvObj.addr.sin_family = AF_INET;
    srvObj.addr.sin_port = htons(stoi(port));
    srvObj.addrSize = sizeof(srvObj.addr);

    srvObj.thread = std::async(std::launch::async, &Net::netManager, this);
#if defined(_WIN32) || defined(_WIN64)
    opStatus = bind(srvObj.sock, (sockaddr*)&srvObj.addr, srvObj.addrSize);
    if (opStatus != 0)
    {
        logger("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Ошибка привязки сокета сервера");
    }
#elif defined(__linux__)
    opStatus = bind(srvObj.sock, (struct sockaddr*)&srvObj.addr, srvObj.addrSize);
    if (opStatus == -1)
    {
        logger("[NET] ERROR: Ошибка привязки сокета сервера");
    }
#endif
    else
    {
        logger("[NET] Привязка сокета выполнена.");
    }
}

void Net::servListen()
{
#if defined(_WIN32) || defined(_WIN64)
    opStatus = listen(srvObj.sock, max_clients);    
    if (opStatus != 0)
    {
        logger("[NET] ERROR: Ошибка при постановке на приём данных # " + std::to_string(WSAGetLastError()));
        disconnect();
    }
#elif defined(__linux__)
    opStatus = listen(srvObj.sock, max_clients);
    if (opStatus == -1)
    {
        logger("[NET] ERROR: Ошибка при постановке на приём данных ");
        disconnect();
    }
#endif
    else
    {
        logger("[NET] Количество подключенных клиентов: " + std::to_string(cliObj.size()));
        logger("[NET] Ожидание подключений...");
    }
}

void Net::acceptConn()
{
    NetObj* client = new NetObj();
#if defined(_WIN32) || defined(_WIN64)
    client->sock = accept(srvObj.sock, (sockaddr*)&client->addr, &client->addrSize);
    if (client->sock == INVALID_SOCKET)
    {
        logger("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Сервер несмог принять данные от клиента.");
        closesocket(client->sock);
    }
    else
    {
        std::lock_guard<std::mutex> lock(m_connThreads);
        cliObj.push_back(client);
        int id = cliObj.size() - 1;
        DWORD dwSizeOfStr = sizeof(cliObj[id]->cli_id);
        WSAAddressToStringA((LPSOCKADDR)&cliObj[id]->addr, cliObj[id]->addrSize, NULL, cliObj[id]->cli_id, &dwSizeOfStr);
        cliObj[id]->thread = std::async(std::launch::async, &Net::threadCycle, this, cliObj[id]);
        logger("[NET] Соединение успешно установлено");
    }
#elif defined(__linux__)
    client->sock = accept(srvObj.sock, (struct sockaddr*)&client->addr, &client->addrSize);
    if (client->sock == -1)
    {
        logger("[NET] ERROR: Сервер несмог принять данные от клиента.");
        close(client->sock);
        disconnect();
    }
    else
    {
        cliObj.push_back(client);
        int id = cliObj.size() - 1;
        cliObj[id]->cli_id = inet_ntoa(cliObj[id]->addr.sin_addr);
        cliObj[id]->thread = std::async(std::launch::async, &Net::threadCycle, this, cliObj[id]);
        logger("[NET] " + (std::to_string(*cliObj[id]->cli_id)) + ":" + (std::to_string(cliObj[id]->sock)) + " Соединение успешно установлено");
    }
#endif
}

bool Net::sendReq(NetObj* cli)
{
    strcpy(cli->package, "test_send_request");

#if defined(_WIN32) || defined(_WIN64)    
    opStatus = send(cli->sock, cli->package, sizeof(cli->package), 0);
    if (opStatus == SOCKET_ERROR)
    {
        logger("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Ошибка отправки сообщения клиенту " + cli->cli_id);
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

bool Net::getReq(NetObj* cli)
{
#if defined (_WIN32) || defined (_WIN64)
    ZeroMemory(&cli->package, sizeof(cli->package));
    opStatus = recv(cli->sock, cli->package, sizeof(cli->package), 0);
    if (opStatus == SOCKET_ERROR)
    {
        logger("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Ошибка получения сообщения от клиента " + cli->cli_id);
        closesocket(cli->sock);
        return false;
    }
    else return true;
#elif defined (__linux__)
    bzero(&cli->package, sizeof(cli->package));
    opStatus = read(cli->sock, cli->package, sizeof(cli->package));
    if (opStatus < 0)
    {
        logger("[NET] ERROR: Ошибка получения сообщения от клиента");
        close(cli->sock);
        return false;
    }
    else return true;
#endif
}

void Net::threadCycle(NetObj* cli)
{
    bool online = true;
    while (online)
	{
		online = getReq(cli);
		online = sendReq(cli);
	}
}

void Net::netManager()
{
    while (!isShutdown)
    {
        if (!cliObj.empty())
        {
            std::lock_guard<std::mutex> lock(m_connThreads);
            for (int i = 0; i < cliObj.size(); ++i)
            {
                if (cliObj[i]->thread.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    std::vector<NetObj*>::iterator delCli = cliObj.begin() + i;
#if defined(_WIN32) || defined(_WIN64)
                    logger("[NET] Thread for " + std::string((*delCli)->cli_id) + ":" + std::to_string((*delCli)->sock) + " ended, removing connection...");
#elif defined(__linux__)
                    logger("[NET] Thread for " + std::string((*delCli)->cli_id) + " ended");
#endif
                    delete *delCli;
                    cliObj.erase(delCli);
                    i--;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void Net::logger(const std::string& entry)
{
    std::thread write(&Logger::recLogEntry, std::ref(Log), std::cref(entry));
    write.join();
}

void Net::run()
{
    logger("[NET] Запуск сервера...");
    isShutdown = false;
    statusMainThread = true;
    port = Config->getChatPort();
    max_clients = Config->getMaxClients();
    #if defined(_WIN32) || defined(_WIN64)
    initWinsock();
#endif
    createSocket();
    bindSocket();
    while (!isShutdown)
    {
        servListen();
        acceptConn();	
    }
    disconnect();
    statusMainThread = false;
    isShutdown = true;
}

void Net::stop()
{
    logger("[NET] Выключение сервера...");
    isShutdown = true;
    disconnect();
    logger("[NET] Сервер выключен.");
}