#pragma once
//Header for Main LucyBot implementation.

#include "sleepy_discord/websocketpp_websocket.h"


//Client class, inheriting from BaseDiscordClient from client.h
class LucyClient : public SleepyDiscord::DiscordClient { 	//create client class for the bot, inheritance structure: //BaseDiscordClient --> WebsocketppDiscordClient ==typedef in namespace SleepyDiscord== DiscordClient --> LucyClient															
public:
	using SleepyDiscord::DiscordClient::DiscordClient; 			//sets namespace(?)

	
	//Event handlers
	/*void onMessage(SleepyDiscord::Message UserMessage);
	void onError(SleepyDiscord::ErrorCode errorCode, const std::string errorMessage);*/
	void onReady(SleepyDiscord::Ready readyData);
	void onServer(SleepyDiscord::Server server);
	//void onQuit();
	void ChannelSwitch();
	void SafeMessaging(std::string const& message);
	void ServerSwitch();
	void CMD(std::string const& input);
    

	std::vector<SleepyDiscord::Server> ServerList;							//Should this be public/private? Should be private for security

private:
	bool tts;
	SleepyDiscord::Channel CurrentChannel {getChannel("286853054010097666")};
	SleepyDiscord::Server CurrentServer {getServer("271034455462772737")};
};
