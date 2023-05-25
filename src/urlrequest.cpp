#include <sstream>
#include <iostream>

#include <urlrequest.h>

void UrlRequest::GetUrlElements(const std::string &url, std::string &url_base, std::map<std::string, std::string> &url_variables)
{
	std::stringstream input, input2;
	std::string str_url_variables, url_variable, key, value;
	
	input.str(url);
	
	// Get url_base
	std::getline(input, url_base, '?');
	
	// Get url variables
	if(std::getline(input, str_url_variables, '?'))
	{
		input.clear();
		input.str(str_url_variables);
		
		// Get get url variable
		while(std::getline(input, url_variable, '&'))
		{
			input2.clear();
			input2.str(url_variable);
			
			// Get key
			std::getline(input2, key, '=');
			
			// Get value
			if(std::getline(input2, value, '='))
			{
				url_variables.insert({ key, value });
			}
		}
	}
}

