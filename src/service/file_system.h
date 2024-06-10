#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <map>

namespace Service
{
	/**
	 * @brief Manages file paths and their corresponding data.
	 */
	class File_System
	{
		private:
			std::map<std::string, std::string> _files;  /**< File paths and their data. */
			mutable std::mutex                 _mutex;  /**< Mutex for thread safety. */
		
		public:
			File_System();
			~File_System();
			
			/**
			 * @brief Loads a file into the File_System.
			 * 
			 * @param file_path The path of the file to load.
			 */
			void Load(const std::string& file_path);
			
			/**
			 * @brief Retrieves the data of a file from the File_System.
			 * 
			 * @param file_path The path of the file to retrieve.
			 * @param data Reference to store the file data.
			 * @return True if the file was found and data was retrieved, false otherwise.
			 */
			bool GetFile(const std::string& file_path, std::string& data);
			
			/**
			 * @brief Unloads all files from the File_System.
			 */
			void UnLoadAll();
	};
}

#endif // FILE_SYSTEM_H
