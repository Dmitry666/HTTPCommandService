#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <module.h>

#include <include/v8.h>
#include <include/libplatform/libplatform.h>

#include <map>
#include <string>

#include <curl/curl.h>

class XMLHttpRequest
{
public:
	explicit XMLHttpRequest( const v8::FunctionCallbackInfo<v8::Value>& args );
	virtual ~XMLHttpRequest();

	void Open(const std::string& method, const std::string& url, bool bAsync);
	void Open(const std::string& method, const std::string& url, bool bAsync, const std::string& userName);
	void Open(const std::string& method, const std::string& url, bool bAsync, const std::string& userName, const std::string& password);

	void Send(const std::string& params);
	void Abort();

	void SetRequestHeader(const std::string& name, const std::string& value);
	std::map<std::string, std::string> GetAllResponseHeaders();
	std::string GetResponseHeader(const std::string& headerName);


	//
	void SetReadyState(int32_t readyState) { _readyState = readyState; }
	int32_t GetReadyState() const { return _readyState; }

	void SetResponseText(const std::string& responseText) { _responseText = responseText; }
	const std::string& GetResponseText() const { return _responseText; }

	void SetStatus(int32_t status) { _status = status; }
	int32_t GetStatus() const { return _status; }

	void SetStatusText(const std::string& statusText) { _statusText = statusText; }
	const std::string& GetStatusText() const { return _statusText; }


	static void Register(v8::Isolate* isolate);

private:
	void SendEasy(const std::string& params);
	static size_t writeCallback(char *ptr, size_t size, size_t nmemb, std::string *userdata);

protected:
	//_onReadyStateChange
	int32_t _readyState;
	std::string _responseText;
	// responseXML

	int32_t _status;
	std::string _statusText;	

	std::map<std::string, std::string> _requestHeader;

	//
	bool _bAsync;
	CURL *_curl;
	char errorBuffer[CURL_ERROR_SIZE];
};

#endif // EXAMPLECPPMODULE_H