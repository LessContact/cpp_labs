#include "WavParser.h"

uint32_t WavParser::GetTrackDurationSeconds() const
{
    return header.subchunk2Size / (header.numChannels * header.bitsPerSample / 8) / header.sampleRate;
}

void WavParser::ReadWavHeader(WavHeader& outWaveHeader, std::ifstream& outSkippedHeaderStream)
{
    if (!outSkippedHeaderStream.is_open()) throw std::runtime_error(".wav file doesn't exist.");
    outSkippedHeaderStream.read(reinterpret_cast<char*>(&outWaveHeader), sizeof(outWaveHeader) - 8);

    if (outWaveHeader.audioFormat != 1) throw std::runtime_error("Non-PCM audio files are not supported.");

    if (outWaveHeader.sampleRate != 44100) throw std::runtime_error("Unsupported sample rate.");

    if (outWaveHeader.numChannels != 1) throw std::runtime_error("Num channels greater than 1.");

    // Skipping LIST info.
    while (!outSkippedHeaderStream.eof())
    {
        outSkippedHeaderStream.read(outWaveHeader.subchunk2Id, 4);
        const std::string curSubChunkString = std::string(outWaveHeader.subchunk2Id, 4);

        if (curSubChunkString == std::string("data"))
        {
            outSkippedHeaderStream.seekg(-4, std::ios::cur);
            outSkippedHeaderStream.read(reinterpret_cast<char*>(&outWaveHeader) + sizeof(outWaveHeader) - 8, 8);
            break;
        }

        outSkippedHeaderStream.read(reinterpret_cast<char*>(&outWaveHeader.subchunk2Size),
                                    sizeof(outWaveHeader.subchunk2Size));
        outSkippedHeaderStream.seekg(outWaveHeader.subchunk2Size, std::ios::cur);
    }

    if (std::string(outWaveHeader.subchunk2Id, 4) != std::string("data", 4))
        throw std::runtime_error(
            "Failed to find \"data\" block!");
}
