#pragma once
#include "settings.h"

class Logger
{
public:
	Logger();
	~Logger();
	void recLogEntry(const std::string& entry);
	void readAllEntry();
private:
#if defined (_WIN32) || defined (_WIN64)
	std::string log_file = "main_log.txt";
	int diff = -2;
#elif defined (__linux__)
	std::string log_file = "./main_log.txt";
	int diff = -1;
#endif
	std::fstream slog;
	std::shared_mutex s_mutex;
	std::string logEntry = "";
};