var meta = {
	name: "JSControllerExample",
	description: "Example javascript.",
	validator: "Validate",
	actions: {
		Process: {
			name: "Process", 
			description: "ProcessDescription",
			validator: "ProcessValidate"
		}
	}
};

function Initialize() { }

function Validate() {
	log("Controller Validate");
	return true;
}

function ProcessValidate() {
	log("Controller ProcessValidate");
	return true;
}

function Process(arguments) {

	
  //if (options.verbose) {
  //  log("Processing " + request.host + request.path +
  //      " from " + request.referrer + "@" + request.userAgent);
  //}
  //if (!output[request.host]) {
  //  output[request.host] = 1;
  //} else {
  //  output[request.host]++
  //}
  
	var keys = Object.keys(arguments);
	output["keys"] = keys.toString();
	for (key in keys) {
		log(key);
		output[key] = arguments[key]; 
	}
	
	output["key"] = arguments.key;
	output["arguments"] = arguments.toString();
	//arguments.forEach(logMapElements);
	output["body"] = "Ok.";
}

Initialize();
