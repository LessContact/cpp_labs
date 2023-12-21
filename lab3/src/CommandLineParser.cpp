#include "CommandLineParser.h"

void CommandLineOptions::displayHelp(const std::map<std::string, CommandLineOptions::CommandInfo>& commands) {

    std::cout << "Usage: PCMprocess [-h] [-c config.txt output.wav input1.wav [input2.wav ...]]" << std::endl;
    for (const auto& commandPair : commands) {
        const auto& info = commandPair.second;
        std::cout << info.option << ": " << info.brief << '\n';
    }
    std::cout << "config.txt example:"<< std::endl
        << "--------------------------------------------------------" << std::endl
        << "# Mute 1st input file starting from 10 seconds 5 seconds\n"
        << "mute 10 5" << std::endl
        << "mix $2 5" << std::endl
        << "--------------------------------------------------------" << std::endl;
}

bool CommandLineOptions::PrintHelpIfRequested(int argc, char* argv[]) {
    if(argc < 5){ std::cerr << "Not enougn arguments passed." << std::endl;
        displayHelp(commands);
        return true;}

    std::vector<std::string> args(argv + 1, argv + argc);
    // Search for help option in all the command-line arguments passed
    if(std::any_of(args.begin(), args.end(), [](const std::string& arg) {
        return arg == "-h";
    }))
    {
        displayHelp(commands);
        return true;
    }
    return false;
}

std::string CommandLineOptions::getArgumentAfterFlag(int argc, char* argv[], const std::string& flag)
{
    for (int i = 1; i < argc - 1; ++i) {
        if (argv[i] == flag) {
            // Make sure the next argument is not a flag or a wav file
            if (argv[i + 1][0] != '-' && !hasWavExtension(argv[i+1])) {
                return argv[i + 1];
            } else {
                std::cerr << "Error: Expected config file name after \"" << flag << "\", but got \"" << argv[i + 1] <<"\"" << std::endl;
                return "";
            }
        }
    }
    return "";
}

bool CommandLineOptions::hasWavExtension(const std::string& fileName) {
    if (fileName.size() < 4) {
        return false;
    }
    std::string extension = fileName.substr(fileName.size() - 4);
    return extension == ".wav";
}
