#include "net.h"

net::net()
{
    convert_ip();
    initWinSock();
    createSocket();
}

net::~net()
{
    disconnect();
}

void net::convert_ip()
{
    opStatus = inet_pton(AF_INET, server_ip.data(), &ip_to_num);
    if (opStatus < 0)
    {
        std::cout << "[NET] ERROR: Ошибка преобразования IP адреса в специальный цифровой формат." << std::endl;
        exit(1);
    }
}

void net::initWinSock()
{
    opStatus = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (opStatus != 0)
    {
        std::cout << "[NET] ERROR: " << opStatus << " WSAStartup failed" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "[NET] WSAStartup: " << wsaData.szSystemStatus << std::endl;
    }
}

void net::createSocket()
{
    sockCli = socket(AF_INET, SOCK_STREAM, 0);
    if (sockCli == INVALID_SOCKET)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка создания сокета." << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "[NET] Сокет создан." << std::endl;
    }
    
}

void net::connServer()
{
    ZeroMemory(&servInfo, sizeof(servInfo));
    servInfo.sin_family = AF_INET;
    servInfo.sin_addr = ip_to_num;
    servInfo.sin_port = htons(stoi(port));

    opStatus = connect(sockCli, (sockaddr*)&servInfo, sizeof(servInfo));
    if (opStatus != 0)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка подключения к серверу." << std::endl;
        disconnect();
        exit(1);
    }
}

void net::sendRequest(const std::string& str)
{
    ZeroMemory(&package, pkg_length);
    strcpy(package, str.data());
    opStatus = send(sockCli, package, sizeof(package), 0);
    if (opStatus == SOCKET_ERROR)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << "Ошибка отправки пакета серверу." << std::endl;
        disconnect();
        exit(1);
    }
    
}

void net::getRequest()
{
    ZeroMemory(&package, pkg_length);
    opStatus = recv(sockCli, package, sizeof(package), 0);
    if (opStatus == SOCKET_ERROR)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка получения ответа от сервера." << std::endl;
        disconnect();
        exit(1);
    }
    else
    {
        std::cout << "[NET] Request \'" << package << "\' accepted" << std::endl;
    }
}

void net::disconnect()
{
    closesocket(sockCli);
    WSACleanup();
}

void net::netStart()
{
    connServer();
    size_t i = 0;
    while (true)
    {
        sendRequest("Test" + std::to_string(i));
        getRequest();
        ++i;
    }
}