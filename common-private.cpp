#include "common-private.h"

#include <fstream>
#include <sstream>
#include <string>

using namespace std;

namespace openrc {

vector<string> split(const string& str, char delimiter)
{
    vector<string> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;

    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }

    while(!internal.empty() && internal.front().empty())
        internal.erase(internal.begin());

    return internal;
}

string quoted(const string& text)
{
	string out;
	//transform(text.begin(), text.end(), out.begin(), [](char c){
	//	return c == '\"' ? "\\\"" : c;
	//});
	for(const char& c : text)
	{
		if(c == '\"')
			out.append("\\\"");
		else 
			out.push_back(c);
	}

	return out;
}

} // End openrc.