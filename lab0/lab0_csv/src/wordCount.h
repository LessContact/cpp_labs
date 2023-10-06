#pragma once
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <locale>
#include <iomanip>
#include <fstream>

bool OutputWordFrequencyToCSV(char* input_filename, char* output_filename);
bool IsBadFileExtensions(char* inputName, char* outputName);

//class WordCounter {
//public:
//    bool OutputWordFrequencyToCSV(char* input_filename, char* output_filename);
//    bool CheckFileExtensions(char* inputName, char* outputName);
//};
