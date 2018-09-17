// LucyBot.cpp : Defines the entry point for the console application.
//
//Built with MSVSC17

#include "stdafx.h"  //req by VS17
#include "sleepy_discord/websocketpp_websocket.h"
#include "sleepy_discord/voice_connection.h"
#include "Ref.h"
#include <chrono>
#include <random>
#include <iostream>
#include <fstream>

typedef std::uniform_int_distribution<int> unidis;									//Uniform distribution, used for Choose function
typedef std::array < std::array < std::vector < std::string >, 31 >, 12 > BDarr;	//Container for Birthday names

Ref reference; //contains token, ownerid, server IDs.

//Improvement upon the server object structure, might be inefficient
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

//Voice event handler needed for activity in VC
class LucyVoiceHandler : public SleepyDiscord::BaseVoiceEventHandler {



public:
	LucyVoiceHandler() {}
	void onReady(SleepyDiscord::VoiceConnection& connection) {
		/*Do stuff when ready to start sending audio*/

	}

};

//Client class, inheriting from BaseDiscordClient from client.h
class LucyClient : public SleepyDiscord::DiscordClient { 	//create client class for the bot, inheritance structure:

															//BaseDiscordClient --> WebsocketppDiscordClient ==typedef in namespace SleepyDiscord== DiscordClient --> LucyClient
public:
	using SleepyDiscord::DiscordClient::DiscordClient; 			//sets namespace(?)

	
	//Event handlers
	void colourprint(SleepyDiscord::Message UM);
	void colourgive(SleepyDiscord::Message UM);
	void onReady(SleepyDiscord::Ready readyData);
	void onMessage(SleepyDiscord::Message UserMessage);
	void onServer(SleepyDiscord::Server server);
	void onError(SleepyDiscord::ErrorCode errorCode, const std::string errorMessage);
	void onQuit();

	//Mixed variables
	int ServerIndex(SleepyDiscord::Snowflake<SleepyDiscord::Server> ServerID);
	std::vector<LucyServer> Serverlist;
	BDarr BirthdayArray;  //Array of depth 12 containing arrays of depth 31, each containing a vector of strings. No elements need to be added to the array, just the vectors.
	bool Logging = false;
	bool tts = false;
	SleepyDiscord::Channel CurrentChannel=getChannel("286853054010097666");
	int ServerCount = 0;									//Used to check if all servers have been run through, used by all kinds of functions, so public
	

private:
	SleepyDiscord::User ColourmeClient;
	bool activeColourme = false;
	bool MomJokeCooldown = false;
	std::minstd_rand Chooser;

	std::ofstream PingList;
	std::ifstream BirthdayInput;
	LucyVoiceHandler LVH;
};


void BirthdayLoad(BDarr& BirthdayList, std::ifstream& Input) {
	try {
		Input.open("Birthdays.txt");
		if (Input.is_open()) {
			std::cout << "Birthday file opened." << std::endl;

		}
		else { std::cerr << "File could not be opened." << std::endl; }
	}
	catch (std::ios_base::failure FileNotFound) {
		std::cerr << "Error code " << FileNotFound.code() << " , " << FileNotFound.what() << std::endl;
	}
}

//Event redefinitions

//Finds the enumerator of a given server in the array of servers --> inefficient
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

//Prints colours in chat
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

//Gives the current person using colourme the colour they want
void LucyClient::colourgive(SleepyDiscord::Message UM) {

	SleepyDiscord::Snowflake<SleepyDiscord::Server> A = getChannel(UM.channelID).cast().serverID;
	
	std::vector<SleepyDiscord::Snowflake<SleepyDiscord::Role>> userRoles = getMember(A, UM.author).cast().roles;
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

//Executed on finishing the connection to Discord
void LucyClient::onReady(SleepyDiscord::Ready readyData) {
	std::cout << "onReady was called." << std::endl;
	unsigned int Seed = std::chrono::system_clock::now().time_since_epoch().count(); //Give RNG its seed
	Chooser.seed(Seed);
	
	//Read in Birthday list
	BirthdayLoad(BirthdayArray, BirthdayInput);

}

//Executed when a message is received by the client
void LucyClient::onMessage(SleepyDiscord::Message UserMessage) { 		
	//redefines OnMessage event from client.h
	//If the author of the message isnt Lucy(should be static compare tbh)
	if (!(UserMessage.author == getCurrentUser().cast())) {

		if (this->Logging) {
			std::cout << UserMessage.author.username << " said| " << UserMessage.content << " |at " << UserMessage.timestamp << "." << std::endl;
			
		}

		if (UserMessage.startsWith("-hello")) {				//if you type whcg hello, the bot greets you.
			sendMessage(UserMessage.channelID, "hello " + UserMessage.author.username + ", what's up");
		}

		if (UserMessage.startsWith("Lucy, initiate shutdown") ) {				//if you type "Lucy, initiate shutdown", she shuts down...or does she?
			sendMessage(UserMessage.channelID, "You think you can get rid of me? Fool.");
			
		}
		
		if (UserMessage.startsWith("-ping")) {				//ping-pong, implement ping time later.
			sendMessage(UserMessage.channelID, "Pong! \\n You sent this at " + UserMessage.timestamp);
			//this->sendHeartbeat();

		}

		if (UserMessage.startsWith("-speak")) {				//Annoying tts command.
			sendMessage(UserMessage.channelID, "hi domo virtual Youtuber Kisuna i des", 1);
		}

		if (UserMessage.startsWith("*patsbots") && UserMessage.author.ID == reference.ownerid) {				//Clingy bot command.
			sendMessage(UserMessage.channelID, "*purrs* \\n I luv u", 0);
		}

		if (UserMessage.startsWith("tibbired")) {				//Clingy bot command.
			sendMessage(UserMessage.channelID, "https://cdn.discordapp.com/attachments/343802646320250882/343802829829308427/sus.gif", 0);
		}

		if (UserMessage.startsWith("thats what they all say")) {				//Clingy bot command.
			sendMessage(UserMessage.channelID, "and then they get pregnant", 0);
		}

	

		if (UserMessage.startsWith("-help")) {				//Help command
			sendMessage(UserMessage.channelID, "-hello: Makes me greet you \\n -ping: Makes me pong you and show you when you sent your ping \\n -speak: Makes me show you the true meaning of fear, as long as you have tts enabled \\n -colourme: Starts a dialogue that makes me assign a [C] role to you. Use WITHOUT arguments, instructions will follow.\\n Des: find out yourself ;P");
		}

		if (UserMessage.startsWith("alexa play despacito") || UserMessage.startsWith("Alexa play despacito") ||  UserMessage.startsWith("lucy play despacito") || UserMessage.startsWith("Lucy play despacito")) {

			sendMessage(UserMessage.channelID,"https://www.youtube.com/watch?v=W3GrSMYbkBE");
		}

		if (UserMessage.startsWith("Welcome to the Committee") && (UserMessage.author.ID == "88575421972516864")) {

			sendMessage("484895888507011098", "F R E S H");
		}

		if (UserMessage.startsWith("Is this loss") || UserMessage.startsWith("is this loss")) {
			sendMessage(UserMessage.channelID,"No, this is Lucy :>",0);
		}

		if (UserMessage.startsWith("Lucy, what do you think of 216") || UserMessage.startsWith("lucy, what do you think of 216") || UserMessage.startsWith("Lucy, what do you think of 42") || UserMessage.startsWith("lucy, what do you think of 42") || UserMessage.startsWith("Lucy, what do you think of 42cy") || UserMessage.startsWith("lucy, what do you think of 42cy")) {

			sendMessage(UserMessage.channelID, "I don't like her.");
			sendMessage(UserMessage.channelID, "+hello");
			schedule([this, UserMessage]() {
				sendMessage(UserMessage.channelID, "See, she's always this crabby.");
			}, 2000);

		}

		if (UserMessage.isMentioned(reference.ownerid)) {
			schedule([this, UserMessage]() {
				if (!(UserMessage.author.ID == reference.ownerid )) {

					if (!(getMessage(UserMessage.channelID, getChannel(UserMessage.channelID).cast().lastMessageID).cast().author.ID == reference.ownerid)) {
						
						sendMessage(UserMessage.channelID, "I have relayed your message to him.", 0);
						//sendMessage(OwnerDM,UserMessage.author.username + " pinged you in " + getChannel(UserMessage.channelID).cast().name, 0);

						PingList.open("PingList.txt");
						if (PingList.is_open()	) {
							PingList << "At "
								<< UserMessage.timestamp
								<< ", "
								<< UserMessage.author.username
								<< "pinged you and said: "
								<< UserMessage.content
								<< " in the channel "
								<< getChannel(UserMessage.channelID).cast().name

								<< std::endl;
						}

					}

				}
			}, 5000);
			
		}

		if (UserMessage.isMentioned(getCurrentUser().cast())) {
			sendMessage(UserMessage.channelID,"Is there something?" , 0); //"Hi there~ \\n I hope you have a reason for pinging me though =v=\\n If you need more info on me, try the -help command ;p"
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

		
	}
	//Choose function may be used by Lucy as well
	if (UserMessage.startsWith("-Choose")) { //Initiate choose command.
		//Parse input to give a vector of strings containing the arguments
		// User enters: "-Choose \"Des\" \"Pa\" \"Cito\" "
		std::string input = UserMessage.content.substr(7, UserMessage.content.length() - 7);
		std::cout << input << std::endl;
		std::vector<std::string> output;
		bool write = 0,eat = 0;
		
		if(input.size() > 2){
		
			//Method 1: +String methods
			size_t start = 0, end = 0;
			do { // E.g.   input = " \"Des\" \"Pa\" \"Cito\" "

				start = input.find("\"", start) + 1;	// start = 3
				end = input.find("\"", start); // end = 5

				std::string option = input.substr(start, end - start - 1);

				if ((start != std::string::npos) && (end != std::string::npos) && (end > start) && !(option.empty())) {

					if ((option == "Write") || (option == "write")) {
						write = 1;
						break;
					}
					else if ((option == "meow")||(option == "Meow")) {
						continue;
					}
					else if(option == "eat"){
						eat = 1;
						break;
					}
					else {
						output.push_back(option); //new argument is Des
					}
					
					
					
				}
				else { 
					std::cout << "Error. I didn't quite catch that." << std::endl;
					sendMessage(UserMessage.channelID, "Would you mind using proper quotation and avoid escaped characters like backslashes? ^_^", 0);
					break; 
				}

				start = end + 1;


			} while (!((start == std::string::npos) || (end == std::string::npos) || (start >= input.size()) || (end >= input.size()))); //rinse and repeat until no more quotations remain.

			

			 //Create uniform distribution of length of the number of arguments
			unidis Distrib(0, output.size()-1);
			
			//Generate RN according to unidis
			int RNG = Distrib(Chooser);
			
			

			
			//return the corresponding argument
			if (output.size() == 1) {
				sendMessage(UserMessage.channelID,"I guess you really only want one thing...~", 0);

			}
			else if (write) {
				sendMessage(UserMessage.channelID, "Write the fanfic. Do it now.", 0);
			}
			else if (eat) {
				sendMessage(UserMessage.channelID, "You're too thin. You need f o o d", 0);
			}
			else if (!(output[RNG].empty())) {
				sendMessage(UserMessage.channelID, output[RNG], 0);

			}

		}
		else { sendMessage(UserMessage.channelID, "Error. No arguments given.", 0); }


	}
	//Despacito
	if (UserMessage.startsWith("Des")) {
		sendMessage(UserMessage.channelID, "Pa \\n", 0);
		schedule([this, UserMessage]() {
		sendMessage(UserMessage.channelID, "Cito \\n", 0);
		}, 700); //Look up what a lambda body/capture list is
	}
}

//Executed when a server is registered or re-registered
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

//Executed on receiving an error from the client
void LucyClient::onError(SleepyDiscord::ErrorCode errorCode, const std::string errorMessage) {
	std::cerr << "Error found: " << errorCode<< ", Message: " << errorMessage << std::endl;
}

//Executed on disconnect
void LucyClient::onQuit() {
	
}

//Console functionality

//Needs revamp, god why are there gotos
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

//Needs complete revamp, else ifs are inefficient
void CMD(std::string const & line, LucyClient& EL) {
	

//Quitting
	if (line == "!quit") {
		EL.quit();
	}
//Channel Switching
	else if (line == "!cs") {
		ChannelSwitch(EL);
	}
//Returns current channel
	else if (line == "!cc") {
		std::cout << "The current channel is " << EL.CurrentChannel.name << ", ID " << std::string(EL.CurrentChannel.ID) << std::endl;
	}
//Toggle Logging
	else if (line == "!tl") {
		EL.Logging = !EL.Logging;
	}
//Toggle Text-to-speech
	else if (line == "!toggle_tts") {
		EL.tts = !EL.tts;
	}
//Change Nickname
	else if (line.substr(0, 5) == "!nick") {
		EL.editNickname(EL.getChannel(EL.CurrentChannel.ID).cast().serverID, line.substr(6, int(line.length())-6));
	}
//Change status, crashes, no idea why it doesnt work
	else if (line.substr(0,7)=="!status") {						//Doesnt work...Why?
		
		EL.updateStatus("with her hair",0,SleepyDiscord::online,0); //line.substr(8,int(line.length())-8)
	}
	else if (line == "!error") {

		if (EL.CurrentChannel.type == SleepyDiscord::Channel::ChannelType::SERVER_TEXT) { EL.sendMessage(EL.CurrentChannel, "In the (not uncommon) case that I have misunderstood your question or the social context of it, please consult Slangens \\n Or preferably someone competent.", 0); }
		else { std::cout << "This is not a text channel." << std::endl; }
		
	}
	else if (line.substr(0, 7) == "!upload") {
		EL.uploadFile(EL.CurrentChannel, line.substr(8,line.length()-8), "[Uploading...]");
	}
	else if (line.substr(0, 6) == "!react") {
		EL.uploadFile(EL.CurrentChannel, "Reactions" + line.substr(7, line.length() - 7), " ");
	}
	else if (line == "!connect") {
		EL.connectToVoiceChannel(EL.CurrentChannel.ID, EL.CurrentChannel.serverID);
		//EL.connectToVoiceChannel(EL.CurrentChannel,EL.CurrentChannel.serverID,SleepyDiscord::BaseDiscordClient::normal );
		//std::cout << std::string(EL.getChannel(EL.CurrentChannel).cast().serverID) << " " << EL.getServer(EL.getChannel(EL.CurrentChannel).cast().serverID).cast().name << std::endl;
		//EL.connectToVoiceChannel(EL.createVoiceContext(EL.CurrentChannel, EL.CurrentChannel.serverID, &EL.LVH), SleepyDiscord::BaseDiscordClient::mute);
	}

//If no command is used, send input in current channel as standard message, if that channel is a text channel.
	else {
		if (EL.CurrentChannel.type == SleepyDiscord::Channel::ChannelType::SERVER_TEXT) { EL.sendMessage(EL.CurrentChannel, line, EL.tts);  }
		else { std::cout << "This is not a text channel." << std::endl; }
		
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