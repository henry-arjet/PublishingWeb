#include <arjet/DBInterface.h>
#include <mysqlx/xdevapi.h>
using namespace mysqlx;

Session* sesh;
Schema* db;
DBInterface::DBInterface(const std::string& host, uint32_t port, const std::string& user, const std::string& pass, const std::string& dbName) {
	try {
		cout << "Creating session for " << user << '@' << host << ':' << port << endl;
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
	semaphore.notify(); //sets the initial value of the semaphore to 1. It should never go above this.
}

struct sortObject { //key/value uints
	uint32_t id;
	int64_t key;
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
	semaphore.wait();
	RowResult res = table.select().where("id = '" + std::to_string(id) + "'").execute();
	semaphore.notify();
	if (res.count() == 0){//if we failed to find the user
		cout << "ID " << id << " is invalid" << endl;
		User ret;
		ret.id = 0; //to signal that this is an invalid user
		return ret;
	}

	Row row = res.fetchOne();
	User ret;
	ret.id = id;
	ret.username = (std::string)row.get(1);
	memcpy(ret.hash, row.get(2).getRawBytes().begin(), 32);
	memcpy(ret.salt, row.get(3).getRawBytes().begin(), 32);
	ret.privilege = row.get(4);
	return ret;
}

uint32_t DBInterface::findUser(const std::string& username) {
	uint32_t ret = 0; //returns as 0 if not found
	Table table = db->getTable("users");
	RowResult res;
	semaphore.wait();
	res = table.select().where("username = '" + username + "'").execute();
	semaphore.notify();
	if (res.count() > 0) {
		ret = res.fetchOne().get(0);
	}
	return ret;
}

uint32_t DBInterface::addUser(const User& user) {
	Table table = db->getTable("users");
	try{
		semaphore.wait();
		table.insert().values(Value(), user.username, bytes(user.hash, 32), bytes(user.salt, 32), user.privilege).execute();
		//now to find the last inserted id
		Row row = table.select("LAST_INSERT_ID()").execute().fetchOne();
		semaphore.notify();
		cout << "LAST_INSERT_ID: " << row.get(0) << endl;
		return row.get(0);
	}catch (const mysqlx::Error& err) {
		semaphore.notify();
		cout << "ERROR: " << err << endl;
		return 0;
	}catch (std::exception& ex) {
		semaphore.notify();
		cout << "STD EXCEPTION: " << ex.what() << endl;
		return 0;
	}catch (const char* ex) {
		semaphore.notify();
		cout << "EXCEPTION: " << ex << endl;
		return 0;
	}
}


bool DBInterface::updateStory(Story story){
	Table table = db->getTable("test1");
	try {
		std::string s = "id = " + std::to_string(story.id);
		semaphore.wait();
		table.update().set("title", story.title).set("path", story.path).set("rating", story.rating)
			.set("views", story.views).set("author_id", story.authorID).set("permission", story.permission)
			.where(s).execute(); //reset the hit detector
		semaphore.notify();
		return true;
	}

	catch (const mysqlx::Error& err)
	{
		semaphore.notify();
		cout << "ERROR: " << err << endl;
		return false;
	}
	catch (std::exception& ex)
	{
		semaphore.notify();
		cout << "STD EXCEPTION: " << ex.what() << endl;
		return false;
	}
	catch (const char* ex)
	{
		semaphore.notify();
		cout << "EXCEPTION: " << ex << endl;
		return false;
	}
}

uint32_t DBInterface::addStory(Story story) {
	Table table = db->getTable("test1");
	try {
		if (story.path == "") {//This means it's a auto-created story, most likely from a user
			story.views = 0;
			story.permission = 2;
			story.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			semaphore.wait();
			table.insert().values(Value(), story.title, story.path, 0, story.views, 0, story.authorID, story.permission, story.timestamp).execute();
			story.id = table.select("LAST_INSERT_ID()").execute().fetchOne().get(0);
			story.path = "stories/" + std::to_string(story.id) + ".html";
			table.update().set("path", story.path).where("id = " + std::to_string(story.id)).execute();
			semaphore.notify(); //need to keep the db locked for a while for this to work
			return story.id;

		}
		else {
			semaphore.wait();
			table.insert().values(Value(), story.title, story.path, story.rating, story.views, 0, story.authorID, story.permission, story.timestamp).execute();
			RowResult res = table.select("LAST_INSERT_ID()").execute();
			semaphore.notify();
			return res.fetchOne().get(0);
		}
	}

	catch (const mysqlx::Error& err){
		semaphore.notify();
		cout << "ERROR: " << err << endl;
		return false;
	}
	catch (std::exception& ex){
		semaphore.notify();
		cout << "STD EXCEPTION: " << ex.what() << endl;
		return false;
	}
	catch (const char* ex){
		semaphore.notify();
		cout << "EXCEPTION: " << ex << endl;
		return false;
	}
}


void DBInterface::updateRating(uint32_t storyID) {
	semaphore.wait();
	sesh->sql("UPDATE test1 "
		"INNER JOIN(SELECT "
			"test1.id, AVG(ratings.rating) * 1000 AS rating_average "
		//ratings are stored as uint8 0-50, average ratings are stored as uint16 0-50,000.
		"FROM test1 "
		"INNER JOIN "
		"ratings "
		"ON test1.id = ratings.story_id "
		"group by test1.id) "
		"as b on test1.id = b.id "
		"SET "
		"test1.rating = b.rating_average "
		"WHERE test1.id = " + std::to_string(storyID)).execute();
	semaphore.notify();

}
void DBInterface::updateRatings() {

	semaphore.wait();
	try{
	sesh->sql(
		"UPDATE test1 "
		"INNER JOIN(SELECT "
			"test1.id, AVG(ratings.rating) * 1000 AS rating_average "
			//ratings are stored as uint8 0-50, average ratings are stored as uint16 0-50,000.
			"FROM test1 "
			"INNER JOIN "
			"ratings "
			"ON test1.id = ratings.story_id "
			"group by test1.id) "
		"as b on test1.id = b.id "
		"SET "
		"test1.rating = b.rating_average;").execute();
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
	semaphore.notify();
}

void DBInterface::updateViews() {
	semaphore.wait();
	sesh->sql(
		"UPDATE test1 "
		"INNER JOIN(SELECT "
		"test1.id, COUNT(views.ipv4) hits "
		"FROM test1 "
		"INNER JOIN "
		"views "
		"ON test1.id = views.story_id "
		"group by test1.id) "
		"as b on test1.id = b.id "
		"SET "
		"test1.views = b.hits;").execute();
	semaphore.notify();
}

uint32_t DBInterface::findRating(uint32_t userID, uint32_t storyID) {
	Table table = db->getTable("ratings");
	semaphore.wait();
	RowResult res = table.select("rating").where("story_id = " + std::to_string(storyID) + " AND user_id = " + std::to_string(userID)).execute();
	semaphore.notify();
	if (!res.count()) {
		return 0; //null/no rating found
	}
	Row row = res.fetchOne();
	return row.get(0);
}


int DBInterface::addRating(uint32_t storyID, uint32_t userID, int8_t rating) {
	Table table = db->getTable("ratings");

	semaphore.wait();
	RowResult res = table.select("id").where("user_id = " + std::to_string(userID) + " AND story_id = " + std::to_string(storyID)).execute();
	semaphore.notify();

	if (res.count()) {
		//updateRating(storyID);
		return 409;
	}

	semaphore.wait();
	table.insert().values(Value(), storyID, userID, rating).execute();
	semaphore.notify();

	return 200;
}
int DBInterface::addView(uint32_t storyID, uint32_t userIP) {
	Table table = db->getTable("views");

	semaphore.wait();
	RowResult res = table.select("ipv4").where("ipv4 = " + std::to_string(userIP) + " AND story_id = " + std::to_string(storyID)).execute();
	semaphore.notify();
	if (res.count()) {
		return 409;
	}

	semaphore.wait();
	table.insert().values(storyID, userIP).execute();
	semaphore.notify();

	return 200;
}

void DBInterface::RemoveSortedDuplicates() {

}

void DBInterface::sortList(unsigned int column) {
	std::string keyString;
	std::string tableString;
	if (column == 2) {
		keyString = "stamp";
		tableString = "newest";
	}
	else if (column == 3) {
		keyString = "rating";
		tableString = "toprated";
	}
	else if (column == 4) {
		keyString = "views";
		tableString = "mostviewed";
	}
	else {
		cout << "invalid column" << endl;
		return;
	}

	RowResult res;
	semaphore.wait();
	try {
		res = sesh->sql("SELECT " + tableString + ".id, test1." + keyString +
			" FROM " + tableString +
			" INNER JOIN test1 ON " + tableString + ".id = test1.id"
			" ORDER BY ord; ").execute();
		semaphore.notify();
	}
	catch (const mysqlx::Error& err) {
		semaphore.notify();
		cout << "ERROR: " << err << endl;
		throw;
	}
	catch (std::exception& ex) {
		semaphore.notify();
		cout << "STD EXCEPTION: " << ex.what() << endl;
		throw;
	}
	catch (const char* ex) {
		semaphore.notify();
		cout << "EXCEPTION: " << ex << endl;
		throw;
	}
	vector <sortObject> sortList(res.count());

	int i = 0;
	while (res.count()) {
		Row row = res.fetchOne();

		sortList[i].id = row.get(0);

		sortList[i].key = row.get(1); //rating or views
		i++;
	}

	//and tag all the ones that I just got
	semaphore.wait();

	res = sesh->sql("UPDATE test1 "
		"INNER JOIN " + tableString + " ON " + tableString + ".id = test1.id "
		"SET hit = 1;"
	).execute();

	semaphore.notify();

	//timer.Stop();
	//cout << "updating hits took " << timer.Results() << " miliseconds" << endl;
	//timer.Start();

	//pull unlisted new entries AT END
	//These will be the entries that haven't previously been sorted
	Table test1 = db->getTable("test1");
	RowResult remainers;
	semaphore.wait();
	remainers = test1.select("id", keyString).where("hit = 0").execute();
	semaphore.notify();

	//timer.Stop();
	//cout << "pulling unhits took " << timer.Results() << " miliseconds" << endl;

	sortList.resize(sortList.size() + remainers.count()); //adjust size to take the remainers into account

	while (remainers.count()) { //add the remainers to the sortList
		Row row = remainers.fetchOne();
		sortList[i].id = row.get(0);
		sortList[i].key = row.get(1);
		i++; //same 'i' as last while statment
	}
	//timer.Start();
	//sort that vector via insertionSort
	insertionSort(sortList);

	Table workingTable = db->getTable(tableString);
	vector<Row> rows(sortList.size());//for sending back to the db
	for (int j = 0; j < sortList.size(); j++) {
		Row row;
		row[0] = j + 1;//position
		row[1] = sortList[j].id;
		rows[j] = row;
	}
	semaphore.wait();
	test1.update().set("hit", false).execute(); //reset the hit detector
	workingTable.remove().execute(); //empty everything
	try{
		workingTable.insert().rows(rows).execute();//refill with sorted list. If I try to pass the vector itself the program shits itself
		semaphore.notify();
	}
	catch (const mysqlx::Error& err) {
		semaphore.notify();
		cout << "ERROR: " << err << endl;
		throw;
	}
	catch (std::exception& ex) {
		semaphore.notify();
		cout << "STD EXCEPTION: " << ex.what() << endl;
		throw;
	}
	catch (const char* ex) {
		semaphore.notify();
		cout << "EXCEPTION: " << ex << endl;
		throw;
	}

	//timer.Stop();
	//cout << "reseting hits took " << timer.Results() << " miliseconds" << endl;
}

void DBInterface::sortMostViewed() {
	sortList(4);

}

void DBInterface::sortTopRated() {
	sortList(3);
}
void DBInterface::sortNewest() {
	sortList(2);
}

vector<Story> DBInterface::pullList(std::string tableName, std::string where, uint32_t offset, uint32_t limit, uint32_t permission) {
	RowResult res;
	semaphore.wait();
	try{
		res = sesh->sql(
			"SELECT test1.*, users.username FROM " + tableName +
			" INNER JOIN test1 ON " + tableName + ".id = test1.id"
			" INNER JOIN users ON users.id = test1.author_id"
			" WHERE permission <= " + std::to_string(permission) + where +
			" ORDER BY ord"
			" LIMIT " + std::to_string(limit) + " OFFSET " + std::to_string(offset)+
			";"
		).execute();
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
	semaphore.notify();
	auto rSize = res.count(); //changes with fetchOne, so lets make a static copy

	vector<Story> ret(rSize);
	for (uint i = 0; i < rSize; i++) {//for each result
		Row row = res.fetchOne();
		const  unsigned char fuckLLVM = (unsigned int)row.get(7); //will not accept narrowing in initialization of the Story object. So need to narrow here
		Story story = { row.get(0), (std::string)row.get(1), (std::string)row.get(2), row.get(3), row.get(4), row.get(6), fuckLLVM, row.get(8), (std::string)row.get(9) };//add result to return vector
		ret[i] = story;
	}

	return ret;
}

vector<Story> DBInterface::pullMostViewed(std::string where, uint32_t offset, uint32_t limit) {
	return pullList("mostviewed", where, offset, limit);
}

vector<Story> DBInterface::pullTopRated(std::string where, uint32_t offset, uint32_t limit) {
	return pullList("toprated", where, offset, limit);
}

vector<Story> DBInterface::pullNewest(std::string where, uint32_t offset, uint32_t limit) {
	return pullList("newest", where, offset, limit);
}

Story DBInterface::pullStoryInfo(const uint32_t& id) {
	Story ret = {};
	RowResult res;
	try {
		semaphore.wait();
		res = sesh->sql("SELECT test1.*, users.username FROM test1 INNER JOIN users ON test1.author_id = users.id WHERE test1.id = " + std::to_string(id)).execute();
		semaphore.notify();
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
	ret.timestamp = row.get(8);
	ret.authorName = (std::string)row.get(9);

	return ret;

}

vector<Story> DBInterface::pullUserStories(uint32_t id, uint32_t offset, uint32_t limit, uint32_t permission){

	return pullList("mostviewed", " AND author_id = " + std::to_string(id), offset, limit, permission);
}

uint32_t DBInterface::getNextIncrement() {
	RowResult res;
	try {
		//Remember: semaphores are handled by whichever method calls this method.
		Table table = sesh->getSchema("INFORMATION_SCHEMA").getTable("TABLES");
		res = table.select("AUTO_INCREMENT").where("TABLE_NAME = 'test1'").execute();
		return res.fetchOne().get(0);
	}
	catch (const mysqlx::Error& err) {
		cout << "ERROR: " << err << endl;
		return 0;
	}
	catch (std::exception& ex) {
		cout << "STD EXCEPTION: " << ex.what() << endl;
		return 0;
	}
	catch (const char* ex) {
		cout << "EXCEPTION: " << ex << endl;
		return 0;
	}
}

DBInterface::~DBInterface() {
	delete db;
	delete sesh;
}
