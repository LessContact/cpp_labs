#include <cstring>
#include <bits/algorithmfwd.h>

#include "ConfigParser.h"

MuteConverter::MuteConverter(const std::string& inputFilePath, const std::string& outputFilePath,
                             const uint32_t intervalBeginSeconds,
                             const uint32_t intervalEndSeconds)
    : Converter(inputFilePath, outputFilePath), IntervalBeginSeconds(intervalBeginSeconds),
      IntervalEndSeconds(intervalEndSeconds)
{
}
//TODO: put all chekcs inside the converters themselves
//TODO: also put info abt converters inside the converters
void MuteConverter::Process()
{
    const static std::string tempFilePath = "mute_Converter.TMP";

    std::ifstream inputTrack(InputFilePath, std::ios::in | std::ios::binary);
    if (!inputTrack.is_open()) throw std::runtime_error("Failed to open .wav!");

    const bool isRenameNeeded = InputFilePath == OutputFilePath;
    std::ofstream outTrack(isRenameNeeded ? tempFilePath : OutputFilePath, std::ios::out | std::ios::binary);
    if (!outTrack.is_open())
    {
        inputTrack.close();
        throw std::runtime_error("Failed to open .wav!");
    }

    if (IntervalBeginSeconds < 0) throw std::runtime_error("Invalid interval.");
    if (IntervalEndSeconds < 0) throw std::runtime_error("Invalid interval.");

    if (IntervalBeginSeconds >= IntervalEndSeconds)
        throw std::runtime_error("intervalBeginSeconds >= intervalEndSeconds.");

    WavParser inputWav = {};
    inputWav.ReadWavHeader(inputWav.header, inputTrack);
    outTrack.write(reinterpret_cast<const char*>(&inputWav), sizeof(inputWav));

    std::vector<int16_t> inputTrackSamples;
    inputTrackSamples.resize(inputWav.header.sampleRate);

    const uint32_t inputTrackDurationSeconds = inputWav.GetTrackDurationSeconds();
    if (IntervalEndSeconds > inputTrackDurationSeconds) IntervalEndSeconds = inputTrackDurationSeconds;

    for (uint32_t elapsedSeconds = 0; elapsedSeconds < inputTrackDurationSeconds; ++elapsedSeconds)
    {
        if (inputTrack.eof()) break;

        inputTrack.read(reinterpret_cast<char*>(inputTrackSamples.data()),
                        inputTrackSamples.size() * sizeof(inputTrackSamples[0]));

        if (elapsedSeconds >= IntervalBeginSeconds && elapsedSeconds < IntervalEndSeconds)
            memset(inputTrackSamples.data(), 0, inputTrackSamples.size() * sizeof(inputTrackSamples[0]));
        outTrack.write(reinterpret_cast<const char*>(inputTrackSamples.data()),
                       inputTrackSamples.size() * sizeof(inputTrackSamples[0]));
    }

    outTrack.close();
    inputTrack.close();

    if (isRenameNeeded &&
        (std::remove(OutputFilePath.data()) != 0 ||
            std::rename(tempFilePath.data(), OutputFilePath.data()) != 0))
        throw std::runtime_error("Failed to complete converter operations!");
}

MixConverter::MixConverter(const std::string& inputFilePath, const std::string& outputFilePath,
                           const std::string& mixFilePath,
                           uint32_t insertTimeSeconds)
    : Converter(inputFilePath, outputFilePath), AdditionalStream(mixFilePath), InsertTimeSeconds(insertTimeSeconds)
{
}

void MixConverter::Process()
{
    const static std::string tempFilePath = "mix_Converter.TMP";

    std::ifstream inputTrack(InputFilePath, std::ios::in | std::ios::binary);
    if (!inputTrack.is_open()) throw std::runtime_error("Failed to open .wav");

    std::ifstream extraTrack(AdditionalStream, std::ios::in | std::ios::binary);
    if (!extraTrack.is_open())
    {
        inputTrack.close();
        throw std::runtime_error("Failed to open .wav!");
    }

    const bool isRenameNeeded = InputFilePath == OutputFilePath;
    std::ofstream outTrack(isRenameNeeded ? tempFilePath : OutputFilePath, std::ios::out | std::ios::binary);
    if (!outTrack.is_open())
    {
        inputTrack.close();
        extraTrack.close();
        throw std::runtime_error("Failed to open .wav!");
    }

    WavParser inputWav = {};
    inputWav.ReadWavHeader(inputWav.header, inputTrack);
    outTrack.write(reinterpret_cast<const char*>(&inputWav), sizeof(inputWav));

    std::vector<int16_t> inputTrackSamples;
    inputTrackSamples.resize(inputWav.header.sampleRate);

    WavParser extraWav = {};
    extraWav.ReadWavHeader(extraWav.header, extraTrack);
    std::vector<int16_t> extraTrackSamples;
    extraTrackSamples.resize(extraWav.header.sampleRate);

    const uint32_t inputTrackDurationSeconds = inputWav.GetTrackDurationSeconds();

    bool ExtraTrackEnded = false;
    for (uint32_t elapsedSeconds = 0; elapsedSeconds < inputTrackDurationSeconds; ++elapsedSeconds)
    {
        if (inputTrack.eof()) break;

        inputTrack.read(reinterpret_cast<char*>(inputTrackSamples.data()),
                        inputTrackSamples.size() * sizeof(inputTrackSamples[0]));

        if (!extraTrack.eof())
            extraTrack.read(reinterpret_cast<char*>(extraTrackSamples.data()),
                            extraTrackSamples.size() * sizeof(extraTrackSamples[0]));

        for (uint32_t sampleIndex = 0; sampleIndex < inputTrackSamples.size(); ++sampleIndex)
        {
            if (elapsedSeconds >= InsertTimeSeconds && !ExtraTrackEnded)
            {
                inputTrackSamples[sampleIndex] = (inputTrackSamples[sampleIndex] + extraTrackSamples[sampleIndex]) / 2;
            }
        }
        outTrack.write(reinterpret_cast<const char*>(inputTrackSamples.data()),
                       inputTrackSamples.size() * sizeof(inputTrackSamples[0]));
        ExtraTrackEnded = extraTrack.eof();
    }

    inputTrack.close();
    extraTrack.close();
    outTrack.close();

    if (isRenameNeeded &&
        (std::remove(OutputFilePath.data()) != 0 ||
            std::rename(tempFilePath.data(), OutputFilePath.data()) != 0))
        throw std::runtime_error("Failed to complete converter operations!");
}

SpeedUpConverter::SpeedUpConverter(const std::string& inputFilePath, const std::string& outputFilePath,
                                   const uint32_t intervalBeginSeconds,
                                   const uint32_t intervalEndSeconds, const uint32_t speedUpTimes)
    : Converter(inputFilePath, outputFilePath), IntervalBeginSeconds(intervalBeginSeconds),
      IntervalEndSeconds(intervalEndSeconds), SpeedUpTimes(speedUpTimes)
{
}

void SpeedUpConverter::Process()
{
    const static std::string tempFilePath = "speedup_Converter.TMP";

    std::ifstream inputTrack(InputFilePath, std::ios::in | std::ios::binary);
    if (!inputTrack.is_open()) throw std::runtime_error("Failed to open .wav!");

    const bool isRenameNeeded = InputFilePath == OutputFilePath;
    std::ofstream outTrack(isRenameNeeded ? tempFilePath : OutputFilePath, std::ios::out | std::ios::binary);
    if (!outTrack.is_open())
    {
        inputTrack.close();
        throw std::runtime_error("Failed to open .wav!");
    }

    if (IntervalBeginSeconds < 0) throw std::runtime_error("Invalid interval.");
    if (IntervalEndSeconds < 0) throw std::runtime_error("Invalid interval.");

    if (IntervalBeginSeconds >= IntervalEndSeconds)
        throw std::runtime_error("intervalBeginSeconds >= intervalEndSeconds.");

    WavParser inputWav = {};
    inputWav.ReadWavHeader(inputWav.header, inputTrack);
    outTrack.write(reinterpret_cast<const char*>(&inputWav), sizeof(inputWav));

    std::vector<int16_t> inputTrackSamples;
    inputTrackSamples.resize(inputWav.header.sampleRate);

    const uint32_t inputTrackDurationSeconds = inputWav.GetTrackDurationSeconds();
    if (IntervalEndSeconds > inputTrackDurationSeconds) IntervalEndSeconds = inputTrackDurationSeconds;

    for (uint32_t elapsedSeconds = 0; elapsedSeconds < inputTrackDurationSeconds; ++elapsedSeconds)
    {
        if (inputTrack.eof()) break;

        inputTrack.read(reinterpret_cast<char*>(inputTrackSamples.data()),
                        inputTrackSamples.size() * sizeof(inputTrackSamples[0]));

        if (elapsedSeconds >= IntervalBeginSeconds && elapsedSeconds < IntervalEndSeconds)
        {

            outTrack.write(reinterpret_cast<const char*>(inputTrackSamples.data()),
                       inputTrackSamples.size() * sizeof(inputTrackSamples[0]));
        }
    }

    outTrack.close();
    inputTrack.close();

    if (isRenameNeeded &&
        (std::remove(OutputFilePath.data()) != 0 ||
            std::rename(tempFilePath.data(), OutputFilePath.data()) != 0))
        throw std::runtime_error("Failed to complete converter operations!");
}
