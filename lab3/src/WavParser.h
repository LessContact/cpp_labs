#pragma once
#include <cstdint>
#include <string>
#include <exception>
#include <stdexcept>
#include <fstream>

class WavParser{
public:
    struct WavHeader
    {
        char chunkId[4]; // "RIFF"
        uint32_t chunkSize; // size without chunkId and chunkSize

        char format[4]; // "WAVE"
        char subchunk1Id[4]; // "fmt "
        uint32_t subchunk1Size; // 16 for PCM format
        uint16_t audioFormat; // 1 for PCM
        uint16_t numChannels;
        uint32_t sampleRate; // in Hz

        // sampleRate * numChannels * bitsPerSample/8
        uint32_t byteRate;

        // numChannels * bitsPerSample/8
        // Bytes count for one sample, including all channels
        uint16_t blockAlign;
        uint16_t bitsPerSample;

        char subchunk2Id[4]; // "data"
        // numSamples * numChannels * bitsPerSample/8
        uint32_t subchunk2Size;
    };
    // explicit WavParser(std::string& nameFile);
    WavHeader header;
    WavParser() = default;
    void ReadWavHeader(WavHeader& outWaveHeader, std::ifstream& outSkippedHeaderStream);
    uint32_t GetTrackDurationSeconds() const;
};
