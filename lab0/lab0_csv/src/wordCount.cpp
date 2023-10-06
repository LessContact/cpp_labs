#include "wordCount.h"

bool IsBadFileExtensions(char* inputName, char* outputName) {
	std::string in(inputName);
	std::string out(outputName);
	if (in.ends_with(".txt") || out.ends_with(".csv")) {
		return false;
	}
	return true;
}

bool OutputWordFrequencyToCSV(char* inputFileName, char* outputFileName) {

    std::wifstream in(inputFileName);
    if (!in.is_open())
    {
        std::cerr << "Failed to open: " << inputFileName << '\n' << "File likely doesn't exist\n";
        return false;
    }

    std::wofstream out(outputFileName);
    if (!out.is_open())
    {
        std::cerr << "Failed to open: " << outputFileName << '\n';
        in.close();
        return false;
    }

    std::map<std::wstring, unsigned int> word_TimesEncountered_Map;
    unsigned int totalWordCount = 0;  // Used for counting percentage.
    {
        std::wstring word;
        while (/*!in.eof()*/true)
        {
            std::wstring line;
            std::getline(in, line);

            /*std::wstring word;*/

            for (wchar_t character : line) { // building the word a letter at a time
                if (std::isalnum(character, std::locale()) || character == '\'') { // everything is a delimiter except " ' " and alphanumerical
                    word += std::tolower(character, std::locale());
                }
                else if (!word.empty()) {
                    ++word_TimesEncountered_Map[word];
                    ++totalWordCount;
                    word = L"";
                }
            }
            if (in.eof()) {
                if (!word.empty()) {
                    ++word_TimesEncountered_Map[word];
                    ++totalWordCount;
                }
                break;
            }
        }
    }
    // its a vector with a pair so like kinda [[word,123], ...]
    std::vector<std::pair<std::wstring, unsigned int>> sortedWords(word_TimesEncountered_Map.begin(), word_TimesEncountered_Map.end());
    std::sort(sortedWords.begin(), sortedWords.end(), [](const auto& left, const auto& right) { return left.second > right.second; }); // sorts in descending order

    out << "word" << ",frequency" << ",%ofAllWords" << std::endl;
    for (auto& [word, frequency] : sortedWords)
    {
        out << word << ',' << frequency << ',' << (static_cast<float>(frequency) / static_cast<float>(totalWordCount)) * 100.0f
            << std::endl;
    }

    in.close();
    out.close();
    return true;

}