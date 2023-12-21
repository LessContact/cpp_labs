#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class CommandLineOptions
{
public:
    struct CommandInfo {
        std::string option;    // The command-line option
        std::string brief;     // Brief description of the option
    };
    bool PrintHelpIfRequested(int argc, char* argv[]);
    std::string getArgumentAfterFlag(int argc, char* argv[], const std::string& flag);
    bool hasWavExtension(const std::string& fileName);
private:
    void displayHelp(const std::map<std::string, CommandInfo>& commands);
    std::map<std::string, CommandInfo> commands {
                        {"-h", {"-h", "Display this help message"}},
                        {"-c <config.txt>", {"-c <config.txt>", "Specify the config file"}},
                        // Add more commands and descriptions here
                        {"conveters", {"available converters", ""}},
                        {"mute", {"mute X Y", "Mute 1st input file starting from X seconds for Y seconds"}},
                        {"mix", {"mix $X Y", "Mix 1st input file starting from Y seconds with inputfile X"}}
    };
};

