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

namespace http {

void JavascriptManager::LogCallback(const FunctionCallbackInfo<Value>& args) 
{
	  if (args.Length() < 1) 
		  return;

	  HandleScope scope(args.GetIsolate());
	  Handle<Value> arg = args[0];
	  String::Utf8Value value(arg);
	  HttpRequestProcessor::Log(*value);
}

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

/*
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
*/

JavascriptManager::JavascriptManager()
	//: _processor(nullptr)
{}

JavascriptManager::~JavascriptManager()
{}

void JavascriptManager::Initialize(const std::string& path)
{
	V8::InitializeICU();
	Platform* platform = platform::CreateDefaultPlatform();
	V8::InitializePlatform(platform);
	V8::Initialize();

	InitializeJs();

	LoadScritps("./");
	LoadScritps(path);	
}

void JavascriptManager::Shutdown()
{
	//delete _processor;
	//_isolate->Dispose();
	ShutdownJs();

	V8::Dispose();
	V8::ShutdownPlatform();
	//delete platform;
}

int32 JavascriptManager::Execute(const string& file, const map<string, string>& options, const map<string, string>& argumentsMap, map<string, string>& output)
{
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

bool JavascriptManager::InitializeJs()
{
	isolate_ = Isolate::New();
	Isolate::Scope isolate_scope(isolate_);

	// Create a handle scope to hold the temporary references.
	HandleScope handle_scope(GetIsolate());

	// Create a template for the global object where we set the
	// built-in global functions.
	Handle<ObjectTemplate> global = ObjectTemplate::New(GetIsolate());
	global->Set(String::NewFromUtf8(GetIsolate(), "log"),
				FunctionTemplate::New(GetIsolate(), LogCallback));

	// Each processor gets its own context so different processors don't
	// affect each other. Context::New returns a persistent handle which
	// is what we need for the reference to remain after we return from
	// this method. That persistent handle has to be disposed in the
	// destructor.
	Handle<Context> context = Context::New(GetIsolate(), NULL, global);
	context_.Reset(GetIsolate(), context);

	// Enter the new context so all the following operations take place
	// within it.
	Context::Scope context_scope(context);

	// Make the options mapping available within the context
	if (!InstallMaps(&_options, &_outputs))
		return false;

	return true;
}

bool JavascriptManager::ShutdownJs()
{
	context_.Empty();
	return true;
}

bool JavascriptManager::InstallMaps(map<string, string>* opts, map<string, string>* output) 
{
	HandleScope handle_scope(GetIsolate());

	// Wrap the map object in a JavaScript wrapper
	Handle<Object> opts_obj = WrapMap(opts);

	v8::Local<v8::Context> context =
		v8::Local<v8::Context>::New(GetIsolate(), context_);

	// Set the options object as a property on the global object.
	context->Global()->Set(String::NewFromUtf8(GetIsolate(), "options"),
							opts_obj);

	Handle<Object> output_obj = WrapMap(output);
	context->Global()->Set(String::NewFromUtf8(GetIsolate(), "output"),
							output_obj);

	return true;
}

Persistent<ObjectTemplate> JavascriptManager::map_template_;

Handle<Object> JavascriptManager::WrapMap(map<string, string>* obj) 
{
	// Handle scope for temporary handles.
	EscapableHandleScope handle_scope(GetIsolate());

	// Fetch the template for creating JavaScript map wrappers.
	// It only has to be created once, which we do on demand.
	if (map_template_.IsEmpty()) 
	{
		Handle<ObjectTemplate> raw_template = MakeMapTemplate(GetIsolate());
		map_template_.Reset(GetIsolate(), raw_template);
	}

	Handle<ObjectTemplate> templ =
		Local<ObjectTemplate>::New(GetIsolate(), map_template_);

	// Create an empty map wrapper.
	Local<Object> result = templ->NewInstance();

	// Wrap the raw C++ pointer in an External so it can be referenced
	// from within JavaScript.
	Handle<External> map_ptr = External::New(GetIsolate(), obj);

	// Store the map pointer in the JavaScript wrapper.
	result->SetInternalField(0, map_ptr);

	// Return the result through the current handle scope.  Since each
	// of these handles will go away when the handle scope is deleted
	// we need to call Close to let one, the result, escape into the
	// outer handle scope.
	return handle_scope.Escape(result);
}


// Utility function that extracts the C++ map pointer from a wrapper
// object.
map<string, string>* JavascriptManager::UnwrapMap(Handle<Object> obj) 
{
	Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<map<string, string>*>(ptr);
}


string JavascriptManager::ObjectToString(Local<Value> value) 
{
	String::Utf8Value utf8_value(value);
	return string(*utf8_value);
}


void JavascriptManager::MapGet(Local<Name> name,
                                    const PropertyCallbackInfo<Value>& info) 
{
	if (name->IsSymbol()) 
		return;

	// Fetch the map wrapped by this object.
	map<string, string>* obj = UnwrapMap(info.Holder());

	// Convert the JavaScript string to a std::string.
	string key = ObjectToString(Local<String>::Cast(name));

	// Look up the value if it exists using the standard STL ideom.
	map<string, string>::iterator iter = obj->find(key);

	// If the key is not present return an empty handle as signal
	if (iter == obj->end()) return;

	// Otherwise fetch the value and wrap it in a JavaScript string
	const string& value = (*iter).second;
	info.GetReturnValue().Set(String::NewFromUtf8(
		info.GetIsolate(), value.c_str(), String::kNormalString,
		static_cast<int>(value.length())));
}


void JavascriptManager::MapSet(Local<Name> name, Local<Value> value_obj,
                                    const PropertyCallbackInfo<Value>& info) 
{
	if (name->IsSymbol()) 
		return;

	// Fetch the map wrapped by this object.
	map<string, string>* obj = UnwrapMap(info.Holder());

	// Convert the key and value to std::strings.
	string key = ObjectToString(Local<String>::Cast(name));
	string value = ObjectToString(value_obj);

	// Update the map.
	(*obj)[key] = value;

	// Return the value; any non-empty handle will work.
	info.GetReturnValue().Set(value_obj);
}

Handle<ObjectTemplate> JavascriptManager::MakeMapTemplate(Isolate* isolate) 
{
	EscapableHandleScope handle_scope(isolate);

	Local<ObjectTemplate> result = ObjectTemplate::New(isolate);
	result->SetInternalFieldCount(1);
	result->SetHandler(NamedPropertyHandlerConfiguration(MapGet, MapSet));

	// Again, return the result through the current handle scope.
	return handle_scope.Escape(result);
}

} // End http.
#endif
