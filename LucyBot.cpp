//Built with VSCode using G++ 7.3.0
//Version 2.0.0
//Main LucyBot implementation.

#include "Headers/LucyBot.hpp"
#include "Headers/Ref.hpp"
#include <iostream>

void LucyClient::onServer(SleepyDiscord::Server server){ //OwO notices your server
    //static unsigned short index {0};
    this->ServerList.push_back(server);
    
    /*std::cout << "Server " << index << " has name " << server.name << " " << std::endl;
    std::cout << ",has " << server.channels.size() << " channels" << std::endl;
    std::cout << ", " << server.roles.size() << " roles" << std::endl;
    std::cout << "and " << server.members.size() << " members." << std::endl;
    index++;*/
}

void LucyClient::SafeMessaging(std::string const& message){  //Modification of message sends only in text channels. Use with regards to how often the message happens.
    if (this->CurrentChannel.type == SleepyDiscord::Channel::ChannelType::SERVER_TEXT) { this->sendMessage(this->CurrentChannel, message, this->tts);  }
		else { std::cerr << "This is not a text channel." << std::endl; }

}

void LucyClient::ChannelSwitch(){
    for(size_t i=0; i<this->CurrentServer.channels.size();i++){                                             //go through all channels, indexed
               if (this->CurrentChannel.type == SleepyDiscord::Channel::ChannelType::SERVER_TEXT){          //If a channel is a text channel,
                std::cout << "[" << i << "] " << this->CurrentServer.channels[i].name << "\n";              //print its name and index. 
               }
            }
    std::cout << "Please insert your preferred channel's index. \n" << std::endl;
            
    int indexinput;                                                                         //string to which getline reads and where the index is put into
                try{
                std::cin >> indexinput;                                                    //read in index                
                this->CurrentChannel = this->CurrentServer.channels[indexinput];            //update current channel
            }catch(std::invalid_argument err) {
				std::cerr << "Invalid argument given: " << err.what() << std::endl;
                }
}

void LucyClient::ServerSwitch(){
    for(size_t j=0; j < this->ServerList.size();j++){
        std::cout << "[" << j << "] " << this->ServerList[j].name << "\n";
    }
    std::cout << "Please insert your preferred server's index. \n" << std::endl;

    int indexinput;                                                                         //string to which getline reads and where the index is put into
    bool works=false;        
            try{
            std::cin >> indexinput;                                                                //read in index    
            this->CurrentServer = this->ServerList[indexinput];                             //Update CurrentServer...
            works=true;
            }catch(std::invalid_argument err) {
				std::cerr << "Invalid argument given: " << err.what() << std::endl;
                }
    if(works){
        this->ChannelSwitch();                                                              //Then follow up with a channel switch.
    }
}

void LucyClient::CMD(std::string const& input){

    //channel switching
	if (input == "!cs") {
		this->ChannelSwitch();
    }else if(input == "!ss"){
        this->ServerSwitch();
    //Print all server info
    }else if(input == "!print"){

        for(const SleepyDiscord::Server& server : this->ServerList){
            std::cout << "Server " << " has name " << server.name << " " << std::endl;
            std::cout << ",has " << server.channels.size() << " channels" << std::endl;
            std::cout << ", " << server.roles.size() << " roles" << std::endl;
            std::cout << "and " << server.members.size() << " members." << std::endl;
        }
    //Otherwise just post what you wrote in CurrentChannel
    }else{
        this->SafeMessaging(input);

    }

}




int main(){
LucyClient Lucy(BotToken, 3);	//threads normally at 2, at 3 it autocalls run on a seperate thread, allowing main to continue														
std::string line;                           //Create input string

    while(true){                            //Execute input loop infinitely until program quits
    std::cout << "Lucy >";                  //Give shell-lookalike interface
    std::getline(std::cin, line);           //Get input from cin
    if(!line.empty()) {                     //If nonempty string found in input,

        if(line == "!quit"){                //Check if you want to quit
            Lucy.quit();
            break;                          //Exit loop
        }
        Lucy.CMD(line);                    //do something with it otherwise
        }
    }

	std::cout << "Exiting console application. \n";

    return 0;
}