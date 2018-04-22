// Lucy_console_v1_0.cpp : Defines the entry point for the console application.
//

//Built with MSVSC17
//Copied mostly from example by yourWaifu

#include "stdafx.h"  //req by VS17
#include "sleepy_discord/websocketpp_websocket.h"
#include "Ref.h"


Ref ref; //contains token, ownerid, server IDs.
std::vector<std::vector<SleepyDiscord::Channel>> Channellist; //A vector of three(server amount) vectors containing the channel objects of the respective server.
SleepyDiscord::Snowflake<SleepyDiscord::Channel> CurrentChannel("286853054010097666");
bool Logging = false;
bool tts = false;

struct LucyServer : public SleepyDiscord::Server{
	

	int index;

};

struct LucyChannel : public SleepyDiscord::Channel{
	 		


//	LucyServer server;

};

class LucyClient : public SleepyDiscord::DiscordClient { 	//create client class for the bot, inheritance structure:

//BaseDiscordClient --> WebsocketppDiscordClient ==typedef in namespace SleepyDiscord== DiscordClient --> LucyClient
public:
	using SleepyDiscord::DiscordClient::DiscordClient; 			//sets namespace(?)
	


	void colourprint(SleepyDiscord::Message UM);

	void colourgive(SleepyDiscord::Message UM);

	void onReady(std::string* jsonMessage);
	
	void onMessage(SleepyDiscord::Message UserMessage);

	void onServer(SleepyDiscord::Server server);
	
	
	std::vector<std::vector<SleepyDiscord::Role>> RoleList;
	std::vector<std::vector<SleepyDiscord::Role>> ColourList;			//Rolelist only for Colours
	SleepyDiscord::User ColourmeClient;
	bool activeColourme=false;

private:
	int onServerCount = 0;									//Used to check if all servers have been run through
};



void LucyClient::colourprint(SleepyDiscord::Message UM) {


	//print colourlist
	std::string ColourPrint;

	for (int i = 0; i < int(ref.serverlist.size()); i++) {
		if (getChannel(UM.channelID).cast().serverID == ref.serverlist[i]) {

			ColourPrint += "``";
			for (int l = 0; l<int(ColourList[i].size()); l++) {

				ColourPrint = ColourPrint + "[" + std::to_string(l) + "] " + ColourList[i][l].name + "\\n";
			}
			ColourPrint += "`` \\n Please select a role by sending the corresponding index number.";
			//	std::cout << ColourPrint << std::endl;
			sendMessage(UM.channelID, ColourPrint);
			ColourmeClient = UM.author;
			activeColourme = true;

		}
	}

}

void LucyClient::colourgive(SleepyDiscord::Message UM) {

	std::vector<std::string> userRoles = getMember(getChannel(UM.channelID).cast().serverID, UM.author).cast().roles;
	bool hasRole = false;

	//Find server index
	for (int i = 0; i < int(ref.serverlist.size()); i++) {
		if (getChannel(UM.channelID).cast().serverID == ref.serverlist[i]) {


			try {

				//Check if user has role by iterating over all roles and comparing
				for (int k = 0; k<int(userRoles.size()); k++) {
					if (ColourList[i][std::stoi(UM.content)].ID == userRoles[k]) {
						hasRole = true;
						break;
					}
				}



				if (hasRole) {			//If user has the role, remove it
					removeRole(getChannel(UM.channelID).cast().serverID, UM.author, ColourList[i][std::stoi(UM.content)]);
					sendMessage(UM.channelID, "I have removed the " + ColourList[i][std::stoi(UM.content)].name + " Role from you.");
				}
				else {					//Otherwise add it
					addRole(getChannel(UM.channelID).cast().serverID, UM.author, ColourList[i][std::stoi(UM.content)]);
					sendMessage(UM.channelID, "I have given you the " + ColourList[i][std::stoi(UM.content)].name + " Role.");
				}

				activeColourme = !activeColourme;

			}
			catch (std::invalid_argument err) {
				std::cerr << "STD exception in colourgive: " << err.what() << std::endl;
				sendMessage(UM.channelID, "I said enter a FUCKING NUMBER, dimwit");
			}



		}
	}


}

void LucyClient::onReady(std::string* jsonMessage) {
	std::cout << "onReady was called." << std::endl;

	for (int i = 0; i<int(ref.serverlist.size()); i++) {							//go through each server
		if (!(this->getServer(ref.serverlist[i]).cast().unavailable)) {		//check if theyre available
			std::cout << "Server " << i << " is available" << std::endl;
		}
	}
}

void LucyClient::onMessage(SleepyDiscord::Message UserMessage) { 		//redefines OnMessage event from client.h
	if (!(UserMessage.author == getCurrentUser().cast())) {

		if (Logging) {
			std::cout << UserMessage.author.username << " said| " << UserMessage.content << " |at " << UserMessage.timestamp << "." << std::endl;
		}

		if (UserMessage.startsWith("-hello")) {				//if you type whcg hello, the bot greets you.
			sendMessage(UserMessage.channelID, "Hello " + UserMessage.author.username);
		}

		if (UserMessage.startsWith("Lucy, initiate shutdown") && UserMessage.author.ID == ref.ownerid) {				//if you type "Lucy, initiate shutdown", she shuts down.
			sendMessage(UserMessage.channelID, "Gnight!");
			quit();
		}

		if (UserMessage.startsWith("-ping")) {				//ping-pong, implement ping time later.
			sendMessage(UserMessage.channelID, "pong \\n You sent this at " + UserMessage.timestamp);
			//this->sendHeartbeat();

		}

		if (UserMessage.startsWith("-speak")) {				//Annoying tts command.
			sendMessage(UserMessage.channelID, "hi domo virtual Youtuber Kisuna i des", 1);
		}

		if (UserMessage.startsWith("-help")) {				//Help command
			sendMessage(UserMessage.channelID, "-hello: Makes me greet you \\n -ping: Makes me pong you and show you when you sent your ping \\n -speak: Makes me show you the true meaning of fear, as long as you have tts enabled");
		}

		if (UserMessage.isMentioned(getCurrentUser().cast())) {
			sendMessage(UserMessage.channelID, "Hi there~ \\n I hope you have a reason for pinging me though =v=\\n If you need more info on me, try the -help command ;p", 1);
		}

		if ((UserMessage.startsWith("-colourme")) && (!activeColourme)) {
			schedule([this, UserMessage]() {
				if (activeColourme && (ColourmeClient == UserMessage.author)) {
					activeColourme = !activeColourme;
					sendMessage(UserMessage.channelID, "Aborting Colourme due to timeout.");
				}
			}, 30000); //Look up what a lambda body/capture list is

			colourprint(UserMessage);
		}

		if ((UserMessage.author == ColourmeClient) && (!(UserMessage.content == "-colourme")) && (activeColourme)) {

			try {
				colourgive(UserMessage);
			}
			catch (std::runtime_error err) {
				std::cerr << err.what() << std::endl;
			}


		}

		/*if (UserMessage.startsWith("-ChannelName")) {
		editChannelName(UserMessage.channelID,);
		}*/
	}


}

void LucyClient::onServer(SleepyDiscord::Server server) {				//OwO when a server notices you
	std::cout << "onServer was called." << std::endl;

	onServerCount++;

	if (!(RoleList.size() == int(ref.serverlist.size()))) {
		RoleList.resize(int(ref.serverlist.size()));
	}
	if (!(ColourList.size() == int(ref.serverlist.size()))) {
		ColourList.resize(int(ref.serverlist.size()));
	}

	/*for (int i = 0; i < int(ref.serverlist.size()); i++) {
	if (server.ID == ref.serverlist[i]) {}  }*/

	for (int i = 0; i < int(ref.serverlist.size()); i++) {	//For each server

		if (server.ID == ref.serverlist[i]) {				//Check if the server is one of the ones on the list


			Channellist[i].resize(server.channels.size());
			Channellist[i] = server.channels;				//Update the channellist for the server
			RoleList[i] = getRoles(server).vector();			//Update the Rolelist for the server

		}
	}



	//Finalising operations: Create colour list
	if (onServerCount == int(ref.serverlist.size())) {

		std::cout << "Preparing some stuff now that all servers are here..." << std::endl;

		for (int i = 0; i < int(ref.serverlist.size()); i++) { //Go through all servers

			for (int k = 0; k < int(RoleList[i].size()); k++) {

				if (RoleList[i][k].name.substr(0, 3) == "[C]") { //get first three bytes of rolename, then ask if they are [C].
																 //std::cout << RoleList[i][k].name << " is a [C] role" << std::endl;
					ColourList[i].push_back(RoleList[i][k]);		//Add the role to the temp rolelist

				}
			}
			std::cout << "Server " << i << " has " << ColourList[i].size() << " colour roles." << std::endl;
		}

	}

}


void ChannelSwitch(SleepyDiscord::Snowflake<SleepyDiscord::Channel>& CC, LucyClient& L) {												  
	
	
	//Print Serverlist enumerated
	for (int i = 0; i < int(Channellist.size()); i++) {
			std::cout << "[" << i << "] " << L.getServer(ref.serverlist[i]).cast().name << std::endl;
	}

	//Ask for Server enum
	for (int u; std::cout << "Enter Server number : " && std::cin >> u; ) {
		std::cout << "The server has been switched to " << L.getServer(ref.serverlist[u]).cast().name << std::endl;
		
		//Print Channellist enumerated
		for (int j = 0; j < int(Channellist[u].size()); j++) {
			std::cout << "[" << u << "] " << "[" << j << "] " << Channellist[u][j].name << std::endl;
		}
		//ask for channel enum on server
		for (int v; std::cout << "Enter Channel number : " && std::cin >> v; ) {

			CC = Channellist[u][v];		//Set CurrentChannel to the selected one
			std::cout << "The channel has been switched to " << Channellist[u][v].name << std::endl;
			break;
		}
		break;
	}

	std::cout << "Finishing Channel switch.\n";


}

void CMD(std::string const & line,LucyClient& EL){
	
	if (line == "!quit") {
		EL.quit();
	}

	else if (line == "!cs") {
		ChannelSwitch(CurrentChannel, EL);
	}

	else if (line == "!cc") {
		std::cout << "The current channel is " << EL.getChannel(CurrentChannel).cast().name << ", ID " << std::string(CurrentChannel) << std::endl;
	}
	else if (line == "!tl") {
		Logging = !Logging;
	}
	else if (line == "!toggle_tts") {
		tts = !tts;
	}
	else if (line.substr(0,5) == "!nick") {
		EL.editNickname(EL.getChannel(CurrentChannel).cast().serverID, line.substr(7, int(line.length())));
	}
	else{
		EL.sendMessage(CurrentChannel, line,tts);
	}

}


int main() {
	
	Channellist.resize(int(ref.serverlist.size()));										//Put the channellist to proper size
	std::cout <<"There are currently "<< Channellist.size() <<" servers."<< std::endl;	//Output server amount and channellist size
	
	
	LucyClient Lucy(ref.token, 3);														 //threads normally at 2, at 3 it autocalls run on a seperate thread, allowing main to continue
	//Lucy.run();																	//	dont use if threads=3
	

    for (std::string line; std::cout << "Lucy > " && std::getline(std::cin, line); )
    {
        if (!line.empty()) { CMD(line, Lucy); }
    }

	


    std::cout << "Goodbye.\n";

	
}
