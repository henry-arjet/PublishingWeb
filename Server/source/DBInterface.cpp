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
	
}

//uint fabUint(uint8_t chars[4]) {
//	return uint(chars[0]) << 24 | uint(chars[1]) << 16 | uint(chars[2]) << 8 | uint(chars[3]);
//}
//
//uint8_t* breakUint(uint in) {
//	uint8_t ret[4];
//	uint8_t test = 7;
//	ret[0] = uint8_t(in >> 24);
//	ret[1] = uint8_t(in >> 16);
//	ret[2] = uint8_t(in >> 8);
//	ret[3] = uint8_t(in);
//	return ret;
//}

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

void DBInterface::sortTopRated() {
	//read current list of top rated
	std::fstream file;

	file.open("../lists/top_rated.uil", std::ios::ate | std::ios::binary | std::ios::in); //ios::app so we can use tellp
	if (!file.is_open()){cout << "Unable to open file" << endl; return;}
	auto size = file.tellg();
	file.seekg(0, std::ios::beg);
	auto sizeUint = size / 4;
	assert(size % 4 = 0);//cause if not the file is messed up. 4 bytes is one uint
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
		sortList[i].key = row.get(3); //gets rating from 0-50000 (mapped to 0.0-5.0 client side)
	
	}

	//and tag all the ones that I just got
	for (int i = 0; i < readList.size(); i++) { //there's probably a better way
		table.update().set("hit", 1).where("id = " + std::to_string(readList[i])).execute();
	}
	
	//pull unlisted new entries AT END
	//These will be the entries that haven't previously been sorted
	try{
		table.select().execute();
		//RowResult remainers = table.select().where("hit = false").execute();
	}
	catch (const mysqlx::Error& err){
		cout << "ERROR: " << err << endl;
		throw;
	}
	catch (std::exception& ex){
		cout << "STD EXCEPTION: " << ex.what() << endl;
		throw;
	}
	catch (const char* ex){
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
		sortList[i].key = row.get(3);
	}

	//sort that vector via insertionSort
	insertionSort(sortList);

	//save list
	file.open("../lists/top_rated.uil", std::ios::trunc | std::ios::out | std::ios::binary);
	//file.open("top_rated.uil", std::ios::binary | std::ios::trunc);//we just closed and reopened the file so we could erase the contents
	if (!file.is_open()) { cout << "Unable to open file" << endl; return; }
	auto i = file.tellp(); //should be 0
	for (int i = 0; i < sortList.size(); i++) {
		file.write((char*)&sortList[i].id, 4);
	}
	file.close();
	table.update().set("hit", false).execute(); //reset the hit detector
	
}

DBInterface::~DBInterface() {
	delete db;
	delete sesh;
}
