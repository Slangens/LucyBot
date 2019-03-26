//Built with VSCode using G++ 7.3.0
//Version 2.0.0
//Main LucyBot implementation.

#include "Headers/LucyBot.hpp"
#include "Headers/Ref.hpp"
#include <iostream>

void LucyClient::LucyMessaging(std::string const& message){
    if (this->CurrentChannel.type == SleepyDiscord::Channel::ChannelType::SERVER_TEXT) { this->sendMessage(this->CurrentChannel, message, this->tts);  }
		else { std::cerr << "This is not a text channel." << std::endl; }
}

void CMD(std::string const& input,LucyClient& EL){

    //Quitting
	if (input == "iQuick") {
		EL.LucyMessaging("meow");
	}
    else{
        EL.LucyMessaging(input);
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
        CMD(line, Lucy);                    //do something with it otherwise
        }
    }

	std::cout << "Exiting console application. \n";

    return 0;
}