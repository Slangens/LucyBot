// LucyBot.cpp : Defines the entry point for the console application.
//
//Built with MSVSC17

#include "stdafx.h"  //req by VS17
#include "sleepy_discord/websocketpp_websocket.h"
#include "sleepy_discord/voice_connection.h"
#include "Ref.h"
#include <random>

Ref reference; //contains token, ownerid, server IDs.


struct LucyServer  {
public:
	LucyServer() {}
	LucyServer(SleepyDiscord::Server Input, int k, std::vector<SleepyDiscord::Channel> CHList, std::vector<SleepyDiscord::Role> RLList) 
		:server(Input), index(k), ChannelList(CHList), RoleList(RLList) {

		std::cout << "Server " << index << " has name " << server.name << "." << std::endl;
		std::cout << "Server " << index << " has index " << index << "." << std::endl;
		std::cout << "Server " << index << " has " << ChannelList.size() << " Channels." << std::endl;
		std::cout << "Server " << index << " has " << RoleList.size() << " Roles." << std::endl;

		//Create Colour list from Role list
		for (int l = 0; l < int(RoleList.size()); l++) {

			if (RoleList[l].name.substr(0, 3) == "[C]") { //get first three bytes of rolename, then ask if they are [C].
															
				ColourList.push_back(RoleList[l]);		//Add the role to the Colour List
				
			}
		}
		std::cout << "Server " << index << " has " << ColourList.size() << " colour roles." << std::endl;
	}

	SleepyDiscord::Server server;
	int index;
	std::vector<SleepyDiscord::Channel> ChannelList;
	std::vector<SleepyDiscord::Role> RoleList;
	std::vector<SleepyDiscord::Role> ColourList;
	

	
};

class LucyClient : public SleepyDiscord::DiscordClient { 	//create client class for the bot, inheritance structure:

															//BaseDiscordClient --> WebsocketppDiscordClient ==typedef in namespace SleepyDiscord== DiscordClient --> LucyClient
public:
	using SleepyDiscord::DiscordClient::DiscordClient; 			//sets namespace(?)

	void colourprint(SleepyDiscord::Message UM);
	void colourgive(SleepyDiscord::Message UM);
	void onReady(SleepyDiscord::Ready readyData);
	void onMessage(SleepyDiscord::Message UserMessage);
	void onServer(SleepyDiscord::Server server);
	void onError(SleepyDiscord::ErrorCode errorCode, const std::string errorMessage);
	int ServerIndex(SleepyDiscord::Snowflake<SleepyDiscord::Server> ServerID);

	//Voice handlers and contexts
	SleepyDiscord::BaseVoiceEventHandler VCEvent;
	SleepyDiscord::VoiceContext VCon = createVoiceContext(getChannel("286853054010097666").cast().ID, getChannel("286853054010097666").cast().serverID, &(VCEvent));
	
	std::vector<LucyServer> Serverlist;
	bool Logging = false;
	bool tts = false;
	SleepyDiscord::Channel CurrentChannel=getChannel("286853054010097666");
	int ServerCount = 0;									//Used to check if all servers have been run through
	SleepyDiscord::Channel OwnerDM;

private:
	SleepyDiscord::User ColourmeClient;
	bool activeColourme = false;
	bool MomJokeCooldown = false;
};



int LucyClient::ServerIndex(SleepyDiscord::Snowflake<SleepyDiscord::Server> ServerID) {
	int Ind;
	for (int k = 0; k<int(Serverlist.size()); k++) {
		if (Serverlist[k].server.ID == ServerID) {
			Ind = k;
			break;
		}
	}
	return Ind;
}

void LucyClient::colourprint(SleepyDiscord::Message UM) {

	std::string ColourPrint;
	int i = ServerIndex(getChannel(UM.channelID).cast().serverID);

	ColourPrint += "``";
	for (int l = 0; l<int(Serverlist[i].ColourList.size()); l++) {

		ColourPrint = ColourPrint + "[" + std::to_string(l) + "] " + Serverlist[i].ColourList[l].name + "\\n";
	}
	ColourPrint += "`` \\n Please select a role by sending the corresponding index number.";

	sendMessage(UM.channelID, ColourPrint);
	ColourmeClient = UM.author;
	activeColourme = true;
	

}

void LucyClient::colourgive(SleepyDiscord::Message UM) {

	SleepyDiscord::Snowflake<SleepyDiscord::Server> A = getChannel(UM.channelID).cast().serverID;
	std::vector<std::string> userRoles = getMember(A, UM.author).cast().roles;
	bool hasRole = false;
	int i = ServerIndex(A),RoleIndex;
	
		try {
				RoleIndex = std::stoi(UM.content);

			if ((RoleIndex >= int(Serverlist[i].ColourList.size())) || (RoleIndex < 0 )) {
				sendMessage(UM.channelID, "Please enter a valid index next time.");
			}
			else {


				SleepyDiscord::Role ChosenRole = Serverlist[i].ColourList[std::stoi(UM.content)];

				//Check if user has role by iterating over all roles and comparing
				for (int k = 0; k<int(userRoles.size()); k++) {
					if (ChosenRole.ID == userRoles[k]) {
						hasRole = true;
						break;
					}
				}



				if (hasRole) {			//If user has the role, remove it
					removeRole(A, UM.author, ChosenRole);
					sendMessage(UM.channelID, "I have removed the " + ChosenRole.name + " Role from you.");
				}
				else {					//Otherwise add it
					addRole(A, UM.author, ChosenRole);
					sendMessage(UM.channelID, "I have given you the " + ChosenRole.name + " Role.");
				}

				activeColourme = !activeColourme;

			}
	
		}catch (std::invalid_argument err) {
				std::cerr << "STD exception in colourgive: " << err.what() << std::endl;
				sendMessage(UM.channelID, "I said enter a FUCKING NUMBER, dimwit");
		}
		
	}

void LucyClient::onReady(SleepyDiscord::Ready readyData) {
	std::cout << "onReady was called." << std::endl;
	//std::cout << std::string(this->getID()) << std::endl;
	//OwnerDM = this->createDirectMessageChannel(reference.ownerid).cast();
}

void LucyClient::onMessage(SleepyDiscord::Message UserMessage) { 		//redefines OnMessage event from client.h
	if (!(UserMessage.author == getCurrentUser().cast())) {

		if (this->Logging) {
			std::cout << UserMessage.author.username << " said| " << UserMessage.content << " |at " << UserMessage.timestamp << "." << std::endl;
		}

		if (UserMessage.startsWith("-hello")) {				//if you type whcg hello, the bot greets you.
			sendMessage(UserMessage.channelID, "Hello " + UserMessage.author.username);
		}

		/*if (UserMessage.startsWith("Lucy, initiate shutdown") && UserMessage.author.ID == reference.ownerid) {				//if you type "Lucy, initiate shutdown", she shuts down.
			sendMessage(UserMessage.channelID, "Gnight!");
			quit();
		}*/

		if (UserMessage.startsWith("-ping")) {				//ping-pong, implement ping time later.
			sendMessage(UserMessage.channelID, "pong \\n You sent this at " + UserMessage.timestamp);
			//this->sendHeartbeat();

		}

		if (UserMessage.startsWith("-speak")) {				//Annoying tts command.
			sendMessage(UserMessage.channelID, "hi domo virtual Youtuber Kisuna i des", 1);
		}

		if (UserMessage.startsWith("-help")) {				//Help command
			sendMessage(UserMessage.channelID, "-hello: Makes me greet you \\n -ping: Makes me pong you and show you when you sent your ping \\n -speak: Makes me show you the true meaning of fear, as long as you have tts enabled \\n -colourme: Starts a dialogue that makes me assign a [C] role to you. Use WITHOUT arguments, instructions will follow. ");
		}

		/*if (( (UserMessage.startsWith("I'm")) || (UserMessage.startsWith("i'm"))) && (!MomJokeCooldown) && !(UserMessage.channelID == "296130442493689857")) {
			MomJokeCooldown = true;
			schedule([this]() {
				MomJokeCooldown = false;
			}, 300000);

			if (UserMessage.content.substr(3, 1) == " ") {
				sendMessage(UserMessage.channelID, "hi " + UserMessage.content.substr(4, int(UserMessage.content.length())-4) + ", I'm mom");
			}
			else {
				sendMessage(UserMessage.channelID, "hi " + UserMessage.content.substr(3, int(UserMessage.content.length())-3) + ", I'm mom");
			}

			

		}

		if (((UserMessage.startsWith("im")) || (UserMessage.startsWith("Im")) ) && (!MomJokeCooldown) && !(UserMessage.channelID == "296130442493689857")) {
			MomJokeCooldown = true;
			schedule([this]() {
				MomJokeCooldown = false;
			}, 300000);

			
			if (UserMessage.content.substr(2,1) == " ") {
				sendMessage(UserMessage.channelID, "hi " + UserMessage.content.substr(3, int(UserMessage.content.length())-3) + ", I'm mom");
			}
			else {
				sendMessage(UserMessage.channelID, "hi " + UserMessage.content.substr(2, int(UserMessage.content.length())-2) + ", I'm mom");
			}
		}*/


		if (UserMessage.isMentioned(reference.ownerid)) {
			schedule([this, UserMessage]() {
				if (!(UserMessage.author.ID == reference.ownerid )) {
					sendMessage(UserMessage.channelID, "Want me to relay something to him?", 0);
					//sendMessage(OwnerDM,UserMessage.author.username + " pinged you in " + getChannel(UserMessage.channelID).cast().name, 0);
				}
			}, 5000);
			
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

		//Blurple 426766430986436608
		/*if ((UserMessage.startsWith("-Blurple")) || (UserMessage.startsWith("-blurple"))) {

			SleepyDiscord::Role Blurple;

			try{
			
				//426766430986436608
						int I =ServerIndex("271034455462772737");
						//std::cout << Serverlist[I].server.name << " found." << std::endl;
						//std::cout << int(Serverlist[I].RoleList.size()) << std::endl;
						for (int j = 0; j < int(Serverlist[I].RoleList.size()); j++) {
							//std::cout << Serverlist[I].RoleList[j].name << " | " << std::string(Serverlist[I].RoleList[j].ID) << std::endl;
							if (Serverlist[I].RoleList[j].name == "Blurple Birthday") {
								//std::cout << "Blurple found" << std::endl;
								Blurple = Serverlist[I].RoleList[j];
							}
						}

			}catch(std::runtime_error err){
				std::cerr << err.what() << std::endl;
			}
			//std::cout << std::string(Blurple.ID) << std::endl;
			addRole(getChannel(UserMessage.channelID).cast().serverID ,UserMessage.author.ID, Blurple.ID);
				
			sendMessage(UserMessage.channelID , "This is my special attack. You're Blurple now.");



		}

		if ((UserMessage.startsWith("-BlurpleRemove")) || (UserMessage.startsWith("-blurpleRemove"))) {

			SleepyDiscord::Role Blurple;

			try {

				//426766430986436608
				int I = ServerIndex("271034455462772737");
				//std::cout << Serverlist[I].server.name << " found." << std::endl;
				//std::cout << int(Serverlist[I].RoleList.size()) << std::endl;
				for (int j = 0; j < int(Serverlist[I].RoleList.size()); j++) {
					//std::cout << Serverlist[I].RoleList[j].name << " | " << std::string(Serverlist[I].RoleList[j].ID) << std::endl;
					if (Serverlist[I].RoleList[j].name == "Blurple Birthday") {
						//std::cout << "Blurple found" << std::endl;
						Blurple = Serverlist[I].RoleList[j];
					}
				}

			}
			catch (std::runtime_error err) {
				std::cerr << err.what() << std::endl;
			}
			//std::cout << std::string(Blurple.ID) << std::endl;
			removeRole(getChannel(UserMessage.channelID).cast().serverID, UserMessage.author.ID, Blurple.ID);

			sendMessage(UserMessage.channelID, "This is my STAND, 「O V E R  T H E  R A I N B O W」. You're no longer Blurple.");



		}*/

	/*	if (UserMessage.startsWith("-THANOS") && (UserMessage.author.ID == reference.ownerid)) {
			int I = ServerIndex("271034455462772737");
			Serverlist[I].server;
		}*/

		if (UserMessage.startsWith("-ChannelName")) {
			editChannelName(UserMessage.channelID, UserMessage.content.substr(13, UserMessage.content.length() - 13));
		}
	}

}

void LucyClient::onServer(SleepyDiscord::Server server) {				//OwO when a server notices you
	std::cout << "onServer was called." << std::endl;

	ServerCount++;

	Serverlist.push_back(
		LucyServer{ 
			server, 
			int(Serverlist.size()), 
			server.channels, 
			getRoles(server).vector() } 
	);
}

void LucyClient::onError(SleepyDiscord::ErrorCode errorCode, const std::string errorMessage) {
	std::cerr << "Error found: " << errorCode<< ", Message: " << errorMessage << std::endl;
}


void ChannelSwitch(LucyClient& L) {


	//Print Serverlist enumerated

	for (int i = 0; i< int(L.Serverlist.size()); i++) {
		std::cout << "[" << i << "] " << L.Serverlist[i].server.name << std::endl;

	}

	//Ask for Server enum
	for (int u; std::cout << "Enter Server number : " && std::cin >> u; ) {
		if (u >= int(L.Serverlist.size())) {
			std::cout << "Choose a valid number." << std::endl;
			goto end;
		}
		std::cout << "The server has been switched to " << L.Serverlist[u].server.name << std::endl;

		//Print Channellist enumerated
		for (int j = 0; j < int(L.Serverlist[u].ChannelList.size()); j++) {
			std::cout << "[" << u << "] " << "[" << j << "] " << L.Serverlist[u].ChannelList[j].name << std::endl;
		}
		//ask for channel enum on server
		for (int v; std::cout << "Enter Channel number : " && std::cin >> v; ) {

			if (v >= int(L.Serverlist[u].ChannelList.size())) {
				std::cout << "Choose a valid number." << std::endl;
				goto end;
			}

			L.CurrentChannel = L.Serverlist[u].ChannelList[v];

			//CC = L.Serverlist[u].ChannelList[v];		//Set CurrentChannel to the selected one
			std::cout << "The channel has been switched to " << L.CurrentChannel.name  << std::endl;
			goto end;
		}

		
	}

end:
	std::cout << "Finishing Channel switch.\n";


}

void CMD(std::string const & line, LucyClient& EL) {

	if (line == "!quit") {
		EL.quit();
	}

	else if (line == "!cs") {
		ChannelSwitch(EL);
	}

	else if (line == "!cc") {
		std::cout << "The current channel is " << EL.CurrentChannel.name << ", ID " << &EL.CurrentChannel.ID << std::endl;
	}
	else if (line == "!tl") {
		EL.Logging = !EL.Logging;
	}
	else if (line == "!connect") {


		EL.VCon = EL.createVoiceContext(EL.CurrentChannel.ID, EL.getChannel(EL.CurrentChannel.ID).cast().serverID, &(EL.VCEvent));
		EL.connectToVoiceChannel(EL.VCon, SleepyDiscord::BaseDiscordClient::normal);



	}
	else if (line == "!toggle_tts") {
		EL.tts = !EL.tts;
	}
	else if (line == "!checkDM") {
		std::cout << EL.OwnerDM.type<< " | " << std::string(EL.OwnerDM.ID) << std::endl;
	}
	else if (line.substr(0, 5) == "!nick") {
		EL.editNickname(EL.CurrentChannel.serverID, line.substr(6, int(line.length())-6));
	}
	else if (line.substr(0,7)=="!status") {						//Doesnt work...Why?
		EL.updateStatus("with her hair",uint64_t(0),SleepyDiscord::online,false); //line.substr(8,int(line.length())-8)
	}
	/*else if (line == "!LEAVE") {
		if (EL.CurrentChannel.serverID == "271034455462772737") {
			std::cout << "Can't leave the committee" << std::endl;
		}
		else {
			std::cout << "Are you SURE?" << std::endl;
			std::cout << "The current serverID is " << std::string(EL.CurrentChannel.serverID) << std::endl;
			for (std::string line2; std::cout << "Lucy > " && std::getline(std::cin, line2); )
			{
				if (line2 == "Yes") {
					EL.leaveServer(EL.CurrentChannel.serverID);
				}
				else break;
			}
		}
	
	}*/
	else {
		EL.sendMessage(EL.CurrentChannel, line, EL.tts);
	}

}



int main() {

	LucyClient Lucy(reference.token, 3);	//threads normally at 2, at 3 it autocalls run on a seperate thread, allowing main to continue
																
	

	for (std::string line; std::cout << "Lucy > " && std::getline(std::cin, line); )
	{
		if (!line.empty()) { CMD(line, Lucy); }
	}

	std::cout << "Goodbye.\n";

	return 0;
}