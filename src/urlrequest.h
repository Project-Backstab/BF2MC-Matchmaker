#ifndef URLREQUEST_H
#define URLREQUEST_H

#include <string>
#include <map>

namespace UrlRequest
{
	typedef std::map<std::string, std::string> UrlVariables;
	
	void GetUrlElements(const std::string& url, std::string& url_base, UrlRequest::UrlVariables& url_variables);
}

#endif // URLREQUEST_H
