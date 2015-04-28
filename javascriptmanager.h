#ifndef JAVASCRIPTMANAGER_H
#define JAVASCRIPTMANAGER_H

#include "common.h"

namespace http {

class JavascriptManager
{
public:
	JavascriptManager();
	~JavascriptManager();

	void Initialize(int argc, char* argv[]);
	void Shutdown();

	int32 Execute(const std::string& file, const std::map<std::string, std::string>& options, std::map<std::string, std::string>& output);

	static JavascriptManager& Instance();

private:
	void LoadScritps(const std::string& folder);

private:
	std::vector<class JavascriptController*> _controllers;
};

} // End http.

#endif // JAVASCRIPTMANAGER_H
