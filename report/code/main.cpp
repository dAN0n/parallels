#include <iomanip>
#include <pthread.h>
#include <omp.h>
#include "utils.h"

using namespace std;

double startTime, endTime; // For timestamps
unordered_map<string, int> wordDict[3];

double defaultProcess(const string &text) {
	startTime = omp_get_wtime();
	pair<int, int> substring = getStringPart(text, 0, text.length() - 1);
	wordDict[0] = getWordDict(text, substring.first, substring.second);
	endTime = omp_get_wtime();

	return endTime - startTime;
}

double pthreadProcess(const string &text, int threadCount) {
	pthread_t *threads = new pthread_t[threadCount];
	getWordDictArgs args[threadCount];
	unordered_map<string, int> dicts[threadCount];

	int textBlock = text.length() / threadCount;
	int startIndex = 0;

	startTime = omp_get_wtime();
	for(int i = 0; i < threadCount; i++) {
		pair<int, int> substring;
		if(i != threadCount - 1){
			substring = getStringPart(text, startIndex, (i + 1) * textBlock - 1);
		} else {
			substring = getStringPart(text, startIndex, text.length() - 1);
		}
		startIndex = substring.second + 1;

		args[i].str = text;
		args[i].strBegin = substring.first;
		args[i].strEnd = substring.second;
	}
	
	for(int i = 0; i < threadCount; i++)
		pthread_create(&threads[i], NULL, getWordDict_pthread, (void *) &args[i]);

	for(int i = 0; i < threadCount; i++) pthread_join(threads[i], NULL);

	for(int i = 0; i < threadCount; i++) dicts[i] = args[i].dict;

	wordDict[1] = mergeDict(dicts, threadCount);
	endTime = omp_get_wtime();

	return endTime - startTime;
}

double ompProcess(const string &text, int threadCount) {
	getWordDictArgs args[threadCount];
	unordered_map<string, int> dicts[threadCount];

	int textBlock = text.length() / threadCount;
	int startIndex = 0;

	startTime = omp_get_wtime();
	for(int i = 0; i < threadCount; i++) {
		pair<int, int> substring;
		if(i != threadCount - 1){
			substring = getStringPart(text, startIndex, (i + 1) * textBlock - 1);
		} else {
			substring = getStringPart(text, startIndex, text.length() - 1);
		}
		startIndex = substring.second + 1;

		args[i].str = text;
		args[i].strBegin = substring.first;
		args[i].strEnd = substring.second;
	}
	
	omp_set_num_threads(threadCount);
	#pragma omp parallel for
		for(int i = 0; i < threadCount; i++) {
			dicts[i] = getWordDict(args[i].str, args[i].strBegin, args[i].strEnd);
		}

	wordDict[2] = mergeDict(dicts, threadCount);
	endTime = omp_get_wtime();

	return endTime - startTime;
}

void showDictsContent() {
	for(int i = 0; i < 3; i++) {
		cout << i << " RESULT________________________________" << endl;
		for(auto word: wordDict[i]) {
		    cout << left << setw(15) << word.first << "\t" << word.second << endl;
		}
		cout << endl;
	}
}

long getWordCount(unordered_map<string, int> dict) {
	long counter = 0;
	for(auto it: dict) counter += it.second;
	return counter;
}

int main(int argc, char* argv[]) {
	setlocale(LC_CTYPE, "Russian");

	string fileName;
	string text;

	if(argc > 1) fileName = argv[1];
	else {
		cout << "Usage: " << argv[0] << " <filename>" << endl;
		return 0;	
	}

	text = readFromFile(fileName);
	text = trimAndLowerString(text);
	if(text == "") {
		cout << "File read failed or text is empty!" << endl;
		return 1;
	}

	cout << endl << "Increase words with same thread count" << endl;
	cout << "[Default]  [Pthread]  [  OMP  ]      Words  Threads" << endl;
	double time[3];
	int threadCount = omp_get_num_procs();
	string str;
	long wordCount;
	for(int i = 0; i < 6; i++){
		str += (i == 0) ? text + " " : str + " ";
		time[0] = defaultProcess(str);
		time[1] = pthreadProcess(str, threadCount);
		time[2] = ompProcess(str, threadCount);
		wordCount = (i == 0) ? getWordCount(wordDict[0]) : wordCount * 2;

		cout << fixed << setw(9) << time[0] << "  " << fixed << setw(9) << time[1] << "  " << fixed << setw(9) << time[2] << "  " << fixed << setw(9) << wordCount << fixed << setw(9) << threadCount << endl;
	}

	cout << endl << "Increase threads with same words count" << endl;
	cout << "[Default]  [Pthread]  [  OMP  ]      Words  Threads" << endl;
	str = text;
	wordCount /= 32;
	for(int i = 0; i < 9; i++){
		threadCount = (i == 0) ? 1 : threadCount * 2;
		time[0] = defaultProcess(str);
		time[1] = pthreadProcess(str, threadCount);
		time[2] = ompProcess(str, threadCount);

		cout << fixed << setw(9) << time[0] << "  " << fixed << setw(9) << time[1] << "  " << fixed << setw(9) << time[2] << "  " << fixed << setw(9) << wordCount << fixed << setw(9) << threadCount << endl;
	}

	cout << endl << "Same threads count with same words count" << endl;
	cout << "[Default]  [Pthread]  [  OMP  ]      Words  Threads" << endl;
	for(int i = 0; i < 4; i++) str += str + " ";
	threadCount = 4;
	for(int i = 0; i < 10; i++){
		time[0] = defaultProcess(str);
		time[1] = pthreadProcess(str, threadCount);
		time[2] = ompProcess(str, threadCount);
		wordCount = (i == 0) ? getWordCount(wordDict[0]) : wordCount;

		cout << fixed << setw(9) << time[0] << "  " << fixed << setw(9) << time[1] << "  " << fixed << setw(9) << time[2] << "  " << fixed << setw(9) << wordCount << fixed << setw(9) << threadCount << endl;
	}

	// showDictsContent();
}