#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>

#include <server.h>
#include <service.h>

namespace Logger
{
	enum class Type
	{
		Development  = 0,
		Deployment = 1,
	};
	
	void Initialize();
	
	std::string ToString(enum Server::Type type);
	std::string ToString(enum Service::Type type);
	
	void info(const std::string& msg, const std::string& type = "", bool show_console = true);
	void warning(const std::string& msg, const std::string& type = "", bool show_console = true);
	void error(const std::string& msg, const std::string& type = "", bool show_console = true);
	void critical(const std::string& msg, const std::string& type = "", bool show_console = true);
	
	void info(const std::string& msg, enum Server::Type type, bool show_console = true);
	void warning(const std::string& msg, enum Server::Type type, bool show_console = true);
	void error(const std::string& msg, enum Server::Type type, bool show_console = true);
	void critical(const std::string& msg, enum Server::Type type, bool show_console = true);
	
	void info(const std::string& msg, enum Service::Type type, bool show_console = true);
	void warning(const std::string& msg, enum Service::Type type, bool show_console = true);
	void error(const std::string& msg, enum Service::Type type, bool show_console = true);
	void critical(const std::string& msg, enum Service::Type type, bool show_console = true);
	
	void debug(const std::string& msg);
}

extern std::ofstream g_logger;
extern std::mutex    g_logger_mutex;
extern Logger::Type  g_logger_type;

#endif // LOGGER_H
