#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <map>

namespace Service
{
	class File_System
	{
		private:
			std::map<std::string, std::string> _files;
			mutable std::mutex                 _mutex;
		
		public:
			File_System();
			~File_System();
			
			void Load(const std::string& file_path);
			bool GetFile(const std::string& file_path, std::string& data);
			void UnLoadAll();
	};
}

#endif // FILE_SYSTEM_H
