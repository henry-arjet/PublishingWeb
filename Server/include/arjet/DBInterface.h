#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Semaphore.h>


using std::cout;
using std::endl;
using std::vector;
using std::wstring;
using std::wcout;
class Scheema;
struct Story {
	uint32_t id;
	std::string title;
	std::string path;
	uint32_t rating;
	uint32_t views;
	uint32_t authorID;
	unsigned char permission;
};
struct User {
	uint32_t id;
	std::string username;
	unsigned char* hash;
	uint32_t privilege;
	unsigned char* salt;
	User() {
		id = 0;
		username = "";
		hash = static_cast<unsigned char*>(malloc(32));
		salt = static_cast<unsigned char*>(malloc(32));
		uint32_t privilege = 0;
	}
	~User() {
		free(hash);
		free(salt);
	}
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
	uint32_t addUser(const User& user); // returns ID upon success, 0 upon fail

	bool updateStory(Story story);//returns true on success, false on fail
	bool addStory(Story story);//returns true on success, false on fail

	void sortTopRated(); //updates the topRated list stored in lists/top_rated.uil;
	vector<Story> pullTopRated(std::string where = "", uint32_t offset = 0, uint32_t limit = uint32_t(20)); //Pulls results from the database using the toprated list
	~DBInterface();

	void sortMostViewed();
	vector<Story> pullMostViewed(std::string where = "", uint32_t offset = 0, uint32_t limit = uint32_t(20));

	vector<Story> pullUserStories(uint32_t id, uint32_t offset = 0, uint32_t limit = uint32_t(20)); //Pulls results from the database matching user ID
private:
	Semaphore semaphore; 
	vector<Story> pullList(std::string path, std::string where, uint32_t offset, uint32_t limit);
	void sortList(const std::string& path, unsigned int column);//helper function for sorting. Inputs are the path to the uil and the db column used for the key
};