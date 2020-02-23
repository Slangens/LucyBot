//Built with VSCode using G++ 8.3.0
//Version 2.0.0 
//Main LucyBot implementation.

#include "Headers/LucyBot.hpp"
#include "Headers/Ref.hpp"
#include <iostream>
#include <string>
#include <random>

#define SERVERDEBUG
//#define CHOOSEDEBUG

typedef std::uniform_int_distribution<int> unidis;									//Uniform distribution, used for Choose function


void LucyClient::onReady(SleepyDiscord::Ready readyData) {
	std::cout << "onReady was called." << std::endl;
	unsigned int Seed = std::chrono::system_clock::now().time_since_epoch().count(); //Give RNG its seed
	Chooser.seed(Seed);
	
}

void LucyClient::onServer(SleepyDiscord::Server server){ //OwO notices your server
            #ifdef SERVERDEBUG
            static unsigned short index {0};
            std::cout << "Server was registered" << std::endl;
            #endif

    static bool isFirstServer {true};                                                           //Indicator for first registered server. used once.

    this->ServerList.push_back(server);                                                         //The first server is used as a default.
            #ifdef SERVERDEBUG
            std::cout << "Server was added" << std::endl;
            #endif
    if(isFirstServer){
            #ifdef SERVERDEBUG
            std::cout << "This is the first server \n" << std::endl;    
            std::cout << server.name << std::endl;
            std::cout << this->CurrentServer.name << std::endl;
            std::cout << this->ServerList[0].name << std::endl;    
            #endif
            this->CurrentServer = this->ServerList[0];
                    //try{this->CurrentServer = server;}catch(std::runtime_error err){std::cerr << err.what()<< std::endl;}
            #ifdef SERVERDEBUG
                std::cout << server.name << std::endl;
                std::cout << this->CurrentServer.name << std::endl;
                std::cout << "The default server was set to this one." << std::endl;
            #endif
        }

            #ifdef SERVERDEBUG
            std::cout << "Server " << index << " has name " << server.name << " " << std::endl;
            std::cout << ",has " << server.channels.size() << " channels" << std::endl;
            std::cout << ", " << server.roles.size() << " roles" << std::endl;
            std::cout << "and " << server.members.size() << " members." << std::endl;
            index++;
            #endif
    isFirstServer = false;                                                                      //No further servers are used as current server.
    
}

std::string LucyClient::Choose(std::string& userInput){

    std::string prepared;                                                           // String of input for further use
    if(userInput.length() >8){                                                      // only when stuff exists past the leading command
        #ifdef CHOOSEDEBUG
            std::cout << userInput << std::endl;
        #endif
        prepared = userInput.substr(8,userInput.length());                          // get actual input
    }else{return "nothing, since you gave no arguments.Error code Choose_1";}       // otherwise print a default message.

    std::vector<std::string> Options;                                               // Create string vector from which one later chooses
    
    if(prepared.length() > 4){                                                      // if there is at least one option
        #ifdef CHOOSEDEBUG
            std::cout << prepared << std::endl;                                     
        #endif
    	size_t start {0}, end {0};                                                  // start and end as usual
                                                                                    // Now search for the first pair of quotations
        do {                                                                        // E.g.   input = " \"Des\" \"Pa\" \"Cito\" "
			start = prepared.find("\"", start) + 1;	                                // start = 3
			end = prepared.find("\"", start);                                       // end = 5
            
            if((start != std::string::npos) && (end != std::string::npos) && (end > start)){ // If the interval is valid
                
                std::string option = prepared.substr(start, end - start - 1);       // Make the content an option
                if (!(option.empty())) {                                            // Empty inputs are skipped
                        
                        Options.push_back(option);                                  // new argument is Des					
                }   

            }else { 
					std::cout << "Error. I didn't quite catch that." << std::endl;  // If the interval is stupid, print an error...
					return "a user that can properly use escaped characters and quotations.";//...and a roast to the user.Error code Choose_2.
            }
		    start = end + 1;                                                        // Advance to next pair 

		}while (!((start == std::string::npos) || (end == std::string::npos) || (start >= prepared.size()) || (end >= prepared.size()))); 
        //rinse and repeat until no more quotations remain.

			

		unidis Distrib(0, Options.size()-1);                                        //Create uniform distribution of length of the number of arguments
			
		size_t RNG = Distrib(Chooser);	                                            //Generate Random index according to uniform distrib 
			
		if (Options.size() == 1) {                                                  //if there is only one option, print sexy message
			return "you...and only you...";
		}
		else if(!(Options[RNG].empty())) {                                          //Print the chosen option
			return Options[RNG];

        }else{return "a sample text. There was an out of range error. Error code Choose_3.";}//Error case.

    }else{
        return "nothing, since you gave no arguments."; //Error code Choose_4, usual case if someone fucks up.
    }
    return "a sample text. There was a continue error.Error code Choose_5.";
}

void LucyClient::onMessage(SleepyDiscord::Message userMessage){
    if(userMessage.startsWith("-Choose") || userMessage.startsWith("-choose")){
        this->sendMessage(userMessage.channelID, userMessage.author.username + ", I choose " + this->Choose(userMessage.content) , this->tts);
    }
    if(userMessage.startsWith("im")|| userMessage.startsWith("Im")){ // mom jokes. Look up how to make a better case detection.
        sendMessage(userMessage.channelID,"Hi" + userMessage.content.substr(2,userMessage.length()) + ", I am mom"); 
    }
    if(userMessage.startsWith("i'm")|| userMessage.startsWith("I'm")){
        sendMessage(userMessage.channelID,"Hi" + userMessage.content.substr(3,userMessage.length()) + ", I am mom"); 
    }
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
                /*try{*/
                std::cin >> indexinput;                                                    //read in index                
                this->CurrentChannel = this->CurrentServer.channels[indexinput];            //update current channel
            /*}catch(std::invalid_argument err) {
				std::cerr << "Invalid argument given: " << err.what() << std::endl;
                }*/
}

void LucyClient::ServerSwitch(){
    for(size_t j=0; j < this->ServerList.size();j++){
        std::cout << "[" << j << "] " << this->ServerList[j].name << "\n";
    }
    std::cout << "Please insert your preferred server's index. \n" << std::endl;

    int indexinput;                                                                         //string to which getline reads and where the index is put into
    bool works=false;        
            //try{
            std::cin >> indexinput;                                                                //read in index    
            this->CurrentServer = this->ServerList[indexinput];                             //Update CurrentServer...
            works=true;
            /*}catch(std::invalid_argument err) {
				std::cerr << "Invalid argument given: " << err.what() << std::endl;
                }*/
    if(works){
        this->ChannelSwitch();                                                              //Then follow up with a channel switch.
    }
}

void LucyClient::CMD(std::string const& input){
    #ifdef SERVERDEBUG
    std::cout<<"entered CMD. \n" <<std::endl;
    #endif

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
    LucyClient Lucy(BotToken,3);	//threads normally at 2, at 3 it autocalls run on a seperate thread, allowing main to continue														
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