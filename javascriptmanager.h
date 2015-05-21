#ifndef JAVASCRIPTMANAGER_H
#define JAVASCRIPTMANAGER_H

#include "common.h"

#ifdef WITH_JAVASCRIPT
#include <include/v8.h>
#include <include/libplatform/libplatform.h>

namespace openrc {

/**
 * @brief Javascript controller manager.
 *
 * Initialize v8 engine. Search and load javascript controllers.
 */
class JavascriptManager
{
private:
	JavascriptManager();
	~JavascriptManager();

public:
	/**
	 * @brief Initialize js by path.
	 * @param path directory with js files.
	 */
	void Initialize(const std::string& path);

	/**
	 * @brief Shutdown all controllers.
	 */
	void Shutdown();

	int32 Execute(
		const std::string& file, 
		const std::map<std::string, std::string>& options, 
		const std::map<std::string, std::string>& argumentsMap, 
		std::map<std::string, std::string>& output);

	v8::Isolate* GetIsolate() { return isolate_; }
	v8::Persistent<v8::Context>& GetContext() { return context_; }

	/**
	 * @brief Singleton instance.
	 */
	static JavascriptManager& Instance();
	static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
	void LoadScritps(const std::string& folder);
	bool InitializeJs();
	bool ShutdownJs();

	bool InstallMaps(std::map<std::string, std::string>* opts, std::map<std::string, std::string>* output);

	v8::Handle<v8::Object> WrapMap(std::map<std::string, std::string>* obj);
	static std::map<std::string, std::string>* UnwrapMap(v8::Handle<v8::Object> obj);

	static std::string ObjectToString(v8::Local<v8::Value> value);
	static void MapGet(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info);
	static void MapSet(v8::Local<v8::Name> name, v8::Local<v8::Value> value_obj, const v8::PropertyCallbackInfo<v8::Value>& info);
	v8::Handle<v8::ObjectTemplate> MakeMapTemplate(v8::Isolate* isolate);

	
	

	static v8::Persistent<v8::ObjectTemplate> map_template_;

private:
	std::vector<class JavascriptController*> _controllers;
	std::map<std::string, std::string> _options;
	std::map<std::string, std::string> _argumentsMap;
	std::map<std::string, std::string> _outputs;

	v8::Isolate* isolate_;
	v8::Persistent<v8::Context> context_;
};

} // End http.

#endif
#endif // JAVASCRIPTMANAGER_H
