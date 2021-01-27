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
	void setWorkDir(const string &inDir) { workDir = inDir; } //sets the working directory, should be either bio or stories
	uint countWords(const string& name); //counts the words in a given story and returns them
private:
	string workDir;
};

