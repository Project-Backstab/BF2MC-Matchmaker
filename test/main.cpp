#include <iostream>
#include <string>
#include <fstream>
#include <thread>

void readFile(const std::string& file_name)
{
	bool finished = false;
	std::string data;
	
	while(!finished)
	{
		std::ifstream input;
	
		try
		{
			input.open("../" + file_name, std::ifstream::in | std::ifstream::binary);

			if(input.is_open())
			{
				data.append((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
				
				input.close();
				
				finished = true;
			}
		}
		catch(...) {
			std::cout << file_name << std::endl;
		}
	}
}

int main()
{
	for(int i = 0; i < 1000; i++)
	{
		// Start servers
		std::thread t(&readFile, "main.py");
		t.detach();
	}
}