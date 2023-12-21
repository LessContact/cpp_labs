#include "ConfigParser.h"

#include <limits>
#include <sstream>


namespace ConfigParser
{
    static constexpr char commentSymbol = '#';

    std::queue<std::shared_ptr<Converter>> GetConvertersFromConfig(const std::string& configPath,
                                                                   std::vector<std::string>& inputs, int32_t inputCount)
    {
        std::ifstream config(configPath);
        if (!config.is_open()) throw std::runtime_error("Failed to open config file!");

        std::string readLine;
        std::queue<std::shared_ptr<Converter>> converterQueue = {};
        while (!config.eof())
        {
            std::getline(config, readLine);
            if (readLine.empty() || readLine[0] == commentSymbol) continue;

            std::string inputFilePath = inputs[0];
            if (converterQueue.size() < inputCount - 1)
                inputFilePath = inputs[1 + converterQueue.size()];

            std::stringstream ss(readLine);
            if (readLine.find("mute") != std::string::npos)
            {
                int32_t intervalBeginSeconds = 0;
                int32_t intervalEndSeconds = 0;

                ss.ignore(std::numeric_limits<std::streamsize>::max(), ' '); // skip "mute"
                if (!(ss >> intervalBeginSeconds)) throw std::runtime_error("Failed to get interval begin.");

                if (!(ss >> intervalEndSeconds)) throw std::runtime_error("Failed to get interval end.");

                converterQueue.push(std::make_shared<MuteConverter>(converterQueue.empty() ? inputFilePath : inputs[0],
                                                                    inputs[0],
                                                                    intervalBeginSeconds, intervalEndSeconds));
            }
            else if (readLine.find("mix") != std::string::npos)
            {
                std::string mixFilePath;
                int32_t insertTimeSeconds = 0;

                ss.ignore(std::numeric_limits<std::streamsize>::max(), ' '); // skip "mix"
                if (!(ss >> mixFilePath)) throw std::runtime_error("Failed to get mix file reference");

                ss >> insertTimeSeconds;
                if (insertTimeSeconds < 0) throw std::runtime_error("Invalid time interval.");

                {
                    ss = std::stringstream(mixFilePath.data() + 1); // Skipping dollar
                    uint32_t n = 0;
                    ss >> n;
                    if (n > inputCount - 1)
                        throw std::runtime_error("Mix track number is greater than input track count.");
                    mixFilePath = inputs[n];
                }
                // converterQueue.push(ConverterFactory::Create<MixConverter>(
                //     converterQueue.empty() ? inputFilePath : inputs[0],
                //     inputs[0],
                //     mixFilePath, insertTimeSeconds));
                converterQueue.push(std::make_shared<MixConverter>(converterQueue.empty() ? inputFilePath : inputs[0],
                                                                   inputs[0],
                                                                   mixFilePath, insertTimeSeconds));
            }
            else if (readLine.find("speedup") != std::string::npos)
            {
                int32_t intervalBeginSeconds = 0;
                int32_t intervalEndSeconds = 0;
                int32_t speedUpTimes = 0;

                ss.ignore(std::numeric_limits<std::streamsize>::max(), ' '); // skip "speedup"

                if (!(ss >> intervalBeginSeconds)) throw std::runtime_error("Failed to get interval begin.");

                if (!(ss >> intervalEndSeconds)) throw std::runtime_error("Failed to get interval end.");

                // converterQueue.push(ConverterFactory::Create<MuteConverter>(
                //     converterQueue.empty() ? inputFilePath : inputs[0],
                //     inputs[0],
                //     intervalBeginSeconds, intervalEndSeconds, speedUpTimes));
            }
            else
                throw std::runtime_error("Unknown converter");
        }

        config.close();
        return converterQueue;
    }
}
