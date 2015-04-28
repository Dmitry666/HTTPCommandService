#include "js-http-request-processor.h"

#ifdef WITH_JAVASCRIPT
using namespace std;
using namespace v8;


// -------------------------
// --- P r o c e s s o r ---
// -------------------------

static void LogCallback(const FunctionCallbackInfo<Value>& args) 
{
	  if (args.Length() < 1) 
		  return;

	  HandleScope scope(args.GetIsolate());
	  Handle<Value> arg = args[0];
	  String::Utf8Value value(arg);
	  HttpRequestProcessor::Log(*value);
}


// Execute the script and fetch the Process method.
bool JsHttpRequestProcessor::Initialize(map<string, string>* opts, map<string, string>* output) 
{
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
	if (!InstallMaps(opts, output))
		return false;

	// Compile and run the script
	if (!ExecuteScript(script_))
		return false;

	// The script compiled and ran correctly.  Now we fetch out the
	// Process function from the global object.
	Handle<String> process_name = String::NewFromUtf8(GetIsolate(), "Process");
	Handle<Value> process_val = context->Global()->Get(process_name);

	// If there is no Process function, or if it is not a function,
	// bail out
	if (!process_val->IsFunction()) 
		return false;

	// It is a function; cast it to a Function
	Handle<Function> process_fun = Handle<Function>::Cast(process_val);

	// Store the function in a Persistent handle, since we also want
	// that to remain after this call returns
	process_.Reset(GetIsolate(), process_fun);


#if 0
	// name
	Handle<String> meta_name = String::NewFromUtf8(GetIsolate(), "meta");
	Handle<Value> meta_val = context->Global()->Get(meta_name);

	if (!meta_val->IsObject())
		return false;

	Handle<Object> meta_object = Handle<Object>::Cast(meta_val);


	std::string name = JSValue::GetString(meta_object, "name");
	std::string description = JSValue::GetString(meta_object, "description");
	std::string validator = JSValue::GetString(meta_object, "validator");

	Handle<Object> actions_object = JSValue::GetObject(meta_object, "actions");

	Handle<Array> properties_names = actions_object->GetPropertyNames();
	for(int i=0;i<properties_names->Length(); ++i)
	{
		Handle<Value> name_val = properties_names->Get(i);
		Handle<Value> action_val = actions_object->Get(name_val);
		
		if (action_val->IsObject())
		{
			Handle<Object> action_object = Handle<Object>::Cast(action_val);

			String::Utf8Value str(name_val);
			std::string function_name = *str;

			std::string action_name = JSValue::GetString(action_object, "name");
			std::string action_description = JSValue::GetString(action_object, "description");
			std::string action_validator = JSValue::GetString(action_object, "validator");
		}
	}
#endif

	// All done; all went well
	return true;
}

#if 0
Handle<Object> JsHttpRequestProcessor::GetObject(const std::string& name)
{
	HandleScope handle_scope(GetIsolate());
	v8::Local<v8::Context> context =
		v8::Local<v8::Context>::New(GetIsolate(), context_);

	// Enter this processor's context so all the remaining operations
	// take place there
	Context::Scope context_scope(context);

	Handle<String> meta_name = String::NewFromUtf8(GetIsolate(), name.c_str());
	Handle<Value> meta_val = context->Global()->Get(meta_name);

	if (!meta_val->IsObject())
		return Handle<Object>();

	return Handle<Object>::Cast(meta_val);
}
#endif

bool JsHttpRequestProcessor::ExecuteScript(Handle<String> script) 
{
	HandleScope handle_scope(GetIsolate());

	// We're just about to compile the script; set up an error handler to
	// catch any exceptions the script might throw.
	TryCatch try_catch;

	// Compile the script and check for errors.
	Handle<Script> compiled_script = Script::Compile(script);
	if (compiled_script.IsEmpty()) 
	{
		String::Utf8Value error(try_catch.Exception());
		Log(*error);
		// The script failed to compile; bail out.
		return false;
	}

	// Run the script!
	Handle<Value> result = compiled_script->Run();
	if (result.IsEmpty()) 
	{
		// The TryCatch above is still in effect and will have caught the error.
		String::Utf8Value error(try_catch.Exception());
		Log(*error);
		// Running the script failed; bail out.
		return false;
	}

	return true;
}


bool JsHttpRequestProcessor::InstallMaps(map<string, string>* opts, map<string, string>* output) 
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


bool JsHttpRequestProcessor::Process(HttpRequest* request) 
{
	// Create a handle scope to keep the temporary object references.
	HandleScope handle_scope(GetIsolate());

	v8::Local<v8::Context> context =
		v8::Local<v8::Context>::New(GetIsolate(), context_);

	// Enter this processor's context so all the remaining operations
	// take place there
	Context::Scope context_scope(context);

	// Wrap the C++ request object in a JavaScript wrapper
	Handle<Object> request_obj = WrapRequest(request);

	// Set up an exception handler before calling the Process function
	TryCatch try_catch;

	// Invoke the process function, giving the global object as 'this'
	// and one argument, the request.
	const int argc = 1;
	Handle<Value> argv[argc] = { request_obj };
	v8::Local<v8::Function> process =
		v8::Local<v8::Function>::New(GetIsolate(), process_);

	Handle<Value> result = process->Call(context->Global(), argc, argv);
	if (result.IsEmpty()) 
	{
		String::Utf8Value error(try_catch.Exception());
		Log(*error);
		return false;
	} 
	else 
	{
		return true;
	}
}

bool JsHttpRequestProcessor::Process(std::map<std::string, std::string>* arguments)
{
	// Create a handle scope to keep the temporary object references.
	HandleScope handle_scope(GetIsolate());

	v8::Local<v8::Context> context =
		v8::Local<v8::Context>::New(GetIsolate(), context_);

	// Enter this processor's context so all the remaining operations
	// take place there
	Context::Scope context_scope(context);

	// Wrap the C++ request object in a JavaScript wrapper
	Handle<Object> request_obj = WrapMap(arguments);

	// Set up an exception handler before calling the Process function
	TryCatch try_catch;

	// Invoke the process function, giving the global object as 'this'
	// and one argument, the request.
	const int argc = 1;
	Handle<Value> argv[argc] = { request_obj };
	v8::Local<v8::Function> process =
		v8::Local<v8::Function>::New(GetIsolate(), process_);

	Handle<Value> result = process->Call(context->Global(), argc, argv);
	if (result.IsEmpty()) 
	{
		String::Utf8Value error(try_catch.Exception());
		Log(*error);
		return false;
	} 
	else 
	{
		return true;
	}
}

JsHttpRequestProcessor::~JsHttpRequestProcessor() 
{
	// Dispose the persistent handles.  When noone else has any
	// references to the objects stored in the handles they will be
	// automatically reclaimed.
	context_.Reset();
	process_.Reset();
}


Persistent<ObjectTemplate> JsHttpRequestProcessor::request_template_;
Persistent<ObjectTemplate> JsHttpRequestProcessor::map_template_;


// -----------------------------------
// --- A c c e s s i n g   M a p s ---
// -----------------------------------

// Utility function that wraps a C++ http request object in a
// JavaScript object.
Handle<Object> JsHttpRequestProcessor::WrapMap(map<string, string>* obj) 
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
map<string, string>* JsHttpRequestProcessor::UnwrapMap(Handle<Object> obj) 
{
	Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<map<string, string>*>(ptr);
}


// Convert a JavaScript string to a std::string.  To not bother too
// much with string encodings we just use ascii.
string ObjectToString(Local<Value> value) 
{
	String::Utf8Value utf8_value(value);
	return string(*utf8_value);
}


void JsHttpRequestProcessor::MapGet(Local<Name> name,
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


void JsHttpRequestProcessor::MapSet(Local<Name> name, Local<Value> value_obj,
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


Handle<ObjectTemplate> JsHttpRequestProcessor::MakeMapTemplate(Isolate* isolate) 
{
	EscapableHandleScope handle_scope(isolate);

	Local<ObjectTemplate> result = ObjectTemplate::New(isolate);
	result->SetInternalFieldCount(1);
	result->SetHandler(NamedPropertyHandlerConfiguration(MapGet, MapSet));

	// Again, return the result through the current handle scope.
	return handle_scope.Escape(result);
}


// -------------------------------------------
// --- A c c e s s i n g   R e q u e s t s ---
// -------------------------------------------

/**
 * Utility function that wraps a C++ http request object in a
 * JavaScript object.
 */
Handle<Object> JsHttpRequestProcessor::WrapRequest(HttpRequest* request) 
{
	// Handle scope for temporary handles.
	EscapableHandleScope handle_scope(GetIsolate());

	// Fetch the template for creating JavaScript http request wrappers.
	// It only has to be created once, which we do on demand.
	if (request_template_.IsEmpty()) 
	{
		Handle<ObjectTemplate> raw_template = MakeRequestTemplate(GetIsolate());
		request_template_.Reset(GetIsolate(), raw_template);
	}

	Handle<ObjectTemplate> templ =
		Local<ObjectTemplate>::New(GetIsolate(), request_template_);

	// Create an empty http request wrapper.
	Local<Object> result = templ->NewInstance();

	// Wrap the raw C++ pointer in an External so it can be referenced
	// from within JavaScript.
	Handle<External> request_ptr = External::New(GetIsolate(), request);

	// Store the request pointer in the JavaScript wrapper.
	result->SetInternalField(0, request_ptr);

	// Return the result through the current handle scope.  Since each
	// of these handles will go away when the handle scope is deleted
	// we need to call Close to let one, the result, escape into the
	// outer handle scope.
	return handle_scope.Escape(result);
}


/**
 * Utility function that extracts the C++ http request object from a
 * wrapper object.
 */
HttpRequest* JsHttpRequestProcessor::UnwrapRequest(Handle<Object> obj) 
{
	Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<HttpRequest*>(ptr);
}


void JsHttpRequestProcessor::GetPath(Local<String> name,
                                     const PropertyCallbackInfo<Value>& info) 
{
	// Extract the C++ request object from the JavaScript wrapper.
	HttpRequest* request = UnwrapRequest(info.Holder());

	// Fetch the path.
	const string& path = request->Path();

	// Wrap the result in a JavaScript string and return it.
	info.GetReturnValue().Set(String::NewFromUtf8(
		info.GetIsolate(), path.c_str(), String::kNormalString,
		static_cast<int>(path.length())));
}


void JsHttpRequestProcessor::GetReferrer(
    Local<String> name,
    const PropertyCallbackInfo<Value>& info) 
{
	HttpRequest* request = UnwrapRequest(info.Holder());
	const string& path = request->Referrer();
	info.GetReturnValue().Set(String::NewFromUtf8(
		info.GetIsolate(), path.c_str(), String::kNormalString,
		static_cast<int>(path.length())));
}


void JsHttpRequestProcessor::GetHost(Local<String> name,
                                     const PropertyCallbackInfo<Value>& info) 
{
	HttpRequest* request = UnwrapRequest(info.Holder());
	const string& path = request->Host();
	info.GetReturnValue().Set(String::NewFromUtf8(
		info.GetIsolate(), path.c_str(), String::kNormalString,
		static_cast<int>(path.length())));
}


void JsHttpRequestProcessor::GetUserAgent(
    Local<String> name,
    const PropertyCallbackInfo<Value>& info) 
{
	HttpRequest* request = UnwrapRequest(info.Holder());
	const string& path = request->UserAgent();
	info.GetReturnValue().Set(String::NewFromUtf8(
		info.GetIsolate(), path.c_str(), String::kNormalString,
		static_cast<int>(path.length())));
}


Handle<ObjectTemplate> JsHttpRequestProcessor::MakeRequestTemplate(Isolate* isolate) 
{
	EscapableHandleScope handle_scope(isolate);

	Local<ObjectTemplate> result = ObjectTemplate::New(isolate);
	result->SetInternalFieldCount(1);

	// Add accessors for each of the fields of the request.
	result->SetAccessor(
		String::NewFromUtf8(isolate, "path", String::kInternalizedString),
		GetPath);
	result->SetAccessor(
		String::NewFromUtf8(isolate, "referrer", String::kInternalizedString),
		GetReferrer);
	result->SetAccessor(
		String::NewFromUtf8(isolate, "host", String::kInternalizedString),
		GetHost);
	result->SetAccessor(
		String::NewFromUtf8(isolate, "userAgent", String::kInternalizedString),
		GetUserAgent);

	// Again, return the result through the current handle scope.
	return handle_scope.Escape(result);
}


// --- Test ---


void HttpRequestProcessor::Log(const char* event) 
{
	printf("Logged: %s\n", event);
}

#endif