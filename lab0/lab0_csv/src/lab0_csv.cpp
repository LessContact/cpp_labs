#include "wordCount.h"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Incorrect usage. Expeceted: word_count.exe INPUT_FILE.txt OUTPUT_FILE.csv\n";
        return 0;
    }

    if (IsBadFileExtensions(argv[1], argv[2])) {
        std::cerr << "It is highly advised to use files with proper extensions; INPUT_FILE.txt OUTPUT_FILE.csv";
        return 0;
    }

    //std::locale::global(std::locale("ru-RU").combine<std::numpunct<wchar_t>>(std::locale("C")));
    
    std::locale::global(std::locale("ru_RU.utf8").combine<std::numpunct<wchar_t>>(std::locale("C")));

    if (!OutputWordFrequencyToCSV(argv[1], argv[2]))
    {
        std::cerr << "Output to CSV failed.\n";
        return 0;
    }

    return 0;
}
