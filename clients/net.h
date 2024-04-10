#include <iostream>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#elif defined(__linux__)
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#endif
class net
{
public:
    net();
    ~net();
    void netStart();
private:
    void convert_ip();
    void initWinSock();
    void createSocket();
    void connServer();
    void sendRequest(const std::string& str);
    void getRequest();
    void disconnect();
    std::string server_ip = "127.0.0.1";
    std::string port = "48090";
   	static const int pkg_length = 1024;
	char package[pkg_length];
    int opStatus;
    sockaddr_in servInfo;
#if defined(_WIN32) || defined(_WIN64)
    in_addr ip_to_num;
    WSADATA wsaData;
    SOCKET sockCli;
#elif defined(__linux__)
    int sockCli;
#endif
};
