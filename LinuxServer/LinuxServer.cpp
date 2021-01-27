#include <iostream>
#include <fstream>
#include <string.h>
#include <thread>

#include <cryptopp/sha.h>
#include <cryptopp/osrng.h>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>
#include <cpprest/details/http_helpers.h>
#include <cpprest/json.h>

#include <arjet/DBInterface.h>
#include <arjet/WordCounter.h>

using std::cout;
using std::endl;
using std::string;

using web::http::status_codes;
using web::http::http_request;

using namespace web::json;

using CryptoPP::byte;

//contentMap relates file extentions to mime types
std::unordered_map<utility::string_t, utility::string_t> contentMap;

typedef std::map<utility::string_t, utility::string_t> QueryMap;

std::string distFolder = "../react-client/dist/"; //the folder where react builds to
DBInterface* dbp; //pointer to the DBInterface class created in main()

void handleGet(http_request const& req);
void handleGetQuery(http_request const& req);
void handleGetQueryResults(http_request const& req, QueryMap);
void handleGetQueryStory(http_request const& req, const uint32_t& id, QueryMap queries);
void handleGetQueryProfile(http_request const& req, QueryMap queries);
void handleGetFile(http_request const& req);
void handleGetRoot(http_request const& req);
void handleGetBundleCache(http_request const& req);

void handlePost(http_request const& req);
void handleDbPost(http_request const& req, vector<std::string> tokens);
void updateStory(http_request const& req);
void handleLogin(http_request const& req);
void publish(http_request const& req, uint32_t id, uint32_t newPermission);


void handlePut(http_request const& req);
void handlePutStory(http_request const& req, uint32_t id);
void handlePutNew(http_request const& req, vector<std::string> tokens);
void addStoryMeta(http_request const& req);
void handlePutWriter(http_request const& req, uint32_t id);
void handleDbPut(http_request const& req, vector<std::string> tokens);
void addStoryMetaDev(http_request const& req);
void handleSignup(http_request const& req);

void createContentMap() {
    contentMap[U(".html")] = U("text/html");
    contentMap[U(".js")] = U("application/javascript");
    contentMap[U(".css")] = U("text/css");
    contentMap[U(".png")] = U("image/png");
    contentMap[U(".jpg")] = U("image/jpeg");
    contentMap[U(".jpeg")] = U("image/jpeg");
    contentMap[U(".txt")] = U("text/plain");
}

void handle_error(pplx::task<void>& t) {
    try {
        t.get();
    }
    catch (...) {}
}

struct queryItem {
    string key;
    string value;
};

value storyToJSON(const Story& story) {
    value obj = value::object(true);
    obj["id"] = value::number(story.id);
    obj["title"] = value::string(story.title);
    obj["path"] = value::string(story.path);
    obj["rating"] = value::number(story.rating);
    obj["views"] = value::number(story.views);
    obj["authorID"] = value::number(story.authorID);
    obj["permission"] = value::number(story.permission);
    obj["timestamp"] = value::number((int64_t)story.timestamp);
    obj["authorName"] = value::string(story.authorName);
    return obj;
}

Story JSONToStory(value obj) { //NOT safe. Must make safe elsewhere

    Story story;
    if (obj["id"].as_string() == "") {//empty string aka null
        story.id = 0;
    }
    else {
        story.id = std::stoi(obj["id"].as_string());
    }

    story.title = obj[U("title")].as_string();
    story.path = obj["path"].as_string();
    story.rating = stoi(obj["rating"].as_string());
    story.views = stoi(obj["views"].as_string());
    story.authorID = stoi(obj["authorID"].as_string());
    story.permission = stoi(obj["permission"].as_string());
    story.timestamp = stoi(obj["timestamp"].as_string());
    story.authorName = obj["authorName"].as_string();

    return story;
}

UserClear JSONToUserClear(value obj) {
    UserClear user;
    user.id = 0;
    user.username = obj["username"].as_string();
    user.password = obj["password"].as_string();
    user.privilege = 0;
    return user;
}

void outputHash(const unsigned char* hash, std::string message, size_t size = 32) {
    //turns a byte * into a byte vector into base64, then outputs it
    std::vector<unsigned char> vec(size);
    memcpy(vec.data(), hash, size);
    std::string str = utility::conversions::to_base64(vec);
    std::cout << message << str << endl;
}

bool compareHashes(const byte* hash1, const byte* hash2, const size_t size = 32) {  //I should multithread this. Maybe
    for (int i = 0; i < size; i++) {
        if (hash1[i] != hash2[i]) return false;
    }
    return true;
}

value UserClearToJSON(UserClear user) {//replies to a login/signup request with the authTokens for the client to use
    value obj = value::object();
    obj["id"] = user.id;
    obj["username"] = value(user.username);
    obj["password"] = value(user.password);
    obj["privilege"] = user.privilege;
    return obj;
}

uint32_t handleAuthentication(http_request const& req, uint32_t privRequired = 0) {//This function verifies the authentication of the header, and returns the id
    //THIS FUNCTION REPLIES ON FAILURE. DO NOT REPLY TWICE;
    web::http::http_headers headers = req.headers();
    if (!headers.has("Authorization")) {
        req.reply(401U);
        return 0;
    } //make sure this is something to authenticate


    std::string authstringRaw = headers["Authorization"];
    std::string authString64W = authstringRaw.substr(6);

    auto data = utility::conversions::from_base64(authString64W);
    string authString = reinterpret_cast<char*>(data.data());
    authString.resize(data.size());//cause it doesn't know when to end

    size_t delim = authString.find_first_of(':');
    uint32_t id = stoi(authString.substr(0, delim));
    string pass = authString.substr(delim + 1);

    User user = dbp->pullUser(id);
    if (user.id == 0) { //if the previous function failed
        req.reply(401U);
        return 0;
    }
    CryptoPP::SHA256 sha;
    sha.Update((const byte*)pass.data(), pass.size());
    sha.Update(user.salt, 32);
    byte* hash = (byte*)(malloc(32));
    sha.Final(hash);
    if (!compareHashes(hash, user.hash)) {
        req.reply(403U);
        free(hash);
        return(0);
    }
    else if (privRequired) {
        if (user.privilege < privRequired) {
            req.reply(403U);
            return(0);
        }

    }
    return id;
}

uint32_t handleAuthentication(http_request const& req, User& user) {//Overload to fill a user
    //THIS FUNCTION REPLIES ON FAILURE. DO NOT REPLY TWICE;
    web::http::http_headers headers = req.headers();
    if (!headers.has("Authorization")) {
        req.reply(401U);
        return 0;
    } //make sure this is something to authenticate


    std::string authstringRaw = headers["Authorization"];
    std::string authString64W = authstringRaw.substr(6);

    auto data = utility::conversions::from_base64(authString64W);
    string authString = reinterpret_cast<char*>(data.data());
    authString.resize(data.size());//cause it doesn't know when to end

    size_t delim = authString.find_first_of(':');
    uint32_t id = stoi(authString.substr(0, delim));
    string pass = authString.substr(delim + 1);
    user = dbp->pullUser(id);//Should be safe to assign before checking as a failure in authentication leads to a 403 and a return 0
    CryptoPP::SHA256 sha;
    sha.Update((const byte*)pass.data(), pass.size());
    sha.Update(user.salt, 32);
    byte* hash = (byte*)(malloc(32));
    sha.Final(hash);
    if (!compareHashes(hash, user.hash)) {
        req.reply(403U);
        free(hash);
        return(0);
    }

    return id;
}

uint32_t ip4StringToInt(const string& str) {
    //dirty, but works
    uint32_t ret = 0;
    string helper = str; //have to copy so it's no longer const
    ret += 0x01000000 * std::stoi(helper.substr(0, helper.find_first_of('.')));
    helper = helper.substr(helper.find_first_of('.') + 1);
    ret += 0x010000 * std::stoi(helper.substr(0, helper.find_first_of('.')));
    helper = helper.substr(helper.find_first_of('.') + 1);
    ret += 0x0100 * std::stoi(helper.substr(0, helper.find_first_of('.')));
    helper = helper.substr(helper.find_first_of('.') + 1);
    ret += std::stoi(helper);
    return ret;
}
uint32_t getIP(http_request const& req) {
    return ip4StringToInt(req.remote_address().c_str());
}

vector<std::string> splitTokens(http_request const& req) {

    vector<std::string> tokens;
    string s = '/' + (string)req.absolute_uri().path() + '/'; //add the slashes so it won't ignore anything

    std::string delimiter = "/";
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        if (token != "") tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    return tokens;
}


void handleGet(http_request const& req) {
    auto path = req.relative_uri().to_string();
    std::cout << "GET called at " << path << endl;

    if (req.relative_uri().query().size() != 0) { //yes yes I know != 0 is unnecessary but it's easier to read
        handleGetQuery(req);
    }
    else if (req.relative_uri().path().find_last_of(L'.') != std::string::npos) {
        auto len = req.relative_uri().path().size();
        if (req.relative_uri().path().substr(len - 10, 10) == "cbundle.js") {//cached js app
            handleGetBundleCache(req);
        }
        else handleGetFile(req);
    }
    else {
        handleGetRoot(req);
    }
}

void handleGetQuery(http_request const& req) {//For pulling from the db
    //break path into tokens

    vector<std::string> tokens = splitTokens(req);
    if (tokens.size() == 0) { req.reply(404U); return; }
    auto queries = web::uri::split_query(req.relative_uri().query());
    if (tokens[0] == "users") {
        handleGetQueryProfile(req, queries);
    }
    else if (tokens[0] == "results") {
        handleGetQueryResults(req, queries);
    }
    else if (tokens[0] == "story") {
        uint32_t id = stoi(queries["id"]);
        handleGetQueryStory(req, id, queries);
    }
    else req.reply(404U);
}

void handleGetQueryResults(http_request const& req, QueryMap queries) {
    vector<Story> stories;

    string where = ""; //DONT LET THE USER SET THIS
    std::string sqltest = "";
    if (queries.find("sq") != queries.end()) {//where
        //This can be used for catagories, authors, etc. Right now it's useless
    }
    uint32_t lim = 20;
    if (queries.find("lim") != queries.end()) {//results per page
        lim = std::stoi(queries["lim"]);
    }
    uint32_t offset = 0;
    if (queries.find("p") != queries.end()) {//which page
        offset = lim * (std::stoi(queries["p"]) - 1);
    }
    if (queries.find("o") != queries.end()) {//order
        if (queries["o"] == "top") {
            stories = dbp->pullTopRated(where, offset, lim);
        }
        else if (queries["o"] == "mv") {
            stories = dbp->pullMostViewed(where, offset, lim);
        }
        else if (queries["o"] == "new") {
            stories = dbp->pullNewest(where, offset, lim);
        }
    }

    value arr = value::array();
    cout << stories.size() << endl;
    for (int i = 0; i < stories.size(); i++) {
        value temp = storyToJSON(stories[i]);
        arr[i] = temp;
    }

    req.reply(status_codes::OK, arr);
}

void handleGetQueryStory(http_request const& req, const uint32_t& id, QueryMap queries) {
    //Triggers if a query is called that mentions id. Pulls the specific story and sends the story along with the database entry as a JSON

    /* cout << "the IP of the user is " << req.remote_address() << endl;
    uint32_t ip = ip4StringToInt((wchar_t*)req.remote_address().c_str());
    cout << "or " << std::hex << ip << std::dec << endl;*/
    Story story = dbp->pullStoryInfo(id);
    if (story.permission > 1) {
        User user;
        if (!handleAuthentication(req, user)) {
            return;
        }
        if (!(user.privilege > 1 || user.id == story.authorID)) { //well that's a little ugly
            req.reply(403U);
            return;
        }
    }

    if (queries["t"] == "m") {//request for metadata
        value storyJSON = storyToJSON(story);

        if (req.headers().has("Authorization")) {
            auto head = req.headers();
            if (head["Authorization"] != "") {

                uint32_t id = handleAuthentication(req);
                storyJSON["userRating"] = dbp->findRating(id, story.id);
            }
            else storyJSON["userRating"] = 0;
        }
        else storyJSON["userRating"] = 0;
        req.reply(200U, storyJSON);
        return;
    }
    else if (queries["t"] == "t") {//request for html file
        std::wifstream file;
        string path = "../" + story.path;
        cout << path << endl;
        try {
            concurrency::streams::fstream::open_istream(path, std::ios::in)
                .then([=](concurrency::streams::istream is) {
                req.reply(200U, is, "text/html");
                    }).wait(); //need to wait for the try block to work
                    return;
        }
        catch (...) { //Catches if file not found. So reply with 404
            cout << "Story file not found" << endl;
            req.reply(404u);
            return;
        }
    }

}

void handleGetQueryProfile(http_request const& req, QueryMap queries) {
    auto tokens = splitTokens(req);

    uint32_t id = std::stoi(tokens[1]);
    string where = "";
    std::string sqltest = "";

    uint32_t lim = 20;
    if (queries.find("lim") != queries.end()) {//results per page
        lim = std::stoi(queries["lim"]);
    }
    uint32_t offset = 0;
    if (queries.find("p") != queries.end()) {//which page
        offset = lim * (std::stoi(queries["p"]) - 1);
    }

    uint32_t access = 1;
    if (queries["a"] == "self") {
        uint32_t reqID = handleAuthentication(req);
        if (reqID != id) {
            req.reply(403U);
            return;
        }
        access = 2; //the user has the privilage to view her own unpublished stories
    }
    if (queries["a"] == "admin") {
        uint32_t reqID = handleAuthentication(req, 3);
        if (!reqID) {
            req.reply(401U);
            return;
        }
        access = 2; //the user has the privilage to view her own unpublished stories
    }
    vector<Story> stories = dbp->pullUserStories(id, offset, lim, access);
    value arr = value::array();
    for (int i = 0; i < stories.size(); i++) {
        value temp = storyToJSON(stories[i]);
        arr[i] = temp;
    }
    value ret = value::object();
    ret["results"] = arr;
    ret["name"] = value(dbp->pullUser(id).username);
    req.reply(status_codes::OK, ret);

}

void handleGetFile(http_request const& req) {
    auto path = distFolder + req.relative_uri().path();
    auto extension = path.substr(path.find_last_of('.'));
    auto content_type_it = contentMap.find(extension);
    if (content_type_it == contentMap.end()) { //if the file isn't found
        req.reply(404U);
        return;
    }
    string content_type = content_type_it->second;


    concurrency::streams::istream is = concurrency::streams::fstream::open_istream(path, std::ios::in).get();
    req.reply(200U, is, content_type).then([](pplx::task<void> t) { handle_error(t); }).wait();
    is.close();
}

void handleGetRoot(http_request const& req) {
    try {

        concurrency::streams::istream is = concurrency::streams::fstream::open_istream(distFolder + "index.html", std::ios::in).get();
        req.reply(200U, is, "text/html").then([](pplx::task<void> t) { handle_error(t); }).wait();
        is.close();
    }

    catch (web::http::http_exception& ex) {
        cout << "HTTP EXCEPTION: " << ex.what() << endl;
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
}

void handleGetBundleCache(http_request const& req) {
    if (req.headers().has("If-None-Match")) { //Firefox has strict cache rules. I use a different caching system from wha it wants
        req.reply(304U);
    }
    else {
        auto path = distFolder + req.relative_uri().path();
        concurrency::streams::istream is = concurrency::streams::fstream::open_istream(path, std::ios::in).get();
        web::http::http_response res = web::http::http_response(200U);
        res.set_body(is);
        web::http::http_headers head;

        head["Content-Type"] = "application/javascript";
        head["Cache-Contro"] = "public, max-age=6048000, immutable";
        head["ETag"] = "WhyFirefoxWhy";
        res.headers() = head;
        req.reply(res).then([](pplx::task<void> t) { handle_error(t); }).wait();
        is.close();
    }



}


void handlePost(http_request const& req) {

    auto tokens = splitTokens(req);
    if (tokens.size() == 0) { req.reply(404U); return; }
    //tokens is the part of the path with all the slashes

    if ((tokens[0]) == "db") {
        handleDbPost(req, tokens);
    }
    else if (tokens[0] == "auth") {
        handleLogin(req);
    }
    else if (tokens[0] == "writer") {
        if (tokens[2] == "publish") {
            publish(req, std::stoi(tokens[1]), 1);
        }
        else if (tokens[2] == "makeprivate") {
            publish(req, std::stoi(tokens[1]), 2);
        }
        else req.reply(404U);
    }
    else req.reply(404);
}

void publish(http_request const& req, uint32_t id, uint32_t newPermission) { //sets the 'permission' aspect of the story, changing who can access it
    Story story = dbp->pullStoryInfo(id);
    User user;
    if (!handleAuthentication(req, user)) return;
    if (user.id != story.authorID && user.privilege < 3) {
        req.reply(403U); //not the author or an admin
        return;
    }
    //at this point we assume the user is authorized to make changes to this story
    story.permission = newPermission; //the important line
    if (dbp->updateStory(story)) {
        req.reply(200U);
    }
    else req.reply(500U);
    return;

}

void handleDbPost(http_request const& req, vector<std::string> tokens) {
    if (!handleAuthentication(req, 1)) {
        return; //handleAuthentication takes care of the reply
    }
    //Switches can SMD
    if (tokens[1] == "resort") {
        if (tokens[2] == "toprated") {
            dbp->sortTopRated();
            req.reply(200);
            return;
        }
        else if (tokens[2] == "mostviewed") {
            dbp->sortMostViewed();
            req.reply(200);
            return;
        }
    }
    else if (tokens[1] == "update") {
        if (tokens[2] == "story") {

            updateStory(req);
        }
    }
    else req.reply(404);
}

void updateStory(http_request const& req) {

    Story story;
    try {
        value v = req.extract_json().get();
        story = JSONToStory(v);
    }
    catch (web::json::json_exception& ex) {
        cout << "JSON EXCEPTION: " << ex.what() << endl;
        throw;
    }
    catch (std::exception& ex) {
        cout << "STD EXCEPTION: " << ex.what() << endl;
        throw;
    }

    if (dbp->updateStory(story) == true) {
        req.reply(200);
    }
    else {
        req.reply(500);
        cout << "ERROR: failed to update database" << endl;
    }
}

void handleLogin(http_request const& req) {
    //get who we're attempting to log in
    UserClear userClear = JSONToUserClear(req.extract_json().get());
    userClear.id = dbp->findUser(userClear.username);

    if (userClear.id == 0) { req.reply(401U); } //can't find user
    User user = dbp->pullUser(userClear.id);

    userClear.privilege = user.privilege; //for replying with a privilege level

    CryptoPP::SHA256 sha;

    sha.Update((const byte*)userClear.password.data(), userClear.password.size());
    sha.Update(user.salt, 32);
    size_t digestSize = 32;
    byte* hash = (CryptoPP::byte*)malloc(digestSize);
    sha.Final(hash);
    if (compareHashes(hash, user.hash)) {
        req.reply(200U, UserClearToJSON(userClear));
    }
    else {
        req.reply(401U);
    }
    free(hash);//I may be a student, but at least I free my memory
    return;
}


void handlePut(http_request const& req) {
    auto tokens = splitTokens(req);

    if ((tokens[0]) == "db") {
        handleDbPut(req, tokens);
    }
    else if (tokens[0] == "auth") {
        handleSignup(req);
    }
    else if (tokens[0] == "new") {
        handlePutNew(req, tokens);
    }
    else if (tokens[0] == "writer") {
        handlePutWriter(req, std::stoi(tokens[1]));
    }
    else if (tokens[0] == "story") {
        handlePutStory(req, std::stoi(tokens[1]));
    }
    else req.reply(404);

}

void handlePutStory(http_request const& req, uint32_t id) {
    auto queries = web::uri::split_query(req.relative_uri().query());

    if (queries.find("t") != queries.end()) {//type of put
        if (queries["t"] == "r") { //rating
            auto uid = handleAuthentication(req);
            if (!uid) return; //you must be logged in to rate
            int rating = std::stoi(queries["r"]);
            if (rating > 50 || rating < 5) { req.reply(400); return; } //invalid rating
            req.reply(dbp->addRating(id, uid, rating));
        }
        else if (queries["t"] == "v") { //view
            auto ip = getIP(req);
            req.reply(dbp->addView(id, ip));
        }
        else req.reply(404);
    }
    else req.reply(404);
}

void handlePutNew(http_request const& req, vector<std::string> tokens) {
    //Switches can SMD
    if (tokens[1] == "meta") {
        addStoryMeta(req);
    }
    else req.reply(404);
}

void addStoryMeta(http_request const& req) {
    uint32_t authorID = handleAuthentication(req, 0);
    if (!authorID) {
        cout << "Couldn't add story: failed authentication" << endl;
        return;
    }
    Story story;
    story.id = 0; //Not used in this function. Set to 0 to flag as error.
    story.authorID = authorID;
    try {
        story.title = req.extract_json().get()["title"].as_string();
    }
    catch (web::json::json_exception& ex) {
        cout << "JSON EXCEPTION: " << ex.what() << endl;
        req.reply(500);
        return;
    }
    catch (std::exception& ex) {
        cout << "STD EXCEPTION: " << ex.what() << endl;
        req.reply(500);
        return;
    }
    uint32_t id = dbp->addStory(story);
    if (id != 0) {
        req.reply(201U, id);//HTTP Created

    }
    else {
        req.reply(500);
        cout << "ERROR: failed to add to database" << endl;
    }

}

void handlePutWriter(http_request const& req, uint32_t id) {
    std::ofstream file;
    file.open("../Stories-Dirty/" + std::to_string(id) + ".html", std::ios::trunc); //saved in dirty as it is unsanitized user input
    std::vector<unsigned char> content = req.extract_vector().get();
    file.write(reinterpret_cast<char*>(content.data()), content.size());
    file.close();
    std::system("sudo ../SanitizerBuild/Sanitizer");
    //launches the C#-based sanitizer to prevent xss. Outputs the cleaned stories into ../stories
    req.reply(201U);
}

void handleDbPut(http_request const& req, vector<std::string> tokens) {
    //Switches can SMD
    if (tokens[1] == "add") {
        if (tokens[2] == "story") {

            addStoryMetaDev(req);
        }
    }
    else req.reply(404);
}

void addStoryMetaDev(http_request const& req) {
    if (!handleAuthentication(req, 1)) {
        cout << "Couldn't add story: failed authentication" << endl;
        return;
    }
    Story story;
    story.id = 0; //Not used. Set to 0 to flag as error.
    try {
        story = JSONToStory(req.extract_json().get());
    }
    catch (web::json::json_exception& ex) {
        cout << "JSON EXCEPTION: " << ex.what() << endl;
        throw;
    }
    catch (std::exception& ex) {
        cout << "STD EXCEPTION: " << ex.what() << endl;
        throw;
    }
    if (dbp->addStory(story) != 0) {
        req.reply(201);//HTTP Created
    }
    else {
        req.reply(500);
        cout << "ERROR: failed to add to database" << endl;
    }
}

void UpdateStoryFile(http_request const& req, QueryMap queries) {
    //std::ofstream("../)
}

void handleSignup(http_request const& req) {
    UserClear userClear;
    userClear = JSONToUserClear(req.extract_json().get());
    if (dbp->findUser(userClear.username) != 0) {
        cout << "Failed to add user, already exists" << endl;
        req.reply(409U);//conflict
        return;
    }

    CryptoPP::SHA256 sha;
    sha.Update((const byte*)userClear.password.data(), userClear.password.size());

    User user = User();
    user.username = userClear.username;
    user.privilege = 0;

    CryptoPP::NonblockingRng rnjesus = CryptoPP::NonblockingRng();
    rnjesus.GenerateBlock(user.salt, 32);//256 bytes
    sha.Update(user.salt, 32); //user.salt and user.hash were already allocated by User() and will be freed at destruction
    sha.Final(user.hash);

    userClear.id = dbp->addUser(user);
    if (userClear.id != 0) { //success
        req.reply(201U, UserClearToJSON(userClear));
        return;
    }
    else {
        req.reply(500U);
        return;
    }
}


void autoSort(uint64_t miliseconds) {
    Timer timer;
    while (true) { //execute until the end. Could tie this to a shared resource, but nah
        std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));

        //timer.Start();
        dbp->updateRatings();
        dbp->updateViews();
        //timer.Stop();
        //cout << "calculating ratings took " << timer.Results() << " miliseconds" << endl;
        dbp->sortNewest();
        dbp->sortMostViewed();
        dbp->sortTopRated();
    }

}

int main(int argc, char* argv[]) {
    cout << "lets test the counting!" << endl;

    string localIP = "192.168.0.12";

    if (argc > 1){ //allows setting local ip through command line
        localIP = argv[1];
    }

#ifdef DEBUG
    dbp = new DBInterface(localIP, 33060, "app", "xH8#N7GmtILb", "website"); //dev address

    //dbp = new DBInterface("54.242.214.211", 33060, "app", "xH8#N7GmtILb", "website"); //server address
    const std::string base_url = "http://127.0.0.1:8080/";
#else
    dbp = new DBInterface("172.26.5.20", 33060, "app", "xH8#N7GmtILb", "website"); //server-local address
    const std::string base_url = "http://3.225.135.220:80/";
#endif


    createContentMap();

    web::http::experimental::listener::http_listener_config conf;
    conf.set_ssl_context_callback([](boost::asio::ssl::context& ctx){
        try {
            cout << "callback hit" << endl;
            ctx.set_options(boost::asio::ssl::context::default_workarounds);

            boost::system::error_code e;
            ctx.use_certificate_file("cert.pem", boost::asio::ssl::context::pem);
            ctx.use_private_key_file("key.pem", boost::asio::ssl::context::pem);
            ctx.use_certificate_chain_file("chain.pem");
        }
        catch (std::exception& ex) {
            cout << "STD EXCEPTION: " << ex.what() << endl;
            throw;
        }
        catch (const char* ex) {
            cout << "EXCEPTION: " << ex << endl;
            throw;
        }
    });



    web::http::experimental::listener::http_listener listener(base_url, conf);


    listener.support(web::http::methods::GET, handleGet);

    listener.support(web::http::methods::POST, handlePost);

    listener.support(web::http::methods::PUT, handlePut);

    std::thread sorter(autoSort, 15 * 1000);

    try {
        listener.open().wait();
    }
    catch (web::http::http_exception& ex) {
        cout << "HTTP EXCEPTION: " << ex.what() << endl;
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

    std::cout << std::string(U("Listining for requests at: ")) << base_url << std::endl;

    std::this_thread::sleep_for(std::chrono::hours(10000));

    listener.close().wait();
}
