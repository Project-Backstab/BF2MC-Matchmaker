#ifndef URLREQUEST_H
#define URLREQUEST_H

#include <string>
#include <vector>
#include <map>

namespace UrlRequest
{
	typedef std::vector<std::string> Parameter;
	typedef std::map<std::string, std::string> UrlVariables;
	
	UrlRequest::Parameter Request2Parameter(const std::string &request);
	std::string Parameter2Response(const UrlRequest::Parameter &parameter);
	
	void GetUrlElements(const std::string &url, std::string &url_base, UrlRequest::UrlVariables &url_variables);
}

#endif // URLREQUEST_H
