// Lucy_console_v1_0.cpp : Defines the entry point for the console application.
//

//Built with MSVSC17
//Copied mostly from example by yourWaifu

#include "stdafx.h"  //req by VS17
#include "sleepy_discord/websocketpp_websocket.h"
#include "Ref.h"


Ref ref; //contains token, ownerid, server IDs.
std::vector<std::vector<SleepyDiscord::Channel>> Channellist; //A vector of two(server amount) vectors containing the channel objects of the respective server.


class LucyClient : public SleepyDiscord::DiscordClient { 		//create client class for the bot, inheritance structure:
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
			std::cout << "Looking at server " << i << std::endl;

			if (server.ID == ref.serverlist[i]) {				//Check if the server is one of the ones on the list
				std::cout << "Server " << i <<" is on the list, its channels will be copied."<< std::endl;
				Channellist[i] = server.channels;				//Then, put the server's channellist into the corresponding channel vector.
				std::cout << "Server " << i <<"'s channels copied."<< std::endl;
				
				//Greeting
				for (int j = 0; j <= int(Channellist[i].size()); j++) {			//check for each channel in the server
					if (Channellist[i][j].type ==0) {							//if it's a text channel, and then

					sendMessage(Channellist[i][j].ID, "My name is " + this->getCurrentUser().cast().username);
					sendMessage(Channellist[i][j].ID, "Main systems activation...successful."); //Make a notification and
					break;
					}
				}



			}
		}
	}


	/*void onQuit() {
		sendMessage(std::string(432969367534305283), "Sleep mode engaged.");
	}*/
};

void CMD(std::string const & line,LucyClient& L){
	L.sendMessage("432969367534305283", line);
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
