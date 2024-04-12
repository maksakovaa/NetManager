#pragma once
#include "Logger.h"

extern Logger Log;
extern Settings* Config;

class net
{
    friend class menu;
private:
    bool isShutdown = false;
    bool statusMainThread = false;
 	static const int pkg_length = 1024;
    static const int cli_id_length = 64;
    struct netObj
    {
        netObj() {
            addrSize = sizeof(addr);
            sock = 0;
#if defined (_WIN32) || defined (_WIN64)
            ZeroMemory(&addr, addrSize);
            ZeroMemory(&package, pkg_length);
            ZeroMemory(&cli_id, cli_id_length);
#elif defined (__linux__)
            sock = 0;
            bzero(&addr, addrSize);
#endif
            }
#if defined (_WIN32) || defined (_WIN64)
        int addrSize;
        SOCKET sock;
        char cli_id[cli_id_length];
#elif defined (__linux__)
        int sock;
        socklen_t addrSize;
        char* cli_id;
#endif
        sockaddr_in addr;
        char package[pkg_length];
        std::future<void> thread;
    };
    void initWinsock();
    void disconnect();
    void createSocket();
    void bindSocket();
    void servListen();
    void acceptConn();
    bool sendReq(netObj* cli);
    bool getReq(netObj* cli);
    void threadCycle(netObj* cli);
    void connChecker();
    void netManager();
    void logger(const std::string& entry);
    std::string port = Config->getChatPort();;
    int max_clients = Config->getMaxClients();
#if defined(_WIN32) || defined (_WIN64)
    WSADATA WSAData;
#endif
    netObj srvObj;
    std::vector<netObj*> cliObj;
    int opStatus;
    std::mutex m_connThreads;
    std::mutex cout;
public:
    net();
    ~net();
    void run();
    void stop();
};