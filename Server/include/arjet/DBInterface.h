#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Semaphore.h>
#include <arjet/Timer.h>

#define uint unsigned long

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
	unsigned int rating;
	unsigned int views;
	unsigned int authorID;
	unsigned char permission;
	int64_t timestamp;
	uint64_t categories = 0;
	uint32_t wordcount = 0;
	std::string authorName;
};

struct User {
	uint32_t id;
	std::string username;
	unsigned char hash[32];
	uint32_t privilege;
	unsigned char salt[32];
	uint32_t bio; //0 is none, 1 is private, 2 is public
	User() {
		id = 0;
		username = "";
		uint32_t privilege = 0;
		bio = 0;
	}
};

struct UserClear{//Used for holding the user in cleartext
	//SHOULD NEVER BE WRITTEN TO STORAGE
	uint32_t id;
	std::string username;
	std::string password;
	uint32_t privilege;
	uint32_t bio;
};
class DBInterface {
public:
	
	DBInterface(const std::string& host, uint32_t port, const std::string& user, const std::string& pass, const std::string& dbName);
	
	int addRating(uint32_t storyID, uint32_t userID, int8_t rating); //returns response code eg 200, 409, 404
	int addView(uint32_t storyID, uint32_t userIP); //same

	vector <uint32_t> *topRated = NULL; //Copy only! Will change due to external factors
	//Also four billion should be enough

	Story pullStoryInfo(const uint32_t& id); //Pulls the db info for a single story by id

	User pullUser(const uint32_t& id);
	uint32_t findUser(const std::string& username); //searches for a user in database, returns id if found or 0 if not
	uint32_t addUser(const User& user); // returns ID upon success, 0 upon fail
	uint32_t updateUser(const User& user); // returns ID upon success, 0 upon fail

	bool updateStory(Story story);//returns true on success, false on fail
	bool updateStoryWordCount(uint id, uint count); //just updates the word count of a story
	uint32_t addStory(Story story);//returns id on success, 0 on fail

	void updateRating(uint32_t storyID);//recalculates average rating for a story
	void updateRatings();//recalculates average rating for all stories
	uint32_t findRating(uint32_t userID, uint32_t storyID);//returns the rating
	void sortTopRated(); //updates the topRated list stored in lists/top_rated.uil;
	vector<Story> pullTopRated(std::string where = "", uint32_t offset = 0, uint32_t limit = uint32_t(20)); //Pulls results from the database using the toprated list

	void updateViews();
	void sortMostViewed();
	vector<Story> pullMostViewed(std::string where = "", uint32_t offset = 0, uint32_t limit = uint32_t(20));

	void sortNewest();
	vector<Story> pullNewest(std::string where = "", uint32_t offset = 0, uint32_t limit = uint32_t(20));

	void RemoveSortedDuplicates();//Removes duplicate items from the sorted lists. IDK why, but this sometimes happens.

	vector<Story> pullUserStories(uint32_t id, uint32_t offset = 0, uint32_t limit = uint32_t(20), uint32_t permission = 1); //Pulls results from the database matching user ID
	~DBInterface();
private:
	Semaphore semaphore; 
	vector<Story> pullList(std::string path, std::string where, uint32_t offset, uint32_t limit, uint32_t permission = 1);
	void sortList(unsigned int column);//helper function for sorting. Inputs are the path to the uil and the db column used for the key
	uint32_t getNextIncrement();//Assumes threads have already been locked
};