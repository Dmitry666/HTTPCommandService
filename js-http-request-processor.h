// Copyright 2012 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifdef WITH_JAVASCRIPT
#include <include/v8.h>
#include <include/libplatform/libplatform.h>

#include <map>
#include <string>

// These interfaces represent an existing request processing interface.
// The idea is to imagine a real application that uses these interfaces
// and then add scripting capabilities that allow you to interact with
// the objects through JavaScript.

/**
 * A simplified http request.
 */
class HttpRequest 
{
public:
	virtual ~HttpRequest() 
	{}

	virtual const std::string& Path() = 0;
	virtual const std::string& Referrer() = 0;
	virtual const std::string& Host() = 0;
	virtual const std::string& UserAgent() = 0;
};


/**
 * The abstract superclass of http request processors.
 */
class HttpRequestProcessor 
{
public:
	virtual ~HttpRequestProcessor() { }

	// Initialize this processor.  The map contains options that control
	// how requests should be processed.
	virtual bool Initialize(std::map<std::string, std::string>& options,
							std::map<std::string, std::string>& output) = 0;

	// Process a single request.
	virtual bool Process(HttpRequest* req) = 0;

	static void Log(const char* event);
};



/**
 * An http request processor that is scriptable using JavaScript.
 */
class JsHttpRequestProcessor : public HttpRequestProcessor
{
public:
	// Creates a new processor that processes requests by invoking the
	// Process function of the JavaScript script given as an argument.
	JsHttpRequestProcessor(v8::Isolate* isolate, v8::Handle<v8::String> script)
		: isolate_(isolate)
		, script_(script) 
	{}

	virtual ~JsHttpRequestProcessor();

	virtual bool Initialize(std::map<std::string, std::string>& options,
							std::map<std::string, std::string>& output) override;

	virtual bool Process(HttpRequest* req) override;

private:
	// Execute the script associated with this processor and extract the
	// Process function.  Returns true if this succeeded, otherwise false.
	bool ExecuteScript(v8::Handle<v8::String> script);

	// Wrap the options and output map in a JavaScript objects and
	// install it in the global namespace as 'options' and 'output'.
	bool InstallMaps(std::map<std::string, std::string>* opts, std::map<std::string, std::string>* output);

	// Constructs the template that describes the JavaScript wrapper
	// type for requests.
	static v8::Handle<v8::ObjectTemplate> MakeRequestTemplate(v8::Isolate* isolate);
	static v8::Handle<v8::ObjectTemplate> MakeMapTemplate(v8::Isolate* isolate);

	// Callbacks that access the individual fields of request objects.
	static void GetPath(v8::Local<v8::String> name,
						const v8::PropertyCallbackInfo<v8::Value>& info);
	static void GetReferrer(v8::Local<v8::String> name,
							const v8::PropertyCallbackInfo<v8::Value>& info);
	static void GetHost(v8::Local<v8::String> name,
						const v8::PropertyCallbackInfo<v8::Value>& info);
	static void GetUserAgent(v8::Local<v8::String> name,
							const v8::PropertyCallbackInfo<v8::Value>& info);

	// Callbacks that access maps
	static void MapGet(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info);
	static void MapSet(v8::Local<v8::Name> name, v8::Local<v8::Value> value,
						const v8::PropertyCallbackInfo<v8::Value>& info);

	// Utility methods for wrapping C++ objects as JavaScript objects,
	// and going back again.
	v8::Handle<v8::Object> WrapMap(std::map<std::string, std::string>* obj);
	static std::map<std::string, std::string>* UnwrapMap(v8::Handle<v8::Object> obj);
	v8::Handle<v8::Object> WrapRequest(HttpRequest* obj);
	static HttpRequest* UnwrapRequest(v8::Handle<v8::Object> obj);

	v8::Isolate* GetIsolate() { return isolate_; }

	v8::Isolate* isolate_;
	v8::Handle<v8::String> script_;
	v8::Persistent<v8::Context> context_;
	v8::Persistent<v8::Function> process_;
	static v8::Persistent<v8::ObjectTemplate> request_template_;
	static v8::Persistent<v8::ObjectTemplate> map_template_;
};

/**
 * A simplified http request.
 */
class StringHttpRequest : public HttpRequest 
{
public:
	StringHttpRequest(
		const std::string& path,
		const std::string& referrer,
		const std::string& host,
		const std::string& user_agent)
		: path_(path)
		, referrer_(referrer)
		, host_(host)
		, user_agent_(user_agent) 
	{}

	virtual const std::string& Path() { return path_; }
	virtual const std::string& Referrer() { return referrer_; }
	virtual const std::string& Host() { return host_; }
	virtual const std::string& UserAgent() { return user_agent_; }

private:
	std::string path_;
	std::string referrer_;
	std::string host_;
	std::string user_agent_;
};
#endif