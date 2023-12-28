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

void MuteConverter::Process()
{
    if (IntervalBeginSeconds < 0) throw std::runtime_error("Invalid mute time interval.");
    if (IntervalEndSeconds < 0) throw std::runtime_error("Invalid mute time interval.");

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

const std::string MuteConverter::GetSyntaxInfoString() const
{
    return std::string("mute X Y");
}

const std::string MuteConverter::GetParametersInfoString() const
{
    return std::string("X - interval begin time in seconds, Y - interval end time in seconds.");
}

const std::string MuteConverter::GetFeaturesInfoString() const
{
    return std::string("output: from X to Y seconds is silence, everything else is the same as input.");
}

MixConverter::MixConverter(const std::string& inputFilePath, const std::string& outputFilePath,
                           const std::string& mixFilePath,
                           uint32_t insertTimeSeconds)
    : Converter(inputFilePath, outputFilePath), AdditionalStream(mixFilePath), InsertTimeSeconds(insertTimeSeconds)
{
}

void MixConverter::Process()
{
    if (InsertTimeSeconds < 0) throw std::runtime_error("Invalid mix insertion time.");

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

const std::string MixConverter::GetSyntaxInfoString() const
{
    return std::string("mix $X Y");
}

const std::string MixConverter::GetParametersInfoString() const
{
    return std::string("$X - additional stream to mix with, Y - insertion time in seconds.");
}

const std::string MixConverter::GetFeaturesInfoString() const
{
    return std::string(
        "output: mixed track with input number X beginning from time Y. length of output stream equals length of input stream. if additional "
        "stream's length > input length, then it's part is cut, otherwise equal to input.");
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
    if (IntervalBeginSeconds < 0) throw std::runtime_error("Invalid interval.");
    if (IntervalEndSeconds < 0) throw std::runtime_error("Invalid interval.");
    if (SpeedUpTimes < 1) throw std::runtime_error("Invalid SpeedUp Coefficient.");

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

    if (IntervalBeginSeconds >= IntervalEndSeconds)
        throw std::runtime_error("intervalBeginSeconds >= intervalEndSeconds.");

    WavParser inputWav = {};
    inputWav.ReadWavHeader(inputWav.header, inputTrack);

    const uint32_t inputTrackDurationSeconds = inputWav.GetTrackDurationSeconds();
    // UPDATE THE HEADER
    auto originalNumOfSamples = inputWav.header.subchunk2Size / (inputWav.header.numChannels * (inputWav.header.
        bitsPerSample / 8));
    auto originalInteravalSampleNum = inputWav.header.sampleRate * (IntervalEndSeconds -
        IntervalBeginSeconds);
    inputWav.header.subchunk2Size = /*NumSamples*/(originalNumOfSamples - (originalInteravalSampleNum - (inputWav.header
        .sampleRate * (IntervalEndSeconds -
            IntervalBeginSeconds) / SpeedUpTimes))) * inputWav.header.numChannels * inputWav.header.bitsPerSample / 8;

    // inputWav.header.subchunk2Size /= SpeedUpTimes;

    // inputWav.header.chunkSize = 4 + (8 + inputWav.header.subchunk1Size) + (8 + inputWav.header.subchunk2Size);
    inputWav.header.chunkSize = 36 + inputWav.header.subchunk2Size;
    // ----------------
    outTrack.write(reinterpret_cast<const char*>(&inputWav), sizeof(inputWav));

    std::vector<int16_t> inputTrackSamples;
    inputTrackSamples.resize(inputWav.header.sampleRate);


    if (IntervalEndSeconds > inputTrackDurationSeconds) IntervalEndSeconds = inputTrackDurationSeconds;

    for (uint32_t elapsedSeconds = 0; elapsedSeconds < inputTrackDurationSeconds; ++elapsedSeconds)
    {
        if (inputTrack.eof()) break;

        inputTrack.read(reinterpret_cast<char*>(inputTrackSamples.data()),
                        inputTrackSamples.size() * sizeof(inputTrackSamples[0]));

        std::vector<int16_t> spedUpSamplesOneSecond(
            inputTrackSamples.size() * sizeof(inputTrackSamples[0]) / SpeedUpTimes, 0);

        if (elapsedSeconds >= IntervalBeginSeconds && elapsedSeconds < IntervalEndSeconds)
        {
            for (uint32_t iter = 0; iter <= inputWav.header.sampleRate; iter += SpeedUpTimes)
            {
                int64_t spedUpSample = 0;

                for (int32_t i = 0; i < SpeedUpTimes; ++i)
                {
                    spedUpSample += inputTrackSamples[iter + i];
                }
                int16_t sampleToWrite = static_cast<int16_t>(spedUpSample / static_cast<int64_t>(SpeedUpTimes));
                spedUpSamplesOneSecond[iter / SpeedUpTimes] = sampleToWrite;
            }

            outTrack.write(reinterpret_cast<const char*>(spedUpSamplesOneSecond.data()),
                           spedUpSamplesOneSecond.size());
        }
        else
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

const std::string SpeedUpConverter::GetSyntaxInfoString() const
{
    return std::string("speedup X Y");
}

const std::string SpeedUpConverter::GetParametersInfoString() const
{
    return std::string("X - interval begin time in seconds, Y - interval end time in seconds.");
}

const std::string SpeedUpConverter::GetFeaturesInfoString() const
{
    return std::string(
        "output: from X to Y seconds is sped up by Y times, everything else is the same as input.\nNOTE: Time interval is relative to the original file, so if you do from 2 to 5 seconds, in the output it will be from 2 to 3.5 seconds!");
}
