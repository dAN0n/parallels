#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>

struct getWordDictArgs {
    std::string str;
    unsigned int strBegin;
    unsigned int strEnd;
    std::unordered_map<std::string, int> dict;
};

enum cutType {SPACE_0, SPACE_N, WORD_0, WORD_N};

std::string readFromFile(std::string fileName);
std::string trimAndLowerString(std::string str);
std::pair<int, int> getStringPart(const std::string &str, unsigned int beginInd, unsigned int endInd);
unsigned int getCutIndex(const std::string &str, int pos, cutType type);
std::unordered_map<std::string, int> mergeDict(const std::unordered_map<std::string, int> dicts[], int size);
std::unordered_map<std::string, int> getWordDict(const std::string &str, unsigned int strBegin, unsigned int endInd);
void *getWordDict_pthread(void *args);