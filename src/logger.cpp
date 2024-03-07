#include <iostream>
#include <filesystem>

#include <util.h>

#include <logger.h>

// Global
std::ofstream g_logger;
std::mutex    g_logger_mutex;
Logger::Mode  g_logger_mode = Logger::Mode::Development;

void Logger::Initialize()
{
	std::unique_lock<std::mutex> guard(g_logger_mutex); // logger lock (read/write)

	std::string path = "../data/log";

	// Check if the directory already exists
	if (!std::filesystem::exists(path))
	{
		// Create the directory
		if (std::filesystem::create_directory(path))
		{
			std::cout << "[INFO] Create \"data/log/\" directory!" << std::endl;
		}
		else
		{
			std::cout << "[ERROR] Can't create \"data/log/\" directory!" << std::endl;
		}
	}

	g_logger.open(path + "/bf2mc-" + Util::Time::GetNowDateTime("%Y%m%d-%H%M%S") + ".log", std::ios::app);
}

std::string Logger::ToString(enum Server::Type type)
{
	switch(type)
	{
		case Server::Type::GPSP: return "[GPSP]"; break;
		case Server::Type::GPCM: return "[GPCM]"; break;
		case Server::Type::Webserver: return "[Webserver]"; break;
		case Server::Type::Browsing: return "[Browsing]"; break;
		case Server::Type::GameStats: return "[GameStats]"; break;
		case Server::Type::QR: return "[QR]"; break;
		case Server::Type::Websocket: return "[Websocket]"; break;
		case Server::Type::None: return ""; break;
	}
	
	return "";
}

std::string Logger::ToString(enum Service::Type type)
{
	switch(type)
	{
		case Service::Type::FileSystem: return "[FileSystem]"; break;
		case Service::Type::None: return ""; break;
	}
	
	return "";
}

void Logger::info(const std::string& msg, const std::string& type, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_logger_mutex); // logger lock (read/write)

	std::string time = Util::Time::GetNowDateTime("%H:%M:%S");
	
	if(show_console)
	{
		std::cout << "[" << time << "]" << type << "[INFO] " << msg << std::endl;
	}
	
	g_logger  << "[" << time << "]" << type << "[INFO] " << msg << std::endl;
	g_logger.flush();
}

void Logger::warning(const std::string& msg, const std::string& type, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_logger_mutex); // logger lock (read/write)
	
	std::string time = Util::Time::GetNowDateTime("%H:%M:%S");
	
	if(show_console)
	{
		std::cout << "[" << time << "]" << type << "[\e[1;33mWARNING\e[0m] " << msg << std::endl;
	}
	
	g_logger  << "[" << time << "]" << type << "[WARNING] " << msg << std::endl;
	g_logger.flush();
}

void Logger::error(const std::string& msg, const std::string& type, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_logger_mutex); // logger lock (read/write)
	
	std::string time = Util::Time::GetNowDateTime("%H:%M:%S");
	
	if(show_console)
	{
		std::cout << "[" << time << "]" << type << "[\e[1;31mERROR\e[0m] " << msg << std::endl;
	}
	
	g_logger  << "[" << time << "]" << type << "[ERROR] " << msg << std::endl;
	g_logger.flush();
}

void Logger::critical(const std::string& msg, const std::string& type, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_logger_mutex); // logger lock (read/write)
	
	std::string time = Util::Time::GetNowDateTime("%H:%M:%S");
	
	if(show_console)
	{
		std::cout << "[" << time << "]" << type << "[\e[1;31mCRITICAL\e[0m] " << msg << std::endl;
	}
	
	g_logger  << "[" << time << "]" << type << "[CRITICAL] " << msg << std::endl;
	g_logger.flush();
}

void Logger::info(const std::string& msg, enum Server::Type type, bool show_console)
{
	std::string server_type = Logger::ToString(type);
	
	Logger::info(msg, server_type, show_console);
}

void Logger::warning(const std::string& msg, enum Server::Type type, bool show_console)
{
	std::string server_type = Logger::ToString(type);
	
	Logger::warning(msg, server_type, show_console);
}

void Logger::error(const std::string& msg, enum Server::Type type, bool show_console)
{
	std::string server_type = Logger::ToString(type);
	
	Logger::error(msg, server_type, show_console);
}

void Logger::critical(const std::string& msg, enum Server::Type type, bool show_console)
{
	std::string server_type = Logger::ToString(type);
	
	Logger::critical(msg, server_type, show_console);
}

void Logger::info(const std::string& msg, enum Service::Type type, bool show_console)
{
	std::string service_type = Logger::ToString(type);
	
	Logger::info(msg, service_type, show_console);
}

void Logger::warning(const std::string& msg, enum Service::Type type, bool show_console)
{
	std::string service_type = Logger::ToString(type);
	
	Logger::warning(msg, service_type, show_console);
}

void Logger::error(const std::string& msg, enum Service::Type type, bool show_console)
{
	std::string service_type = Logger::ToString(type);
	
	Logger::error(msg, service_type, show_console);
}

void Logger::critical(const std::string& msg, enum Service::Type type, bool show_console)
{
	std::string service_type = Logger::ToString(type);
	
	Logger::critical(msg, service_type, show_console);
}

void Logger::debug(const std::string& msg)
{
	std::unique_lock<std::mutex> guard(g_logger_mutex); // logger lock (read/write)
	
	std::string time = Util::Time::GetNowDateTime("%H:%M:%S");
	
	std::cout << "[" << time << "][DEBUG] " << msg << std::endl;
	//g_logger  << "[" << Util::Time::GetNowTime() << "][DEBUG] " << msg << std::endl;
	//g_logger.flush();
}

