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
	
	g_logger.open(path + "/bf2mc-" + Util::GetNowDateTime() + ".log", std::ios::app);
}

void Logger::info(const std::string& msg, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)
	
	if(show_console)
	{
		std::cout << "[" << Util::GetNowTime() << "][INFO] " << msg << std::endl;
	}
	
	g_logger  << "[" << Util::GetNowTime() << "][INFO] " << msg << std::endl;
	g_logger.flush();
}

void Logger::warning(const std::string& msg, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)
	
	if(show_console)
	{
		std::cout << "[" << Util::GetNowTime() << "][\e[1;33mWARNING\e[0m] " << msg << std::endl;
	}
	
	g_logger  << "[" << Util::GetNowTime() << "][WARNING] " << msg << std::endl;
	g_logger.flush();
}

void Logger::error(const std::string& msg, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)
	
	if(show_console)
	{
		std::cout << "[" << Util::GetNowTime() << "][\e[1;31mERROR\e[0m] " << msg << std::endl;
	}
	
	g_logger  << "[" << Util::GetNowTime() << "][ERROR] " << msg << std::endl;
	g_logger.flush();
}

void Logger::critical(const std::string& msg, bool show_console)
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)
	
	if(show_console)
	{
		std::cout << "[" << Util::GetNowTime() << "][\e[1;31mCRITICAL\e[0m] " << msg << std::endl;
	}
	
	g_logger  << "[" << Util::GetNowTime() << "][CRITICAL] " << msg << std::endl;
	g_logger.flush();
}

void Logger::debug(const std::string& msg)
{
	std::unique_lock<std::mutex> guard(g_mutex_logger); // logger lock (read/write)
	
	std::cout << "[" << Util::GetNowTime() << "][DEBUG] " << msg << std::endl;
	//g_logger  << "[" << Util::GetNowTime() << "][DEBUG] " << msg << std::endl;
	//g_logger.flush();
}

