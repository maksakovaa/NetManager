#pragma once
#pragma warning(disable : 4996)
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <string>
#include <future>
#include <chrono>
#include <thread>
#include <shared_mutex>


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

std::string curDateTime();
std::string logTimeStamp();

class Settings
{
	friend class net;
public:
	Settings();
	~Settings();
	const std::string& getChatPort();
	int getMaxClients();
	const std::string& getMysqlSrvIP();
	const std::string& getMysqlLogin();
	const std::string& getMysqlPass();
	const std::string& getMysql_database();
	const std::string& getMysql_table_users();
	const std::string& getMysql_table_AM();
	const std::string& getMysql_table_AMS();
	const std::string& getMysql_table_PM();
	void changeParam(int param);
private:
	void saveConfig();
	std::string& getValue(std::string& str);
	std::string delim_settings = " = ";
	std::string chat_port;
	int max_clients;
	std::string mysql_srv_ip;
	std::string mysql_login;
	std::string mysql_pass;
	std::string mysql_database;
	std::string mysql_table_users;
	std::string mysql_table_AM;
	std::string mysql_table_AMS;
	std::string mysql_table_PM;
#if defined (_WIN32) || defined (_WIN64)
	std::string cfgPath = "settings.ini";
#elif defined (__linux__)
	std::string cfgPath = "./settings.ini";
#endif
};