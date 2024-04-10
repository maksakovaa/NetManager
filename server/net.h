#include <iostream>
#include <vector>
#include <string>
#include <future>
#include <chrono>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib") 


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
        netObj(): sock(0) { addrSize = sizeof(addr); ZeroMemory(&addr, addrSize); ZeroMemory(&package, pkg_length); ZeroMemory(&cli_id, cli_id_length); }
        netObj(SOCKET& _s, sockaddr_in& _a): sock(_s), addr(_a) { addrSize = sizeof(_a); ZeroMemory(&package, pkg_length); ZeroMemory(&cli_id, cli_id_length); }
        SOCKET sock;
        sockaddr_in addr;
        int addrSize;
        char package[pkg_length];
        char cli_id[cli_id_length];
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
    bool threadCycle(netObj* cli);
    void connChecker();
    std::string port = "48090";
    std::string server_ip = "127.0.0.1";
    WSADATA WSAData;
    netObj srvObj;
    in_addr ip_to_num;
    std::vector<netObj*> cliObj;
    int opStatus;
    std::vector<std::future<bool>> connThreads;
};