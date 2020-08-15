#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
class Scheema;
struct Story {
	uint32_t id;
	std::string title;
	std::string path;
	uint32_t rating;
	uint32_t views;
};
struct User {
	uint32_t id;
	std::string username;
	unsigned char* hash;
	uint32_t privilege;
	unsigned char* salt;
};
struct UserClear{//Used for holding the user in cleartext
	//SHOULD NEVER BE WRITTEN TO STORAGE
	uint32_t id;
	std::string username;
	std::string password;
	uint32_t privilege;
};
class DBInterface {
public:
	
	DBInterface(const std::string& host, uint32_t port, const std::string& user, const std::string& pass, const std::string& dbName);
	
	vector <uint32_t> *topRated = NULL; //Copy only! Will change due to external factors
	//Also four billion should be enough

	Story pullStoryInfo(const uint32_t& id); //Pulls the db info for a single story by id

	User pullUser(const uint32_t& id);
	uint32_t findUser(const std::string& username); //searches for a user in database, returns id if found or 0 if not
	bool addUser(const User& user);

	bool updateStory(Story story);//returns true on success, false on fail
	bool addStory(Story story);//returns true on success, false on fail

	void sortTopRated(); //updates the topRated list stored in lists/top_rated.uil;
	vector<Story> pullTopRated(std::string where = "", uint32_t offset = 0, uint32_t limit = uint32_t(20)); //Pulls results from the database using the toprated list
	~DBInterface();

	void sortMostViewed();
	vector<Story> pullMostViewed(std::string where = "", uint32_t offset = 0, uint32_t limit = uint32_t(20));

private:
	void sortList(const std::string& path, unsigned int column);//helper function for sorting. Inputs are the path to the uil and the db column used for the key
};