#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
class Scheema;

class DBInterface {
public:

	DBInterface(const std::string& host, uint32_t port, const std::string& user, const std::string& pass, const std::string& dbName);
	
	vector <uint32_t> *topRated = NULL; //Copy only! Will change due to external factors
	//Also four billion should be enough
	
	void sortTopRated(); //updates the topRated vector;

	~DBInterface();
};