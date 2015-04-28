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

#include "javascriptmanager.h"
#include "directoryinfo.h"
#include <assert.h>
#include <stdio.h>

#ifdef WITH_JAVASCRIPT
#include <include/v8.h>
#include <include/libplatform/libplatform.h>

#include "javascriptcontroller.h"
#include "js-http-request-processor.h"

using namespace v8;
using namespace std;

#ifdef _MSC_VER
#pragma comment(lib,"v8_base.lib")
#pragma comment(lib,"icuuc.lib")
#pragma comment(lib,"icui18n.lib")
#pragma comment(lib,"v8_libbase.lib")
#pragma comment(lib,"v8_libplatform.lib")
#pragma comment(lib,"v8_snapshot.lib")
#pragma comment(lib,"gmock.lib")
#pragma comment(lib,"winmm.lib")
#endif
#endif


namespace http {

#ifdef WITH_JAVASCRIPT
Handle<String> ReadFile(Isolate* isolate, const string& name) 
{
	FILE* file = fopen(name.c_str(), "rb");
	if (file == NULL) 
		return Handle<String>();

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	char* chars = new char[size + 1];
	chars[size] = '\0';
	for (int i = 0; i < size;) 
	{
		int read = static_cast<int>(fread(&chars[i], 1, size - i, file));
		i += read;
	}
	fclose(file);

	Handle<String> result =
		String::NewFromUtf8(isolate, chars, String::kNormalString, size);

	delete[] chars;
	return result;
}

bool ProcessEntries(HttpRequestProcessor* processor, int count, StringHttpRequest* reqs) 
{
	for (int i = 0; i < count; i++) 
	{
		if (!processor->Process(&reqs[i]))
			return false;
	}

	return true;
}

void PrintMap(const map<string, string>& m) 
{
	for (auto& entry : m) 
	{
		printf("%s: %s\n", entry.first.c_str(), entry.second.c_str());
	}
}


const int kSampleSize = 6;
StringHttpRequest kSampleRequests[kSampleSize] = 
{
	StringHttpRequest("/process.cc", "localhost", "google.com", "firefox"),
	StringHttpRequest("/", "localhost", "google.net", "firefox"),
	StringHttpRequest("/", "localhost", "google.org", "safari"),
	StringHttpRequest("/", "localhost", "yahoo.com", "ie"),
	StringHttpRequest("/", "localhost", "yahoo.com", "safari"),
	StringHttpRequest("/", "localhost", "yahoo.com", "firefox")
};

#endif

JavascriptManager::JavascriptManager()
	//: _processor(nullptr)
{}

JavascriptManager::~JavascriptManager()
{}

void JavascriptManager::Initialize(int argc, char* argv[])
{
#ifdef WITH_JAVASCRIPT
	V8::InitializeICU();
	Platform* platform = platform::CreateDefaultPlatform();
	V8::InitializePlatform(platform);
	V8::Initialize();

#if 0
	_isolate = Isolate::New();

	{
		Isolate::Scope isolate_scope(_isolate);
		HandleScope scope(_isolate);
		Handle<String> source = ReadFile(_isolate, file);

		if (source.IsEmpty()) 
		{
			fprintf(stderr, "Error reading '%s'.\n", file.c_str());
			//return 1;
		}

		_processor = new JsHttpRequestProcessor(_isolate, source);
		if (!_processor->Initialize(&_options, &_outputs)) 
		{
			fprintf(stderr, "Error initializing processor.\n");
			//return 1;
		}
	}
#endif

	LoadScritps("./");
#endif
}

void JavascriptManager::Shutdown()
{
#ifdef WITH_JAVASCRIPT
	//delete _processor;
	//_isolate->Dispose();

	V8::Dispose();
	V8::ShutdownPlatform();
	//delete platform;
#endif
}

int32 JavascriptManager::Execute(const string& file, const map<string, string>& options, const map<string, string>& argumentsMap, map<string, string>& output)
{
#ifdef WITH_JAVASCRIPT
#if 0
	Isolate* isolate = Isolate::New();

	{
		Isolate::Scope isolate_scope(isolate);
		HandleScope scope(isolate);
		Handle<String> source = ReadFile(isolate, file);

		if (source.IsEmpty()) 
		{
			fprintf(stderr, "Error reading '%s'.\n", file.c_str());
			return 1;
		}

		JsHttpRequestProcessor processor(isolate, source);
		_options = options;
		if (!processor.Initialize(&_options, &_outputs)) 
		{
			fprintf(stderr, "Error initializing processor.\n");
			return 1;
		}

		//if (!ProcessEntries(&processor, kSampleSize, kSampleRequests))
		//	return 1;

		_outputs.clear();
		_argumentsMap = argumentsMap;
		if (!processor.Process(&_argumentsMap))
		{
			output = _outputs;
			return 1;
		}

		output = _outputs;
		PrintMap(output);
	}

	isolate->Dispose();
#endif
#endif

	return 0;
}

JavascriptManager& JavascriptManager::Instance()
{
	static JavascriptManager javascriptManager;
	return javascriptManager;
}

void JavascriptManager::LoadScritps(const std::string& folderPath)
{
    auto files = DirectoryInfo::GetFiles(folderPath);
    for (auto& file : files)
    {
        if (file.Extenstion() == "js")
        {
            const std::string& path = file.GetFullPath();

			JavascriptController* controller = new JavascriptController(file.GetShortName(), "javascript", path);
			//controller->JSRegisterMethod("Proccess", "");
			ControllerManager::RegisterController(controller);
			_controllers.push_back(controller);
        }
    }
}

} // End http.

