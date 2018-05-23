#include "utils.h"

std::string readFromFile(std::string fileName) {
	std::string str;
	std::ifstream fin(fileName.c_str());

	if(fin.is_open()) {
		str.assign(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
		fin.close();

		return str;
	} else return "";
}

std::string trimAndLowerString(std::string str) {
	int strBegin = getCutIndex(str, 0, SPACE_0);
	int strLength = getCutIndex(str, str.length() - 1, SPACE_N) - strBegin + 1;
	str = str.substr(strBegin, strLength);
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

// beginInd must be at word start or delimiter
std::pair<int, int> getStringPart(const std::string &str, unsigned int beginInd, unsigned int endInd) {
	std::pair<int, int> ind;

	// cut delimiter if needed from start
	ind.first = (getCutIndex(str, beginInd, WORD_0) == beginInd) ? getCutIndex(str, beginInd, SPACE_0) : beginInd;
	// cut delimiter if needed or restore cut word from end
	ind.second = (getCutIndex(str, endInd, WORD_N) == endInd) ? getCutIndex(str, endInd, SPACE_N) : getCutIndex(str, endInd, WORD_0) - 1;

	return ind;	
}

unsigned int getCutIndex(const std::string &str, int pos, cutType type) {
	std::string delim = " .,!?&@():;-|\"\r\n";
	switch(type) {
		case(SPACE_0):
			return str.find_first_not_of(delim, pos);
		case(SPACE_N):
			return str.find_last_not_of(delim, pos);
		case(WORD_0):
			return str.find_first_of(delim, pos);
		case(WORD_N):
			return str.find_last_of(delim, pos);
		default:
			return -1;
	}
}

std::unordered_map<std::string, int> mergeDict(const std::unordered_map<std::string, int> dicts[], int size){
	std::unordered_map<std::string, int> mergedDict;

	for(int i = 0; i < size; i++) {
		for(auto it: dicts[i]) mergedDict[it.first] += it.second;
	}

	return mergedDict;
}

std::unordered_map<std::string, int> getWordDict(const std::string &str, unsigned int strBegin, unsigned int strEnd) {
	std::unordered_map<std::string, int> dict;

	while((strBegin = getCutIndex(str, strBegin, SPACE_0)) < strEnd){
		int cutEnd = getCutIndex(str, strBegin, WORD_0);
		int strLength = cutEnd - strBegin;
		std::string temp = str.substr(strBegin, strLength);

		strBegin = getCutIndex(str, cutEnd, SPACE_0);
		dict[temp] += 1;
	}

	return dict;
}

void *getWordDict_pthread(void *args) {
	getWordDictArgs *arg = (getWordDictArgs *)args; 
	std::unordered_map<std::string, int> dict;

	if(!arg->str.empty()) {
		while((arg->strBegin = getCutIndex(arg->str, arg->strBegin, SPACE_0)) < arg->strEnd){
			int cutEnd = getCutIndex(arg->str, arg->strBegin, WORD_0);
			int strLength = cutEnd - arg->strBegin;
			std::string temp = arg->str.substr(arg->strBegin, strLength);

			arg->strBegin = getCutIndex(arg->str, cutEnd, SPACE_0);
			dict[temp] += 1;
		}
	}

	arg->dict = dict;
	return 0;
}