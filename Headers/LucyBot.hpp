#pragma once
//Header for Main LucyBot implementation.

#include "sleepy_discord/websocketpp_websocket.h"


//Client class, inheriting from BaseDiscordClient from client.h
class LucyClient : public SleepyDiscord::DiscordClient { 	//create client class for the bot, inheritance structure: //BaseDiscordClient --> WebsocketppDiscordClient ==typedef in namespace SleepyDiscord== DiscordClient --> LucyClient															
public:
	using SleepyDiscord::DiscordClient::DiscordClient; 			//sets namespace(?)

	
	//Event handlers
	/*void onReady(SleepyDiscord::Ready readyData);
	void onMessage(SleepyDiscord::Message UserMessage);
	void onServer(SleepyDiscord::Server server);
	void onError(SleepyDiscord::ErrorCode errorCode, const std::string errorMessage);
	void onQuit();*/
	void LucyMessaging(std::string const& message);

    
	

private:
	bool tts {0};
	SleepyDiscord::Channel CurrentChannel {getChannel("286853054010097666")};
};

void CMD(std::string const& input,LucyClient& EL);