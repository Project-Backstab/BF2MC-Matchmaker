#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>

extern std::ofstream g_logger;
extern std::mutex    g_mutex_logger;

namespace Logger
{
	void Initialize();
	
	void info(const std::string& msg, bool show_console = true);
	void warning(const std::string& msg, bool show_console = true);
	void error(const std::string& msg, bool show_console = true);
	void critical(const std::string& msg, bool show_console = true);
	void debug(const std::string& msg);
}

#endif // LOGGER_H
