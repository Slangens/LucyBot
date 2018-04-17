// Lucy_console_v1_0.cpp : Defines the entry point for the console application.
//

//Built with MSVSC17
//Copied mostly from example by yourWaifu

#include "stdafx.h"  //req by VS17
#include "sleepy_discord/websocketpp_websocket.h"
#include "Ref.h"


Ref ref; //contains token, ownerid, server IDs.
std::vector<std::vector<SleepyDiscord::Channel>> Channellist; //A vector of two(server amount) vectors containing the channel objects of the respective server.
SleepyDiscord::Snowflake<SleepyDiscord::Channel> CurrentChannel("432969367534305283");
bool Logging = true;


class LucyClient : public SleepyDiscord::DiscordClient { 	//create client class for the bot, inheritance structure:

//BaseDiscordClient --> WebsocketppDiscordClient ==typedef in namespace SleepyDiscord== DiscordClient --> LucyClient
public:
	using SleepyDiscord::DiscordClient::DiscordClient; 			//sets namespace(?)
	



	void onReady(std::string* jsonMessage) {
		std::cout << "onReady was called." << std::endl;

		for(int i=0;i<int(ref.serverlist.size());i++){							//go through each server
			if (!(this->getServer(ref.serverlist[i]).cast().unavailable)) {		//check if theyre available
				std::cout << "Server " << i << " is available" << std::endl;
			}
		}
	}



	void onMessage(SleepyDiscord::Message UserMessage) { 		//redefines OnMessage event from client.h
		
		if (Logging && !(UserMessage.author == getCurrentUser().cast())) {
			std::cout << UserMessage.author.username << " said| " << UserMessage.content << " |at " << UserMessage.timestamp << "." << std::endl;
		}
		
		if (UserMessage.startsWith("-hello")) {				//if you type whcg hello, the bot greets you.
			sendMessage(UserMessage.channelID, "Hello " + UserMessage.author.username);
		}

		if (UserMessage.startsWith("Lucy, initiate shutdown") && UserMessage.author.ID == ref.ownerid ) {				//if you type "Lucy, initiate shutdown", she shuts down.
			sendMessage(UserMessage.channelID, "Gnight!" );
			quit();
		}

		if (UserMessage.startsWith("-ping")) {				//ping-pong, implement ping time later.
			sendMessage(UserMessage.channelID, "pong \\n You sent this at " + UserMessage.timestamp);
			//this->sendHeartbeat();

		}

		if (UserMessage.startsWith("-speak")) {				//Annoying tts command.
			sendMessage(UserMessage.channelID, "hi domo virtual Youtuber Kisuna i des", 1);
		} 

		/*if (UserMessage.startsWith("-ChannelName")) {
			editChannelName(UserMessage.channelID,);
		}*/
	} 



	void onServer(SleepyDiscord::Server server) {				//OwO when a server notices you
		std::cout << "onServer was called." << std::endl;

		for (int i = 0; i < int(ref.serverlist.size()); i++) {	//For each server

			if (server.ID == ref.serverlist[i]) {				//Check if the server is one of the ones on the list


				Channellist[i].resize(server.channels.size()); 
				Channellist[i] = server.channels;				//Update the channellist for the server

				}
			}
	}
	
};

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
	
	if(line == "!quit"){
		EL.quit();
	}

	else if(line == "!cs"){
		ChannelSwitch(CurrentChannel, EL);
	}

	else if (line == "!cc") {
		std::cout <<"The current channel is " << EL.getChannel(CurrentChannel).cast().name << ", ID " << std::string(CurrentChannel) << std::endl;
	}
	else if (line == "!tl") {
		Logging =!Logging;
	}

	else{
		EL.sendMessage(CurrentChannel, line);
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
