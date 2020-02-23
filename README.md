# LucyBot
A small-scale discord bot written for a personal server, in C++.

Uses: https://github.com/yourWaifu/sleepy-discord as a library, as well as some STL features.
Compiled with Visual Studio 2017.

Features: 
-Choose command
-Interactive command line that allows the owner to speak through Lucy and do some user-like stuff in the servers the bot is on (A "marionette")

Dependencies: This program was designed on Windows and rebuilt on Ubuntu 18.04.
-Cmake 3.10 or newer
-Libcurl: On ubuntu, this is libcurl4 and libcurl4-openssl-dev plus dependencies.
-OpenSSL: On ubuntu, use the libssl-dev package plus deps.
-SleepyDiscord, master branch, statically built with Cmake, Sub dependencies installed by it:
--CPR, master branch
--ASIO, 22afb8 branch
--Websocketpp, master branch
This was built with g++ 8.4.0 and had no issues. 

Build instructions:
Use Cmake! CmakeLists have been included. All you need to do to configure is to first write your bot token in the proper place and add some servers in the s1,s2,s3 spots. These server IDs must be ones of servers that the Bot is gonna be on. As this is intended to be small-scale, I do not intend on making this less static.
Then, simply create a build directory, and use cmake in it to create a makefile. Then, run make. 

Planned for next release: 
-no Bugfixes
-more features
