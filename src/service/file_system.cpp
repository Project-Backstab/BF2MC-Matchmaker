#include <fstream>
#include <mutex>

#include <logger.h>
#include <service/file_system.h>

Service::File_System::File_System()
{
	
}

Service::File_System::~File_System()
{
	
}

void Service::File_System::Load(const std::string& file_path)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	// Check file is already loaded
	auto it = this->_files.find(file_path);
	if(it != this->_files.end())
	{
		Logger::error("File \"" + file_path + "\" already loaded in memory.");
		return;
	}
	
	std::ifstream input;
	std::string data;
	
	input.open(file_path, std::ifstream::in | std::ifstream::binary);

	if(input.is_open())
	{
		// Get the size of the file
		input.seekg(0, std::ios::end);
		size_t file_size = input.tellg();
		input.seekg(0, std::ios::beg);

		// Reserve space in the string
		data.reserve(file_size);

		// Read the file content into the string
		data.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
		
		// Close file
		input.close();
		
		// Save in memory
		this->_files.insert({ file_path, data });
	}
	else
	{
		Logger::error("Can't open file \"" + file_path + "\"");
	}
}

bool Service::File_System::GetFile(const std::string& file_path, std::string& data)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	auto it = this->_files.find(file_path);
	
	if(it == this->_files.end())
	{
		return false;
	}
	
	data = it->second;
	return true;
}

void Service::File_System::UnLoadAll()
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	this->_files.clear();
}