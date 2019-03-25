//#pragma once
//Contains some miscallenous constants to be used at runtime, but not to be included in the actual source.
//Declares a class that contains all those constants at once.

#include <string>
#include <vector>

#define BotToken @TOKEN@
#define OwnerID @OWNERID@

struct Ref {
	std::string token = BotToken;
	std::string ownerid = OwnerID;
	std::vector<std::string> serverlist = {"432969367534305281","378235955624148995","271034455462772737"};

};
