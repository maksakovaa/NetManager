#include <iostream>
#include <vector>
#include <string>
#include <future>
#include <chrono>
#include <thread>
#include <mutex>

#if defined (_WIN32) || defined (_WIN64)
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#elif defined (__linux__)
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

class net
{
public:
    net();
    ~net();
    void runServer();
private:
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
    void convert_ip();
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
    std::string port = "48090";
    std::string server_ip = "127.0.0.1";
#if defined(_WIN32) || defined (_WIN64)
    WSADATA WSAData;
    in_addr ip_to_num;
#endif
    netObj srvObj;
    std::vector<netObj*> cliObj;
    int opStatus;
    std::mutex m_connThreads;
};