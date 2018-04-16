//#pragma once
//Contains some miscallenous constants to be used at runtime, but not to be included in the actual source.
//Declares a class that contains all those constants at once.

#include "string"
#include "vector"

struct Ref {
	std::string token="token";
	std::string ownerid= "ownerid";
	std::vector<std::string> serverlist = {"serverid", "serverid"};

};
