#include "net.h"

net::net()
{
    convert_ip();
    initWinsock();
    createSocket();
    bindSocket();
}

net::~net()
{
    disconnect();
}

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

void net::disconnect()
{
   	closesocket(srvObj.sock);
	WSACleanup();
}

void net::createSocket()
{
    srvObj.sock = socket(AF_INET, SOCK_STREAM, 0);

    if (srvObj.sock == INVALID_SOCKET)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка создания сокета." << std::endl;
        disconnect();
        exit(1);
    }
    else
    {
        std::cout << "[NET] Инициализация сокета успешна." << std::endl;
    }

}

void net::bindSocket()
{
    srvObj.addr.sin_family = AF_INET;
    srvObj.addr.sin_addr = ip_to_num;
    srvObj.addr.sin_port = htons(stoi(port));
    srvObj.addrSize = sizeof(srvObj.addr);

    opStatus = bind(srvObj.sock, (sockaddr*)&srvObj.addr, srvObj.addrSize);
    if (opStatus != 0)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка привязки сокета сервера" << std::endl;
        disconnect();
        exit(1);
    }
    else
    {
        std::cout << "[NET] Привязка сокета выполнена." << std::endl;
    }
    
}

void net::servListen()
{
    opStatus = listen(srvObj.sock, SOMAXCONN);

    if (opStatus != 0)
    {
        std::cout << "[NET] ERROR: Ошибка при постановке на приём данных # " << WSAGetLastError() << std::endl;
        disconnect();
        exit(1);
    }
    else
    {
        std::cout << "Количество подключенных клиентов: " << connThreads.size() << std::endl;
        std::cout << "Количество данных о клиентах: " << cliObj.size() << std::endl;
        std::cout << "[NET] Ожидание подключений..." << std::endl;
    }
    
}

void net::acceptConn()
{
    netObj* client = new netObj();
    client->sock = accept(srvObj.sock, (sockaddr*)&client->addr, &client->addrSize);
    if (client->sock == INVALID_SOCKET)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Сервер несмог принять данные от клиента." << std::endl;
        closesocket(client->sock);
        disconnect();
        exit(1);
    }
    else
    {
        cliObj.push_back(client);
        int id = cliObj.size() - 1;
        DWORD dwSizeOfStr = sizeof(cliObj[id]->cli_id);
        WSAAddressToStringA((LPSOCKADDR)&cliObj[id]->addr, cliObj[id]->addrSize, NULL, cliObj[id]->cli_id, &dwSizeOfStr);
        connThreads.push_back(std::future<bool>(std::async(std::launch::async, &net::threadCycle, this, cliObj[id])));
        std::cout << "[NET] Соединение успешно установлено" << std::endl;
    }
}

bool net::sendReq(netObj* cli)
{
    strcpy(cli->package, "test_send_request");
    opStatus = send(cli->sock, cli->package, sizeof(cli->package), 0);
    if (opStatus == SOCKET_ERROR)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка отправки сообщения клиенту " << cli->cli_id << std::endl;
        closesocket(cli->sock);
        return false;
    }
    else
    {
        return true;
    }
    
}

bool net::getReq(netObj* cli)
{
    ZeroMemory(&cli->package, sizeof(cli->package));
    opStatus = recv(cli->sock, cli->package, sizeof(cli->package), 0);
    if (opStatus == SOCKET_ERROR)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка получения сообщения от клиента " << cli->cli_id << std::endl;
        closesocket(cli->sock);
        return false;
    }
    else
    {
        return true;
    }
}

bool net::threadCycle(netObj* cli)
{
    bool online = true;
    while (online)
	{
		online = getReq(cli);
		online = sendReq(cli);
	}
    return false;
}

void net::connChecker()
{
    if (!connThreads.empty())
    {
        for (int i = 0; i < connThreads.size(); ++i)
        {
            if (connThreads[i].wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                std::vector<netObj*>::iterator delCli = cliObj.begin() + i;
                std::cout << "Thread for " << (*delCli)->cli_id << " ended, removing connection..." << std::endl;
                delete *delCli;
                cliObj.erase(delCli);
                connThreads.erase(connThreads.begin() + i);
                --i;
            }
        }
    }
}

void net::runServer()
{
    while (true)
    {
        connChecker();
		servListen();
		acceptConn();
    }
}