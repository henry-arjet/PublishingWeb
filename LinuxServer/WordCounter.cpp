#include <arjet/WordCounter.h>
#include <arjet/Timer.h>

using std::cout;
using std::endl;
uint WordCounter::countWordsFile(const string& name) {
	//Timer timer;
	//timer.Start();
	string filePath = workDir + name;
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cout << "failed to open file for word counting" << std::endl;
		std::cout << filePath << std::endl;
		return -1; //should be uint max I think?
	}
	//timer.Stop();
	//cout << "opening the file took " << timer.Results() << " miliseconds" << endl;
	//timer.Start();

	//read file into a string
	string contents;
	file.seekg(0, std::ios::end);
	contents.resize(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&contents[0], contents.size());
	file.close();
	//timer.Stop();
	//cout << "reading the file took " << timer.Results() << " miliseconds" << endl;
	//timer.Start();

	
	
	return countWords(contents);



}

uint WordCounter::countWords(const string& input) {

	//this algorithm finds chunks of the string without tags, and then when it encounters a tag,
	//copies the "good" tagless chars to the clean string
	//I came up with it myself, so there's probably a three line solution that does the same thing 5x as fast

	if (input.size() == 0) {
		std::cout << "file empty" << std::endl;
		return -1;
	}
	string cleanContents;//contents of the file without tags
	auto i = input.begin();

	auto startOfGood = input.cbegin();


	while (i <= input.end()) {
		if (*i == '<') { //encounter the begining of a tag
			if (startOfGood != i) { //make sure there is data to copy
				cleanContents.append(startOfGood, i); //copy the good data to the good string
			}
			++i;
		}
		else if (*i == '>') {
			startOfGood = ++i;
		}
		else if (*i == '&') { //remove &nbsp;
			if ((*(i + 1) == 'n') && (*(i + 5) == ';')) { //yeah I'm being lazy, but its just a word count
				cleanContents.append(startOfGood, i); //copy the good data to the good string
				i += 6; //move i past the &nbsp;
				startOfGood = i; //reset the good marker
			}
			else ++i;
		}
		else ++i;
	}
	//timer.Stop();
	//cout << "cleaning the string took " << timer.Results() << " miliseconds" << endl;
	//timer.Start();
	//std::cout << "The final string is\n\n" << cleanContents << "\n\n" << std::endl;

	//word count via state
	uint count = 0; //return value; number of words in the file
	bool white = true; //is the previous char whitespace?
	i = cleanContents.begin();
	while (i <= cleanContents.end()) {
		if (*i == ' ' || *i == '\n' || *i == '\t') {
			white = true;
		}
		else {
			if (white) {//if the previous char was whitespace and this isn't its the start of a word
				++count;
			}
			white = false;
		}
		++i;
	}
	//timer.Stop();
	//cout << "counting the words took " << timer.Results() << " miliseconds" << endl;
	cout << "I counted " << count << "words" << endl;
	return count;
}