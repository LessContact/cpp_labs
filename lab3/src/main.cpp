#include "ConfigParser.h"
#include "CommandLineParser.h"
#include "WavParser.h"

int main(int argc, char** argv)
{
    std::vector<std::string> inputs;
    CommandLineOptions CLO;
    // Check if help is requested at any position in the argument list
    if (CLO.PrintHelpIfRequested(argc, argv)) return 0;

    std::string config = CLO.getArgumentAfterFlag(argc, argv, "-c");
    try
    {
        if (config.empty())
        {
            throw std::invalid_argument("No config file passed.");
        }

        for (int i = 1; i < argc; i++)
        {
            std::string str = argv[i];
            if (CLO.hasWavExtension(str))
            {
                inputs.emplace_back(argv[i]);
            }
        }

        std::queue<std::shared_ptr<Converter>> ConverterQueue = ConfigParser::GetConvertersFromConfig(
            config, inputs, argc - 3);

        while (!ConverterQueue.empty())
        {
            auto& currentTask = ConverterQueue.front();

            currentTask->Process();

            ConverterQueue.pop();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
