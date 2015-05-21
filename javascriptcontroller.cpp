#include "javascriptcontroller.h"
#include "javascriptmanager.h"
#include "fileinfo.h"

#ifdef WITH_JAVASCRIPT

using namespace v8;
using namespace std;


class JSValue
{
public:
	static std::string GetString(Handle<Object>& obj, const std::string& name)
	{
		// name
		Handle<String> meta_name_name = String::NewFromUtf8(obj->GetIsolate(), name.c_str());
		Handle<Value> meta_name_val = obj->Get(meta_name_name);

		if (meta_name_val->IsString())
		{
			v8::String::Utf8Value str(meta_name_val);
			return *str;
		}

		return "";
	}

	static Handle<Object> GetObject(Handle<Object>& obj, const std::string& name)
	{
		// name
		Handle<String> meta_name_name = String::NewFromUtf8(obj->GetIsolate(), name.c_str());
		Handle<Value> meta_name_val = obj->Get(meta_name_name);

		if (meta_name_val->IsObject())
		{
			return Handle<Object>::Cast(meta_name_val);
		}

		return Handle<Object>();
	}
};

namespace openrc {

bool JavaScriptControllerMethod::Validate(class IController* obj,
                        const SessionId& sessionId,
                        const ControllerArguments& arguments)
{
	JavascriptController* class_ = static_cast<JavascriptController*>(obj);
    return class_->ValidateExecute(validator_, sessionId, arguments);
}

void JavaScriptControllerMethod::Execute(class IController* obj,
                        const SessionId& sessionId,
                        const ControllerArguments& arguments,
                        ControllerOutput& outContent)
{
    JavascriptController* class_ = static_cast<JavascriptController*>(obj);
	class_->ActionExecute(function_, sessionId, arguments, outContent);
}

bool JavascriptController::Construct()
{
	return Load();
}

bool JavascriptController::Load()
{
	isolate_ = JavascriptManager::Instance().GetIsolate();
	//isolate_ = Isolate::New();

	{
		Isolate::Scope isolate_scope(isolate_);
		HandleScope scope(isolate_);
		Handle<String> source = ReadFile(isolate_, _filename);

		if (source.IsEmpty()) 
		{
			fprintf(stderr, "Error reading '%s'.\n", _filename.c_str());
			//return 1;
		}


		script_ = source;

		InitializeScript();

#if 0
		//_processor = new JsHttpRequestProcessor(_isolate, source);
		if (!this->Initialize(&_options, &_outputs)) 
		{
			fprintf(stderr, "Error initializing processor.\n");
			//return 1;
		}

		HandleScope handle_scope(GetIsolate());
		//v8::Local<v8::Context> context =
		//	v8::Local<v8::Context>::New(GetIsolate(), context_);
		v8::Local<v8::Context> context =
			v8::Local<v8::Context>::New(GetIsolate(), JavascriptManager::Instance().GetContext());

		// Enter this processor's context so all the remaining operations
		// take place there
		Context::Scope context_scope(context);

		Handle<String> meta_name = String::NewFromUtf8(GetIsolate(), "meta");
		Handle<Value> meta_val = context->Global()->Get(meta_name);

		if (!meta_val->IsObject())
		{
			bool t = meta_val->IsUndefined();
			return false;
		}

		Handle<Object> meta_object = Handle<Object>::Cast(meta_val);

		std::string name = JSValue::GetString(meta_object, "name");
		if(!name.empty())
		{
			_name = name;
		}

		std::string description = JSValue::GetString(meta_object, "description");
		if(!description.empty())
		{
			_description = description;
		}

		std::string validator = JSValue::GetString(meta_object, "validator");
		if(!validator.empty())
		{
			//_validatorName = validator;

			// The script compiled and ran correctly.  Now we fetch out the
			// Process function from the global object.
			Handle<String> validator_name = String::NewFromUtf8(GetIsolate(), validator.c_str());
			Handle<Value> validator_val = context->Global()->Get(validator_name);

			// If there is no Process function, or if it is not a function,
			// bail out
			if (validator_val->IsFunction()) 
			{
				// It is a function; cast it to a Function
				Handle<Function> validator_fun = Handle<Function>::Cast(validator_val);

				// Store the function in a Persistent handle, since we also want
				// that to remain after this call returns
				validator_.Reset(GetIsolate(), validator_fun);
			}
			else
			{
				fprintf(stderr, "Error not found validator '%s':'%s'.\n", 
					_name.c_str(), 
					validator.c_str());
			}

		}

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
			
				Handle<Function> action_fun;
				Handle<Function> validator_fun;

				// Find
				Handle<String> function_string = String::NewFromUtf8(GetIsolate(), function_name.c_str());
				Handle<Value> function_val = context->Global()->Get(function_string);

				if (function_val->IsFunction()) 
				{
					action_fun = Handle<Function>::Cast(function_val);
				}
				else
				{
					fprintf(stderr, "Error not found action '%s':'%s'.\n", 
						_name.c_str(), 
						action_name.c_str());
				}

				// If exist validator.
				if(!action_validator.empty())
				{
					Handle<String> validator_name = String::NewFromUtf8(GetIsolate(), action_validator.c_str());
					Handle<Value> validator_val = context->Global()->Get(validator_name);

					if (validator_val->IsFunction()) 
					{
						validator_fun = Handle<Function>::Cast(validator_val);
					}
					else
					{
						fprintf(stderr, "Error not found validator '%s':'%s' from action '%s'.\n",
							_name.c_str(),
							action_validator.c_str(), 
							action_name.c_str());
					}
				}
				
				JSRegisterMethod(action_name, action_description, GetIsolate(), action_fun, validator_fun);
			}
		}
#endif
	}

	auto time = FileInfo::LastModifiedTime(_filename);
    return true;
}


void JavascriptController::UnLoad()
{
	context_.Empty();
}

bool JavascriptController::IsModified()
{
	auto time = FileInfo::LastModifiedTime(_filename);
	if (time == 0)
		return false;

	return _lastModifyTime != time;
}

bool JavascriptController::BeginAction()
{
	if(IsModified())
	{
		// Reload.
		// UnLoad();
		// Load();
	}

	return true;
}

bool JavascriptController::EndAction()
{
	return true;
}

bool JavascriptController::Validate(const SessionId& sessionId, const ControllerArguments& arguments) 
{
	Isolate::Scope isolate_scope(isolate_);
	HandleScope scope(isolate_);

	_argumentsMap = arguments.ToArgumentMap();

	// Create a handle scope to keep the temporary object references.
	HandleScope handle_scope(GetIsolate());

	v8::Local<v8::Context> context =
		v8::Local<v8::Context>::New(GetIsolate(), JavascriptManager::Instance().GetContext()); // context_);

	// Enter this processor's context so all the remaining operations
	// take place there
	Context::Scope context_scope(context);

	// Wrap the C++ request object in a JavaScript wrapper
	Handle<Object> request_obj = WrapMap(&_argumentsMap);

	// Set up an exception handler before calling the Process function
	TryCatch try_catch;

	// Invoke the process function, giving the global object as 'this'
	// and one argument, the request.
	const int argc = 1;
	Handle<Value> argv[argc] = { request_obj };
	v8::Local<v8::Function> validator =
		v8::Local<v8::Function>::New(GetIsolate(), validator_);

	Handle<Value> result = validator->Call(context->Global(), argc, argv);
	if (result.IsEmpty()) 
	{
		String::Utf8Value error(try_catch.Exception());
		HttpRequestProcessor::Log(*error);
		return false;
	} 

	if (!result->IsBoolean())
	{
		HttpRequestProcessor::Log("Validator result not boolean.");
		return false;
	}

	Handle<Boolean> validated = Handle<Boolean>::Cast(result);
	return validated->BooleanValue();
}

bool JavascriptController::ValidateExecute(
	Persistent<Function>& validate_per,
	const SessionId& sessionId,
	const ControllerArguments& arguments)
{
	Isolate::Scope isolate_scope(isolate_);
	HandleScope scope(isolate_);

	_argumentsMap = arguments.ToArgumentMap();

	// Create a handle scope to keep the temporary object references.
	HandleScope handle_scope(GetIsolate());

	v8::Local<v8::Context> context =
		v8::Local<v8::Context>::New(GetIsolate(), JavascriptManager::Instance().GetContext());

	// Enter this processor's context so all the remaining operations
	// take place there
	Context::Scope context_scope(context);

	// Wrap the C++ request object in a JavaScript wrapper
	Handle<Object> request_obj = WrapMap(&_argumentsMap);

	// Set up an exception handler before calling the Process function
	TryCatch try_catch;

	// Invoke the process function, giving the global object as 'this'
	// and one argument, the request.
	const int argc = 1;
	Handle<Value> argv[argc] = { request_obj };
	v8::Local<v8::Function> validator =
		v8::Local<v8::Function>::New(GetIsolate(), validate_per);

	Handle<Value> result = validator->Call(context->Global(), argc, argv);
	if (result.IsEmpty()) 
	{
		String::Utf8Value error(try_catch.Exception());
		HttpRequestProcessor::Log(*error);
		return false;
	} 

	if (!result->IsBoolean())
	{
		HttpRequestProcessor::Log("Validator result not boolean.");
		return false;
	}

	Handle<Boolean> validated = Handle<Boolean>::Cast(result);
	return validated->BooleanValue();
}

bool JavascriptController::ActionExecute(
	Persistent<Function>& action_per,
	const SessionId& sessionId,
	const ControllerArguments& arguments,
	ControllerOutput& outContent)
{
	Isolate::Scope isolate_scope(isolate_);
	HandleScope scope(isolate_);

	_argumentsMap = arguments.ToArgumentMap();

	// Create a handle scope to keep the temporary object references.
	HandleScope handle_scope(GetIsolate());

	v8::Local<v8::Context> context =
		v8::Local<v8::Context>::New(GetIsolate(), JavascriptManager::Instance().GetContext());

	// Enter this processor's context so all the remaining operations
	// take place there
	Context::Scope context_scope(context);

	// Wrap the C++ request object in a JavaScript wrapper
	Handle<Object> request_obj = WrapMap(&_argumentsMap);

	// Set up an exception handler before calling the Process function
	TryCatch try_catch;

	// Invoke the process function, giving the global object as 'this'
	// and one argument, the request.
	const int argc = 1;
	Handle<Value> argv[argc] = { request_obj };
	v8::Local<v8::Function> action =
		v8::Local<v8::Function>::New(GetIsolate(), action_per);

	Handle<Value> result = action->Call(context->Global(), argc, argv);
	if (result.IsEmpty()) 
	{
		String::Utf8Value error(try_catch.Exception());
		HttpRequestProcessor::Log(*error);
		return false;
	} 

	auto it = _outputs.find("body");
	if(it != _outputs.end())
	{
		outContent.append(it->second);
		return true;
	}

	outContent.append( "Result: OK\n" );

	for(auto& pair : _outputs)
	{
		outContent.append( "\t" + pair.first + ": " + pair.second + "\n" );
	}

	return true;
}

Handle<String> JavascriptController::ReadFile(Isolate* isolate, const string& name) 
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

bool JavascriptController::InitializeScript()
{
	// Create a handle scope to hold the temporary references.
	HandleScope handle_scope(GetIsolate());

	Handle<ObjectTemplate> global = ObjectTemplate::New(GetIsolate());
	global->Set(String::NewFromUtf8(GetIsolate(), "log"),
				FunctionTemplate::New(GetIsolate(), JavascriptManager::LogCallback));

	Handle<Context> context = Context::New(GetIsolate(), nullptr, global);
	context_.Reset(GetIsolate(), context);

	//Local<Context> context = Local<Context>::New(GetIsolate(), JavascriptManager::Instance().GetContext());
	Context::Scope context_scope(context);

	// Make the options mapping available within the context
	if (!InstallMaps(&_options, &_outputs))
		return false;

	// Compile and run the script
	if (!ExecuteScript(script_))
		return false;

	//HandleScope handle_scope(GetIsolate());
	//v8::Local<v8::Context> context =
	//	v8::Local<v8::Context>::New(GetIsolate(), context_);
	//v8::Local<v8::Context> context =
	//	v8::Local<v8::Context>::New(GetIsolate(), JavascriptManager::Instance().GetContext());

	// Enter this processor's context so all the remaining operations
	// take place there
	//Context::Scope context_scope(context);

	// Search meta.
	Handle<String> meta_name = String::NewFromUtf8(GetIsolate(), "meta");
	Handle<Value> meta_val = context->Global()->Get(meta_name);

	if (!meta_val->IsObject())
	{
		bool t = meta_val->IsUndefined();
		return false;
	}

	Handle<Object> meta_object = Handle<Object>::Cast(meta_val);

	std::string name = JSValue::GetString(meta_object, "name");
	if(!name.empty())
	{
		_name = name;
	}

	std::string description = JSValue::GetString(meta_object, "description");
	if(!description.empty())
	{
		_description = description;
	}

	std::string validator = JSValue::GetString(meta_object, "validator");
	if(!validator.empty())
	{
		//_validatorName = validator;

		// The script compiled and ran correctly.  Now we fetch out the
		// Process function from the global object.
		Handle<String> validator_name = String::NewFromUtf8(GetIsolate(), validator.c_str());
		Handle<Value> validator_val = context->Global()->Get(validator_name);

		// If there is no Process function, or if it is not a function,
		// bail out
		if (validator_val->IsFunction()) 
		{
			// It is a function; cast it to a Function
			Handle<Function> validator_fun = Handle<Function>::Cast(validator_val);

			// Store the function in a Persistent handle, since we also want
			// that to remain after this call returns
			validator_.Reset(GetIsolate(), validator_fun);
		}
		else
		{
			fprintf(stderr, "Error not found validator '%s':'%s'.\n", 
				_name.c_str(), 
				validator.c_str());
		}

	}

	// Actions.
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
			
			Handle<Function> action_fun;
			Handle<Function> validator_fun;

			// Find
			Handle<String> function_string = String::NewFromUtf8(GetIsolate(), function_name.c_str());
			Handle<Value> function_val = context->Global()->Get(function_string);

			if (function_val->IsFunction()) 
			{
				action_fun = Handle<Function>::Cast(function_val);
			}
			else
			{
				fprintf(stderr, "Error not found action '%s':'%s'.\n", 
					_name.c_str(), 
					action_name.c_str());
			}

			// If exist validator.
			if(!action_validator.empty())
			{
				Handle<String> validator_name = String::NewFromUtf8(GetIsolate(), action_validator.c_str());
				Handle<Value> validator_val = context->Global()->Get(validator_name);

				if (validator_val->IsFunction()) 
				{
					validator_fun = Handle<Function>::Cast(validator_val);
				}
				else
				{
					fprintf(stderr, "Error not found validator '%s':'%s' from action '%s'.\n",
						_name.c_str(),
						action_validator.c_str(), 
						action_name.c_str());
				}
			}
				
			JSRegisterMethod(action_name, action_description, GetIsolate(), action_fun, validator_fun);
		}
	}

	// Libs.
	Handle<Object> libs_object = JSValue::GetObject(meta_object, "libs");
	if(!libs_object.IsEmpty() && libs_object->IsArray())
	{
		Handle<Array> libs_names = Handle<Array>::Cast(libs_object);
		for(int i=0;i<libs_names->Length(); ++i)
		{
			Handle<Value> lib_value = libs_names->Get(i);
			Handle<String> lib_string = Handle<String>::Cast(lib_value);

			if(!lib_string.IsEmpty())
			{
				String::Utf8Value str(lib_string);
				std::string filename = *str;

				AddLibrary(filename);
			}
		}
	}


	return true;
}

bool JavascriptController::ExecuteScript(Handle<String> script) 
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
		HttpRequestProcessor::Log(*error);
		// The script failed to compile; bail out.
		return false;
	}

	// Run the script!
	Handle<Value> result = compiled_script->Run();
	if (result.IsEmpty()) 
	{
		// The TryCatch above is still in effect and will have caught the error.
		String::Utf8Value error(try_catch.Exception());
		HttpRequestProcessor::Log(*error);
		// Running the script failed; bail out.
		return false;
	}

	return true;
}

bool JavascriptController::AddLibrary(const std::string& filename)
{
	Isolate::Scope isolate_scope(isolate_);
	HandleScope scope(isolate_);
	Handle<String> source = ReadFile(isolate_, _filename);

	if (source.IsEmpty()) 
	{
		fprintf(stderr, "Error reading '%s'.\n", _filename.c_str());
		return false;
	}

	libs_.push_back(source);

	if (!ExecuteScript(source))
		return false;

	return true;
}

//
bool JavascriptController::InstallMaps(map<string, string>* opts, map<string, string>* output) 
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

Persistent<ObjectTemplate> JavascriptController::map_template_;

Handle<Object> JavascriptController::WrapMap(map<string, string>* obj) 
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
map<string, string>* JavascriptController::UnwrapMap(Handle<Object> obj) 
{
	Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<map<string, string>*>(ptr);
}

//
string JavascriptController::ObjectToString(Local<Value> value) 
{
	String::Utf8Value utf8_value(value);
	return string(*utf8_value);
}


void JavascriptController::MapGet(Local<Name> name, const PropertyCallbackInfo<Value>& info) 
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


void JavascriptController::MapSet(Local<Name> name, Local<Value> value_obj, const PropertyCallbackInfo<Value>& info) 
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

Handle<ObjectTemplate> JavascriptController::MakeMapTemplate(Isolate* isolate) 
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
