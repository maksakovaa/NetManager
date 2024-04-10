#include "net.h"

net::net()
{
#if defined(_WIN32) || defined(_WIN64)
    convert_ip();
    initWinSock();
#endif
    createSocket();
}

net::~net()
{
    disconnect();
}

#if defined(_WIN32) || defined(_WIN64)
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
#endif

void net::createSocket()
{
    sockCli = socket(AF_INET, SOCK_STREAM, 0);
#if defined(_WIN32) || defined(_WIN64)
    if (sockCli == INVALID_SOCKET)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка создания сокета." << std::endl;
        exit(1);
    }
#elif defined(__linux__)
    if (sockCli == -1)
    {
        std::cout << " Ошибка создания сокета." << std::endl;
        exit(1);
    }
#endif
    else
    {
        std::cout << "[NET] Сокет создан." << std::endl;
    }
}

void net::connServer()
{
#if defined (_WIN32) || defined (_WIN64)
    ZeroMemory(&servInfo, sizeof(servInfo));
    servInfo.sin_addr = ip_to_num;
#elif defined(__linux__)
    bzero(&servInfo, sizeof(servInfo));
    inet_aton(server_ip.data(), &servInfo.sin_addr);
//    servInfo.sin_addr.s_addr = inet_addr(server_ip.data());
#endif
    servInfo.sin_family = AF_INET;
    servInfo.sin_port = htons(stoi(port));

    opStatus = connect(sockCli, (sockaddr*)&servInfo, sizeof(servInfo));
#if defined(_WIN32) || defined(_WIN64)
    if (opStatus != 0)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка подключения к серверу." << std::endl;
        disconnect();
        exit(1);
    }
#elif defined(__linux__)
    if (opStatus == -1)
    {
        std::cout << "[NET] ERROR: Ошибка подключения к серверу." << std::endl;
        disconnect();
        exit(1);
    }
#endif
    else
    {
        std::cout << "[NET] Подключение установлено." << std::endl;
    }
}

void net::sendRequest(const std::string& str)
{
#if defined(_WIN32) || defined(_WIN64)
    ZeroMemory(&package, pkg_length);
    strcpy(package, str.data());
    opStatus = send(sockCli, package, sizeof(package), 0);
    if (opStatus == SOCKET_ERROR)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка отправки пакета серверу." << std::endl;
        disconnect();
        exit(1);
    }
#elif defined(__linux__)
    bzero(&package, pkg_length);
    strcpy(package, str.data());
    ssize_t bytes = write(sockCli, package, sizeof(package));
    if (bytes < 0)
    {
        std::cout << "[NET] ERROR: Ошибка отправки пакета серверу." << std::endl;
        disconnect();
        exit(1);
    }
#endif
    else
    {
        std::cout << "[NET] Пакет \'" << str << "\' отправлен."<< std::endl;
    }
}

void net::getRequest()
{
#if defined(_WIN32) || defined(_WIN64)
    ZeroMemory(&package, pkg_length);
    opStatus = recv(sockCli, package, sizeof(package), 0);
    if (opStatus == SOCKET_ERROR)
    {
        std::cout << "[NET] ERROR: " << WSAGetLastError() << " Ошибка получения ответа от сервера." << std::endl;
        disconnect();
        exit(1);
    }
#elif defined(__linux__)
    bzero(&package, pkg_length);
    opStatus = read(sockCli, package, sizeof(package));
    if (opStatus < 0)
    {
        std::cout << "[NET] ERROR: Ошибка получения ответа от сервера." << std::endl;
        disconnect();
        exit(1);
    }
#endif
    else
    {
        std::cout << "[NET] Request \'" << package << "\' accepted" << std::endl;
    }
}

void net::disconnect()
{
#if defined(_WIN32) || defined(_WIN64)
    closesocket(sockCli);
    WSACleanup();
#elif defined(__linux__)
    close(sockCli);
#endif
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