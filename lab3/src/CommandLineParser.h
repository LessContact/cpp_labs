#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class CommandLineOptions
{
public:
    bool PrintHelpIfRequested(int argc, char* argv[]);
    std::string getArgumentAfterFlag(int argc, char* argv[], const std::string& flag);
    bool hasWavExtension(const std::string& fileName);
private:
    void displayHelp();
};

