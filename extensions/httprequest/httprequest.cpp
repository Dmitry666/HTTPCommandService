#include "httprequest.h"

#ifdef WITH_JAVASCRIPT
#include <v8pp/class.hpp>
#include <v8pp/module.hpp>
#endif

#include <future>

#ifdef WITH_JAVASCRIPT
using namespace v8;

XMLHttpRequest::XMLHttpRequest( const v8::FunctionCallbackInfo<v8::Value>& args )
	: _bAsync(true)
	, _curl(nullptr)
{
	_curl = curl_easy_init();
	//value_ = v8pp::from_v8<double>(args.GetIsolate(), args[0], 0);
}
#else
XMLHttpRequest::XMLHttpRequest()
	: _bAsync(true)
	, _curl(nullptr)
{
	_curl = curl_easy_init();
}
#endif

XMLHttpRequest::~XMLHttpRequest()
{
	curl_easy_cleanup(_curl);
}

void XMLHttpRequest::Open(const std::string& method, const std::string& url, bool bAsync)
{
	_bAsync = bAsync;

	curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, errorBuffer);
	curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());

	curl_easy_setopt(_curl, CURLOPT_HEADER, 1);

    //curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);

	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_responseText);
}

void XMLHttpRequest::Open(const std::string& method, const std::string& url, bool bAsync, const std::string& userName)
{
	Open(method, url, bAsync);

	curl_easy_setopt(_curl, CURLOPT_USERNAME, userName.c_str());	
}

void XMLHttpRequest::Open(const std::string& method, const std::string& url, bool bAsync, const std::string& userName, const std::string& password)
{
	Open(method, url, bAsync, userName);

	curl_easy_setopt(_curl, CURLOPT_PASSWORD, password.c_str());
}

void XMLHttpRequest::Send(const std::string& params)
{
	if(!_requestHeader.empty())
	{
		struct curl_slist *chunk = nullptr;
		for(auto& pair : _requestHeader)
		{
			const std::string requestHeader = pair.first + ": " + pair.second;
			chunk = curl_slist_append(chunk, requestHeader.c_str());
		}

		CURLcode res = curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, chunk);
	}

	if(_bAsync)
	{
		std::async(std::launch::async, [this, params](){
			SendEasy(params);
		});

		return;
	}

	SendEasy(params);
}

void XMLHttpRequest::SendEasy(const std::string& params)
{
	/* Perform the request, res will get the return code */ 
    CURLcode res = curl_easy_perform(_curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}

	OnReadyStateChange(_responseText);
}

void XMLHttpRequest::Abort()
{
	// TODO. Nothing.
}

void XMLHttpRequest::SetRequestHeader(const std::string& name, const std::string& value)
{
	_requestHeader[name] = value;
}

std::map<std::string, std::string> XMLHttpRequest::GetAllResponseHeaders()
{
	return _requestHeader;
}

std::string XMLHttpRequest::GetResponseHeader(const std::string& headerName)
{
	auto it = _requestHeader.find(headerName);
	if (it == _requestHeader.end())
		return "";

	return it->second;
}

#ifdef WITH_JAVASCRIPT
void XMLHttpRequest::Register(Isolate* isolate)
{
	Isolate* isolate = Isolate::GetCurrent();

	// Prepare class binding
	v8pp::class_<XMLHttpRequest> HttpRequest_class(isolate);

	// Prototype
	HttpRequest_class
		.set("open", &XMLHttpRequest::Open)
		.set("send", &XMLHttpRequest::Send)
		.set("abort", &XMLHttpRequest::Abort)

		.set("setRequestHeader", &XMLHttpRequest::SetRequestHeader)
		.set("getAllResponseHeaders", &XMLHttpRequest::GetAllResponseHeaders)
		.set("getResponseHeader", &XMLHttpRequest::GetResponseHeader)

		.set("readyState", property(&XMLHttpRequest::GetReadyState))
		.set("responseText", property(&XMLHttpRequest::GetResponseText))
		.set("status", property(&XMLHttpRequest::GetStatus))
		.set("statusText", property(&XMLHttpRequest::GetStatusText));

	v8pp::module bindings(isolate);
	bindings.set("XMLHttpRequest", HttpRequest_class);

	static Persistent<Object> bindings_(isolate, bindings.new_instance());
}
#endif

size_t XMLHttpRequest::writeCallback(char *ptr, size_t size, size_t nmemb, std::string *buffer)
{
	int result = 0;
	if (buffer != nullptr)
	{
		buffer->append(ptr, size * nmemb);
		result = size * nmemb;
	}
	return result;
}