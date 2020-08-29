#include <arjet/DBInterface.h>

#include <mysqlx/xdevapi.h>
using namespace mysqlx;

typedef uint32_t uint;
Session* sesh;
Schema* db;
DBInterface::DBInterface(const std::string& host, uint32_t port, const std::string& user, const std::string& pass, const std::string& dbName) {
	cout << "Creating session on " << host << ":" << port <<" for " << user << "..." << endl;
	try {
		sesh = new Session(host, port, user, pass, dbName);

		db = new Schema(sesh->getDefaultSchema()); 
							
	}
	catch (const mysqlx::Error& err)
	{
		cout << "ERROR: " << err << endl;
		throw;
	}
	catch (std::exception& ex)
	{
		cout << "STD EXCEPTION: " << ex.what() << endl;
		throw;
	}
	catch (const char* ex)
	{
		cout << "EXCEPTION: " << ex << endl;
		throw;
	}
	cout << "Done" << endl;
}

struct sortObject { //key/value uints
	uint32_t id;
	uint32_t key;
};

void insertionSort(vector <sortObject>& objects) { //modifies the input vector
	int i, j;
	for (i = 1; i < objects.size(); i++) {
		sortObject store = objects[i];
		j = i - 1;

		while (j >= 0 && objects[j].key < store.key) {
			objects[j + 1] = objects[j];
			j--;
		}
		objects[j + 1] = store;
	}
}

User DBInterface::pullUser(const uint32_t& id) {
	Table table = db->getTable("users");
	RowResult res = table.select().where("id = '" + std::to_string(id) + "'").execute();

	Row row = res.fetchOne();
	User ret = User();
	ret.id = id;
	ret.username = (std::string)row.get(1);
	memcpy(ret.hash, row.get(2).getRawBytes().begin(), 32);
	memcpy(ret.salt, row.get(3).getRawBytes().begin(), 32);
	ret.privilege = row.get(4);
	return ret;
}

uint32_t DBInterface::findUser(const std::string& username) {
	uint ret = 0; //returns as 0 if not found
	Table table = db->getTable("users");
	RowResult res;
	res = table.select().where("username = '" + username + "'").execute();
	if (res.count() > 0) {
		ret = res.fetchOne().get(0);
	}
	return ret;
}

uint32_t DBInterface::addUser(const User& user) {
	Table table = db->getTable("users");
	try{
		table.insert().values(Value(), user.username, bytes(user.hash, 32), bytes(user.salt, 32), user.privilege).execute();
		//now to find the last inserted id
		Row row = table.select("LAST_INSERT_ID()").execute().fetchOne();
		cout << "LAST_INSERT_ID: " << row.get(0) << endl;
		return row.get(0);
	}catch (const mysqlx::Error& err) {
		cout << "ERROR: " << err << endl;
		return 0;
	}catch (std::exception& ex) {
		cout << "STD EXCEPTION: " << ex.what() << endl;
		return 0;
	}catch (const char* ex) {
		cout << "EXCEPTION: " << ex << endl;
		return 0;
	}
}


bool DBInterface::updateStory(Story story){
	Table table = db->getTable("test1");
	try {
		std::string s = "id = " + std::to_string(story.id);
		table.update().set("title", story.title).set("path", story.path).set("rating", story.rating)
			.set("views", story.views).set("author_id", story.authorID).set("permission", story.permission)
			.where(s).execute(); //reset the hit detector
		return true;
	}

	catch (const mysqlx::Error& err)
	{
		cout << "ERROR: " << err << endl;
		return false;
	}
	catch (std::exception& ex)
	{
		cout << "STD EXCEPTION: " << ex.what() << endl;
		return false;
	}
	catch (const char* ex)
	{
		cout << "EXCEPTION: " << ex << endl;
		return false;
	}
}

bool DBInterface::addStory(Story story) {
	Table table = db->getTable("test1");
	try {
		table.insert().values(Value(), story.title, story.path, story.rating, story.views, 0, story.authorID, story.permission).execute();
		return true;
	}

	catch (const mysqlx::Error& err){
		cout << "ERROR: " << err << endl;
		return false;
	}
	catch (std::exception& ex){
		cout << "STD EXCEPTION: " << ex.what() << endl;
		return false;
	}
	catch (const char* ex){
		cout << "EXCEPTION: " << ex << endl;
		return false;
	}
}

void DBInterface::sortList(const std::string& path, unsigned int column) {
	//read current list of top rated
	std::fstream file;

	file.open(path, std::ios::ate | std::ios::binary | std::ios::in); //ios::app so we can use tellp
	if (!file.is_open()) { cout << "Unable to open file" << endl; return; }
	auto size = file.tellg();
	file.seekg(0, std::ios::beg);
	auto sizeUint = size / 4;
	assert(size % 4 == 0);//cause if not the file is messed up. 4 bytes is one uint
	vector<uint> readList(sizeUint); //vector to hold the uints

	if (size) {//read the file if not empty
		vector<char> buffer(size);
		file.read(buffer.data(), size);
		memcpy(readList.data(), buffer.data(), size);
	}
	file.close();

	//fill sortList from database using the list from the file
	vector <sortObject> sortList(readList.size());

	Table table = db->getTable("test1");

	for (int i = 0; i < readList.size(); i++) {

		RowResult res = table.select().where("id = " + std::to_string(readList[i])).execute();
		Row row = res.fetchOne();

		sortList[i].id = readList[i]; //since the id is what we selected by
		sortList[i].key = row.get(column); //gets rating from 0-50000 (mapped to 0.0-5.0 client side)
	}

	//and tag all the ones that I just got
	for (int i = 0; i < readList.size(); i++) { //there's probably a better way
		table.update().set("hit", 1).where("id = " + std::to_string(readList[i])).execute();
	}

	//pull unlisted new entries AT END
	//These will be the entries that haven't previously been sorted
	try {
		table.select().execute();
		//RowResult remainers = table.select().where("hit = false").execute();
	}
	catch (const mysqlx::Error& err) {
		cout << "ERROR: " << err << endl;
		throw;
	}
	catch (std::exception& ex) {
		cout << "STD EXCEPTION: " << ex.what() << endl;
		throw;
	}
	catch (const char* ex) {
		cout << "EXCEPTION: " << ex << endl;
		throw;
	}

	RowResult remainers = table.select().where("hit = 0").execute();
	auto rCount = remainers.count();
	sortList.resize(sortList.size() + rCount); //adjust size to take the remainers into account

	for (int i = readList.size(); i < rCount + readList.size(); i++) { //add the remainers to the sortList
		Row row = remainers.fetchOne();
		try {
			sortList[i].id = row.get(0);
		}
		catch (const mysqlx::Error& err) {
			cout << "ERROR: " << err << endl;
			throw;
		}
		catch (std::exception& ex) {
			cout << "STD EXCEPTION: " << ex.what() << endl;
			throw;
		}
		catch (const char* ex) {
			cout << "EXCEPTION: " << ex << endl;
			throw;
		}
		sortList[i].key = row.get(column);
	}

	//sort that vector via insertionSort
	insertionSort(sortList);

	//save list
	file.open(path, std::ios::trunc | std::ios::out | std::ios::binary);
	if (!file.is_open()) { cout << "Unable to open file" << endl; return; }
	auto i = file.tellp(); //should be 0
	for (int i = 0; i < sortList.size(); i++) {
		file.write((char*)&sortList[i].id, 4);
	}
	file.close();
	table.update().set("hit", false).execute(); //reset the hit detector
}

void DBInterface::sortMostViewed() {
	sortList("../lists/most_viewed.uil", 4);

}

void DBInterface::sortTopRated() {
	sortList("../lists/top_rated.uil", 3);
}

vector<Story> pullList(std::string path, std::string where, uint32_t offset, uint32_t limit) {
	//File stuff
	std::fstream file;

	file.open(path, std::ios::ate | std::ios::binary | std::ios::in); //ios::app so we can use tellp
	if (!file.is_open()) { cout << "Unable to open file" << endl; vector<Story> fail; return fail; }
	auto size = file.tellg();
	file.seekg(0, std::ios::beg);
	auto sizeUint = size / 4;
	assert(size % 4 == 0);//cause if not the file is messed up. 4 bytes is one uint32
	vector<uint> readList(sizeUint); //vector to hold the uints
	if (size) {//read the file if not empty
		vector<char> buffer(size);
		file.read(buffer.data(), size);
		memcpy(readList.data(), buffer.data(), size);
	}
	file.close();

	//if (where.length()) {
	//	where = " AND " + where; //Don't want to have and if we're not using a where
	//}
	vector<Story> ret;
	ret.reserve(limit); //want to make sure we don't have empty values at the end if there aren't enough results. Thus we use reserve and pushBack

	Table table = db->getTable("test1");
	uint rSize = readList.size();
	for (uint i = 0; i < limit; i++) {//for each item in the top rated list
		if (i + offset >= rSize) {//break the loop if we've run out of items in the list
			break;
		}
		RowResult res = table.select().where("id = " + std::to_string(readList[i + offset])).execute();
		if (res.count()) { //If the catagories match and therefore we got a hit
			Row row = res.fetchOne();
			Story story = { row.get(0), (std::string)row.get(1), (std::string)row.get(2), row.get(3), row.get(4), row.get(6), (unsigned int)row.get(7) };//add result to return vector
			ret.push_back(story);
		}
		else limit++;//we didn't add a value, so we'll add one more to the limit
	}

	return ret;
}

vector<Story> DBInterface::pullMostViewed(std::string where, uint32_t offset, uint32_t limit) {
	return pullList("../lists/most_viewed.uil", where, offset, limit);
} 

Story DBInterface::pullStoryInfo(const uint32_t& id) {
	Story ret = {};
	Table table = db->getTable("test1");
	string qString = "id = " + std::to_string(id);
	RowResult res = table.select().where(qString).execute();
	if (!res.count())cout << "WARNING: ID QUERY RETURNED NO RESULTS" << endl;
	if (res.count() > 1)cout << "WARNING: ID QUERY RETURNED MULTIPLE RESULTS" << endl;

	Row row = res.fetchOne();
	ret.id = id;
	ret.title = (std::string)row.get(1);
	ret.path = (std::string)row.get(2);
	ret.rating = row.get(3);
	ret.views = row.get(4);
	ret.authorID = row.get(6);
	ret.permission = (unsigned int)row.get(7);

	return ret;

}

vector<Story> DBInterface::pullTopRated(std::string where, uint32_t offset, uint32_t limit) {
	return pullList("../lists/top_rated.uil", where, offset, limit);
}

vector<Story> DBInterface::pullUserStories(uint32_t id, uint32_t offset, uint32_t limit){
	

	vector<Story> ret;
	ret.reserve(limit); //want to make sure we don't have empty values at the end if there aren't enough results. Thus we use reserve and pushBack

	Table table = db->getTable("test1");
	
	RowResult res = table.select().where("author_id = " + std::to_string(id)).execute();
	if (res.count()) { //If the catagories match and therefore we got a hit
		
		Row row = res.fetchOne();
		while (row) {
			Story story = { row.get(0), (std::string)row.get(1), (std::string)row.get(2), row.get(3), row.get(4), row.get(6), (unsigned int)row.get(7) };//add result to return vector
			ret.push_back(story);
			row = res.fetchOne();
		}
	}


	return ret;
}

DBInterface::~DBInterface() {
	delete db;
	delete sesh;
}