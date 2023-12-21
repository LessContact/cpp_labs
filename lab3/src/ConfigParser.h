#pragma once

#include <memory>
#include <queue>
#include <string>
#include <fstream>
#include "WavParser.h"

class Converter
{
public:
    Converter(const std::string& inputFilePath, const std::string& outputFilePath)
        : InputFilePath(inputFilePath), OutputFilePath(outputFilePath)
    {
    }

    Converter() = default;
    virtual ~Converter() = default;

    virtual void Process() = 0;

protected:
    std::string InputFilePath;
    std::string OutputFilePath;
};

class MuteConverter final : public Converter
{
public:
    MuteConverter(const std::string& inputFilePath, const std::string& outputFilePath,
                  const uint32_t intervalBeginSeconds,
                  const uint32_t intervalEndSeconds);
    ~MuteConverter() override = default;

    void Process() final;

private:
    uint32_t IntervalBeginSeconds = 0;
    uint32_t IntervalEndSeconds = 0;
};

class MixConverter final : public Converter
{
public:
    MixConverter(const std::string& inputFilePath, const std::string& outputFilePath, const std::string& mixFilePath,
                 uint32_t insertTimeSeconds = 0);
    ~MixConverter() override = default;

    void Process() final;

private:
    std::string AdditionalStream;
    uint32_t InsertTimeSeconds = 0;
};

class SpeedUpConverter final : public Converter
{
public:
    SpeedUpConverter(const std::string& inputFilePath, const std::string& outputFilePath,
                     const uint32_t intervalBeginSeconds,
                     const uint32_t intervalEndSeconds, const uint32_t SpeedUpTimes);
    ~SpeedUpConverter() override = default;

    void Process() final;

private:
    uint32_t IntervalBeginSeconds = 0;
    uint32_t IntervalEndSeconds = 0;
    uint32_t SpeedUpTimes = 0;
};

namespace ConfigParser
{
    std::queue<std::shared_ptr<Converter>> GetConvertersFromConfig(const std::string& configPath,
                                                                   std::vector<std::string>& inputs,
                                                                   int32_t inputCount);
}


namespace ConverterFactory
{
    template <typename T, typename... Args>
    static __forceinline std::shared_ptr<Converter> Create(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
};
