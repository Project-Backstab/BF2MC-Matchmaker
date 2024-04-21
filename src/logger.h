#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>

#include <server.h>
#include <service.h>

namespace Logger
{
	/**
	 * @brief Enumeration for different logging modes.
	 */
	enum Mode
	{
		Development = 0x1, ///< Verbose logging mode.
		Production = 0x2,  ///< Standard logging mode.
		Deployment = 0x4   ///< Minimal logging mode.
	};
	
	/**
	 * @brief Initializes the logger.
	 */
	void Initialize();
	
	/**
	 * @brief Converts Server::Type enum values to string.
	 * 
	 * @param type The Server::Type enum value.
	 * @return A string representation of the Server type.
	 */
	std::string ToString(enum Server::Type type);

	/**
	 * @brief Converts Service::Type enum values to string.
	 * 
	 * @param type The Service::Type enum value.
	 * @return A string representation of the Service type.
	 */
	std::string ToString(enum Service::Type type);
	
	/**
	 * @brief Logs an informational message.
	 * 
	 * @param msg The message to log.
	 * @param type Optional type of the log message.
	 * @param show_console Whether to display the message in the console.
	 */
	void info(const std::string& msg, const std::string& type = "", bool show_console = true);

	/**
	 * @brief Logs a warning message.
	 * 
	 * @param msg The message to log.
	 * @param type Optional type of the log message.
	 * @param show_console Whether to display the message in the console.
	 */
	void warning(const std::string& msg, const std::string& type = "", bool show_console = true);
	
	/**
	 * @brief Logs an error message.
	 * 
	 * @param msg The message to log.
	 * @param type Optional type of the log message.
	 * @param show_console Whether to display the message in the console.
	 */
	void error(const std::string& msg, const std::string& type = "", bool show_console = true);

	/**
	 * @brief Logs a critical error message.
	 * 
	 * @param msg The message to log.
	 * @param type Optional type of the log message.
	 * @param show_console Whether to display the message in the console.
	 */
	void critical(const std::string& msg, const std::string& type = "", bool show_console = true);

	/**
	 * @brief Logs an informational message related to a Server.
	 * 
	 * @param msg The message to log.
	 * @param type The Server::Type enum value.
	 * @param show_console Whether to display the message in the console.
	 */
	void info(const std::string& msg, enum Server::Type type, bool show_console = true);

	/**
	 * @brief Logs a warning message related to a Server.
	 * 
	 * @param msg The message to log.
	 * @param type The Server::Type enum value.
	 * @param show_console Whether to display the message in the console.
	 */
	void warning(const std::string& msg, enum Server::Type type, bool show_console = true);

	/**
	 * @brief Logs an error message related to a Server.
	 * 
	 * @param msg The message to log.
	 * @param type The Server::Type enum value.
	 * @param show_console Whether to display the message in the console.
	 */
	void error(const std::string& msg, enum Server::Type type, bool show_console = true);

	/**
	 * @brief Logs a critical error message related to a Server.
	 * 
	 * @param msg The message to log.
	 * @param type The Server::Type enum value.
	 * @param show_console Whether to display the message in the console.
	 */
	void critical(const std::string& msg, enum Server::Type type, bool show_console = true);

	/**
	 * @brief Logs an informational message related to a Service.
	 * 
	 * @param msg The message to log.
	 * @param type The Service::Type enum value.
	 * @param show_console Whether to display the message in the console.
	 */
	void info(const std::string& msg, enum Service::Type type, bool show_console = true);

	/**
	 * @brief Logs a warning message related to a Service.
	 * 
	 * @param msg The message to log.
	 * @param type The Service::Type enum value.
	 * @param show_console Whether to display the message in the console.
	 */
	void warning(const std::string& msg, enum Service::Type type, bool show_console = true);

	/**
	 * @brief Logs an error message related to a Service.
	 * 
	 * @param msg The message to log.
	 * @param type The Service::Type enum value.
	 * @param show_console Whether to display the message in the console.
	 */
	void error(const std::string& msg, enum Service::Type type, bool show_console = true);

	/**
	 * @brief Logs a critical error message related to a Service.
	 * 
	 * @param msg The message to log.
	 * @param type The Service::Type enum value.
	 * @param show_console Whether to display the message in the console.
	 */
	void critical(const std::string& msg, enum Service::Type type, bool show_console = true);

	/**
	 * @brief Logs a debug message.
	 * 
	 * @param msg The debug message to log.
	 */
	void debug(const std::string& msg);
}

extern std::ofstream g_logger; /**< The global logger output stream. */
extern std::mutex    g_logger_mutex; /**< Mutex for thread-safe logging. */
extern Logger::Mode  g_logger_mode; /**< The current logging mode. */

#endif // LOGGER_H
