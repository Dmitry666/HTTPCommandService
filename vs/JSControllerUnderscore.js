var meta = {
	name: "JSControllerUnderscore",
	description: "Example javascript.",
	validator: "Validate",
	actions: {
		Process: {
			name: "Process", 
			description: "ProcessDescription",
			validator: "ProcessValidate"
		}
	},
	libs: ["libs/underscore-min.js"]
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

function httpGet(theUrl)
{
    var xmlHttp = null;

    xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "GET", theUrl, false );
    xmlHttp.send( null );
    return xmlHttp.responseText;
}

function Process(arguments) {
	var str = "";
	_.each([1, 2, 3], function(num){ str += num; });
	output["body"] = str;
}

Initialize();
