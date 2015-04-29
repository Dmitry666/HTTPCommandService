#ifndef JAVASCRIPTMANAGER_H
#define JAVASCRIPTMANAGER_H

#include "common.h"

#ifdef WITH_JAVASCRIPT
#include <include/v8.h>
#include <include/libplatform/libplatform.h>

namespace http {

class JavascriptManager
{
public:
	JavascriptManager();
	~JavascriptManager();

	void Initialize(int argc, char* argv[]);
	void Shutdown();

	int32 Execute(
		const std::string& file, 
		const std::map<std::string, std::string>& options, 
		const std::map<std::string, std::string>& argumentsMap, 
		std::map<std::string, std::string>& output);

	static JavascriptManager& Instance();

private:
	void LoadScritps(const std::string& folder);

private:
	std::vector<class JavascriptController*> _controllers;
	std::map<std::string, std::string> _options;
	std::map<std::string, std::string> _argumentsMap;
	std::map<std::string, std::string> _outputs;

	v8::Isolate* _isolate;
};

} // End http.

#endif
#endif // JAVASCRIPTMANAGER_H
