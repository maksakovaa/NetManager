#pragma once
#include "reqAPI.h"

extern Logger* Log;
extern Settings* Config;
extern ReqAPI* API;

class Net
{
    friend class Menu;
private:
    bool isShutdown = false;
    bool statusMainThread = false;
 	static const int pkg_length = 1024;
    static const int cli_id_length = 64;
    struct NetObj
    {
        NetObj() {
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
    std::string port;
    int max_clients;
#if defined(_WIN32) || defined (_WIN64)
    WSADATA WSAData;
#endif
    NetObj srvObj;
    std::vector<NetObj*> cliObj;
    int opStatus;
    std::mutex m_connThreads;
    void initWinsock();
    void disconnect();
    void createSocket();
    void bindSocket();
    void servListen();
    void acceptConn();
    bool sendReq(NetObj* cli);
    bool getReq(NetObj* cli);
    void threadCycle(NetObj* cli);
    void netManager();
    void logger(const std::string& entry);
    void reqHandler(NetObj* cli);
public:
    Net();
    ~Net();
    void run();
    void stop();
};