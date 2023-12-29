#include "CommandLineParser.h"
#include "ConfigParser.h"

void CommandLineOptions::displayHelp() {

    std::cout << "Usage: PCMprocess [-h] [-c config.txt output.wav input1.wav [input2.wav ...]]\n" << std::endl;

    std::queue<std::shared_ptr<Converter>> helpQueue;
    helpQueue.push(std::make_shared<MuteConverter>());
    helpQueue.push(std::make_shared<MixConverter>());
    helpQueue.push(std::make_shared<SpeedUpConverter>());

    std::cout << "Available Converters:" << std::endl;
    while (!helpQueue.empty())
    {
        auto& currentConverter = helpQueue.front();

        std::cout << currentConverter->GetSyntaxInfoString() << "\n" << std::endl;
        std::cout << currentConverter->GetParametersInfoString() << "\n" << std::endl;
        std::cout << currentConverter->GetFeaturesInfoString() << "\n" << std::endl;
        helpQueue.pop();
    }

    std::cout << "config.txt example:"<< std::endl
        << "--------------------------------------------------------" << std::endl
        << "# This is a comment\n"
        << "mute 10 5" << std::endl
        << "mix $2 5" << std::endl
        << "--------------------------------------------------------" << std::endl;
}

bool CommandLineOptions::PrintHelpIfRequested(int argc, char* argv[]) {
    if(argc < 5){ std::cerr << "Not enougn arguments passed." << std::endl;
        displayHelp();
        return true;}

    std::vector<std::string> args(argv + 1, argv + argc);
    // Search for help option in all the command-line arguments passed
    if(std::any_of(args.begin(), args.end(), [](const std::string& arg) {
        return arg == "-h";
    }))
    {
        displayHelp();
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
