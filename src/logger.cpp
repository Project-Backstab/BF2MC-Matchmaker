#include <iostream>
#include <filesystem>

#include <util.h>

#include <logger.h>

// Global
std::ofstream g_logger;
std::mutex    g_mutex_logger;

void Logger::Initialize()
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)

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
		case Server::Type::None: return ""; break;
	}
	
	return "";
}

void Logger::info(const std::string& msg, enum Server::Type type, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)
	
	std::string server_type = Logger::ToString(type);
	std::string time = Util::Time::GetNowDateTime("%H:%M:%S");
	
	if(show_console)
	{
		std::cout << "[" << time << "]" << server_type << "[INFO] " << msg << std::endl;
	}
	
	g_logger  << "[" << time << "]" << server_type << "[INFO] " << msg << std::endl;
	g_logger.flush();
}

void Logger::warning(const std::string& msg, enum Server::Type type, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)
	
	std::string server_type = Logger::ToString(type);
	std::string time = Util::Time::GetNowDateTime("%H:%M:%S");
	
	if(show_console)
	{
		std::cout << "[" << time << "]" << server_type << "[\e[1;33mWARNING\e[0m] " << msg << std::endl;
	}
	
	g_logger  << "[" << time << "]" << server_type << "[WARNING] " << msg << std::endl;
	g_logger.flush();
}

void Logger::error(const std::string& msg, enum Server::Type type, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)
	
	std::string server_type = Logger::ToString(type);
	std::string time = Util::Time::GetNowDateTime("%H:%M:%S");
	
	if(show_console)
	{
		std::cout << "[" << time << "]" << server_type << "[\e[1;31mERROR\e[0m] " << msg << std::endl;
	}
	
	g_logger  << "[" << time << "]" << server_type << "[ERROR] " << msg << std::endl;
	g_logger.flush();
}

void Logger::critical(const std::string& msg, enum Server::Type type, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)
	
	std::string server_type = Logger::ToString(type);
	std::string time = Util::Time::GetNowDateTime("%H:%M:%S");
	
	if(show_console)
	{
		std::cout << "[" << time << "]" << server_type << "[\e[1;31mCRITICAL\e[0m] " << msg << std::endl;
	}
	
	g_logger  << "[" << time << "]" << server_type << "[CRITICAL] " << msg << std::endl;
	g_logger.flush();
}

void Logger::debug(const std::string& msg)
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)
	
	std::string time = Util::Time::GetNowDateTime("%H:%M:%S");
	
	std::cout << "[" << time << "][DEBUG] " << msg << std::endl;
	//g_logger  << "[" << Util::Time::GetNowTime() << "][DEBUG] " << msg << std::endl;
	//g_logger.flush();
}

