#include "Logger.h"
Logger::Logger()
{
	
}

Logger::~Logger()
{
	
}

void Logger::recLogEntry(const std::string& entry)
{
	s_mutex.lock();
	slog.open(log_file, std::ios::in | std::ios::app);
	slog << logTimeStamp();
	slog << entry;
	slog << "\n";
	slog.close();
	s_mutex.unlock();
}

void Logger::readAllEntry()
{
	s_mutex.lock_shared();
	slog.open(log_file, std::ios::in | std::ios::app);
	if (slog.is_open())
	{
		std::string str;
		while (std::getline(slog, str))
		{
			std::cout << str << std::endl;
		}
		slog.close();
	}
	s_mutex.unlock_shared();
}