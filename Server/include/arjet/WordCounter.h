#pragma once
//This class aims to gain a rough count of the rendered words from an html file
//It is not a singleton, which allows it to be used by multiple threads at once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::string;

class WordCounter
{
public:
	WordCounter() {}
	WordCounter(const string& inDir) { workDir = inDir; } //sets the working dir
	void setWorkDir(const string &inDir) { workDir = inDir; } //sets the working directory, should be either bio or stories
	uint countWordsFile(const string& name); //counts the words in a given story file and returns them
	uint countWords(const string& input); //counts the words in a given story string and returns them
private:
	string workDir;
};

