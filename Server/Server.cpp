#include <iostream>
#include <fstream>

#if defined(WIN32)
#include <windows.h>
#endif

#include <cryptopp/sha.h>
#include <cryptopp/osrng.h>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>
#include <cpprest/details/http_helpers.h>
#include <cpprest/json.h>

#include <arjet/DBInterface.h>

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

std::wstring distFolder = L"../react-client/dist/"; //the folder where react builds to
DBInterface *dbp; //pointer to the DBInterface class created in main()

void handleGet(http_request const& req);
void handleGetQuery(http_request const& req);
void handleGetQueryResults(http_request const& req, QueryMap);
void handleGetQueryStory(http_request const& req, const uint32_t& id, QueryMap queries);
void handleGetQueryProfile(http_request const& req, QueryMap queries);
void handleGetFile(http_request const& req);
void handleGetRoot(http_request const& req);

void handlePost(http_request const& req);
void handleDbPost(http_request const& req, vector<std::wstring> tokens);
void updateStory(http_request const& req);
void handleLogin(http_request const& req);


void handlePut(http_request const& req);
void handlePutNew(http_request const& req, vector<std::wstring> tokens);
void addStoryMeta(http_request const& req);
void handleDbPut(http_request const& req, vector<std::wstring> tokens);
void addStoryMetaDev(http_request const& req);
void handleSignup(http_request const& req);

void createContentMap() {
    contentMap[U(".html")] = U("text/html");
    contentMap[U(".js")] = U("application/javascript");
    contentMap[U(".css")] = U("text/css");
}

void handle_error(pplx::task<void>& t) {
    try {
        t.get();
    }
    catch (...){}
}

struct queryItem {
    string key;
    string value;
};

value storyToJSON(const Story& story){
    value obj = value::object();
    obj[L"id"] = value::number(story.id);
    obj[L"title"] = value::string(utility::conversions::utf8_to_utf16(story.title));
    obj[L"path"] = value::string(utility::conversions::utf8_to_utf16(story.path));
    obj[L"rating"] = value::number(story.rating);
    obj[L"views"] = value::number(story.views);
    obj[L"authorID"] = value::number(story.authorID);
    obj[L"permission"] = value::number(story.permission);
    return obj;
}

Story JSONToStory(value obj) { //NOT safe. Must make safe elsewhere

    Story story;
    if (obj[L"id"].as_string() == L"") {//empty string aka null
        story.id = 0; 
    }
    else {
        story.id = std::stoi(obj[L"id"].as_string());        
    }

    story.title = utility::conversions::utf16_to_utf8(obj[L"title"].as_string());
    story.path = utility::conversions::utf16_to_utf8(obj[L"path"].as_string());
    story.rating = stoi(obj[L"rating"].as_string());
    story.views = stoi(obj[L"views"].as_string());
    story.authorID = stoi(obj[L"authorID"].as_string());
    story.permission = stoi(obj[L"permission"].as_string());
    return story;
}

UserClear JSONToUserClear(value obj) {
    UserClear user;
    user.id = 0;
    user.username = utility::conversions::utf16_to_utf8(obj[L"username"].as_string());
    user.password = utility::conversions::utf16_to_utf8(obj[L"password"].as_string());
    user.privilege = 0;
    return user;
}

void outputHash(const unsigned char* hash, std::wstring message, size_t size = 32) {
    //turns a byte * into a byte vector into base64, then outputs it
    std::vector<unsigned char> vec(size);
    memcpy(vec.data(), hash, size);
    std::wstring str = utility::conversions::to_base64(vec);
    std::wcout << message << str << endl;
}

bool compareHashes(const byte* hash1, const byte* hash2, const size_t size = 32) {  //I should multithread this. Maybe
    for (int i = 0; i < size; i++) {
        if (hash1[i] != hash2[i]) return false;
    }
    return true;
}

value UserClearToJSON( UserClear user) {//replies to a login/signup request with the authTokens for the client to use
    value obj = value::object();
    obj[L"id"] = user.id;
    obj[L"username"] = value(utility::conversions::utf8_to_utf16(user.username));
    obj[L"password"] = value(utility::conversions::utf8_to_utf16(user.password));
    obj[L"privilege"] = user.privilege;
    return obj;
}

uint32_t handleAuthentication(http_request const& req, uint32_t privRequired = 0) {//This function verifies the authentication of the header, and returns the id
    //THIS FUNCTION REPLIES ON FAILURE. DO NOT REPLY TWICE;
    web::http::http_headers headers = req.headers();
    if (!headers.has(L"Authorization")) {
        req.reply(401U);
        return 0; 
    } //make sure this is something to authenticate

    
    std::wstring authstringRaw = headers[L"Authorization"];
    std::wstring authString64W = authstringRaw.substr(6);
    
    auto data = utility::conversions::from_base64(authString64W);
    string authString = reinterpret_cast<char*>(data.data());
    authString.resize(data.size());//cause it doesn't know when to end
    
    size_t delim = authString.find_first_of(':');
    uint32_t id = stoi(authString.substr(0, delim));
    string pass = authString.substr(delim + 1);

    User user = dbp->pullUser(id);
    CryptoPP::SHA256 sha;
    sha.Update((const byte*)pass.data(), pass.size());
    sha.Update(user.salt, 32);
    byte* hash = static_cast<byte*>(malloc(32));
    sha.Final(hash);
    if (!compareHashes(hash, user.hash)) {
        req.reply(403U);
        free(hash);
        return(0);
    }
    else if(privRequired){
        if (user.privilege < privRequired) {
            req.reply(403U);
            return(0);
        }

    }
    return id;
}

uint32_t handleAuthentication(http_request const& req, User & user) {//Overload to fill a user
    //THIS FUNCTION REPLIES ON FAILURE. DO NOT REPLY TWICE;
    web::http::http_headers headers = req.headers();
    if (!headers.has(L"Authorization")) {
        req.reply(401U);
        return 0;
    } //make sure this is something to authenticate


    std::wstring authstringRaw = headers[L"Authorization"];
    std::wstring authString64W = authstringRaw.substr(6);

    auto data = utility::conversions::from_base64(authString64W);
    string authString = reinterpret_cast<char*>(data.data());
    authString.resize(data.size());//cause it doesn't know when to end

    size_t delim = authString.find_first_of(':');
    uint32_t id = stoi(authString.substr(0, delim));
    string pass = authString.substr(delim + 1);

    user = dbp->pullUser(id); //Should be safe to assign before checking as a failure in authentication leads to a 403 and a return 0
    CryptoPP::SHA256 sha;
    sha.Update((const byte*)pass.data(), pass.size());
    sha.Update(user.salt, 32);
    byte* hash = static_cast<byte*>(malloc(32));
    sha.Final(hash);
    if (!compareHashes(hash, user.hash)) {
        req.reply(403U);
        free(hash);
        return(0);
    }

    return id;
}

void handleGet(http_request const& req) {

    auto path = req.relative_uri().to_string();
    std::wcout << L"GET called at " << path << endl;
    
    if (req.relative_uri().query().size() != 0) { //yes yes I know != 0 is unnecessary but it's easier to read
        handleGetQuery(req);
    }
    else if (req.relative_uri().path().find_last_of(L'.') != std::wstring::npos) {
        handleGetFile(req);
    }
    else {
        handleGetRoot(req);
    }
}

void handleGetQuery(http_request const& req) {//For pulling from the db
    //break path into tokens

    //TODO clean this tf up
    //Also move to its own function
    vector<std::wstring> tokens;
    wchar_t* pathC = (wchar_t*)req.absolute_uri().path().c_str();
    wchar_t* del = (wchar_t*)L"/";
    wchar_t* helperPtr;
    wchar_t* token = wcstok_s(pathC, del, &helperPtr);
    while (token != NULL)
    {
        tokens.push_back(token);
        token = wcstok_s(nullptr, del, &helperPtr);
    }
    if (tokens.size() == 0) { req.reply(404U); }

    auto queries = web::uri::split_query(req.relative_uri().query());

    if (tokens[0] == L"users") {
        handleGetQueryProfile(req, queries);
    }
    else if (tokens[0] == L"results") {
        handleGetQueryResults(req, queries);
    }
    else if (tokens[0] == L"story") {
        uint32_t id = stoi(queries[L"id"]);
        handleGetQueryStory(req, id, queries);
    }
    else req.reply(404U);
}

void handleGetQueryResults(http_request const& req, QueryMap queries){
    vector<Story> stories;

    string where = "";
    std::wstring sqltest = L"";
    if (queries.find(L"sql") != queries.end()) {//where
        //This can be used for catagories, authors, etc. Right now it's useless
    }
    uint32_t lim = 20;
    if (queries.find(L"lim") != queries.end()) {//results per page
        lim = std::stoi(queries[L"lim"]);
    }
    uint32_t offset = 0;
    if (queries.find(L"p") != queries.end()) {//which page
        offset = lim * (std::stoi(queries[L"p"]) - 1);
    }
    if (queries.find(L"o") != queries.end()) {//order
        if (queries[L"o"] == L"top") {
            stories = dbp->pullTopRated(where, offset, lim);
        }
        else if (queries[L"o"] == L"mv") {
            stories = dbp->pullMostViewed(where, offset, lim);
        }
    }

    value arr = value::array();
    for (int i = 0; i < stories.size(); i++) {
        value temp = storyToJSON(stories[i]);
        arr[i] = temp;
    }

    req.reply(status_codes::OK, arr);
}

void handleGetQueryStory(http_request const& req, const uint32_t& id, QueryMap queries) {
    //Triggers if a query is called that mentions id. Pulls the specific story and sends the story along with the database entry as a JSON

    Story story = dbp->pullStoryInfo(id);
    if (story.permission > 0) {
        User user;
        if (!handleAuthentication(req, user)) {
            return;
        }
        if (!(user.privilege > 1 || user.id == story.authorID)) { //well that's a little ugly
            req.reply(403U);
            return;
        }
    }

    if (queries[L"t"] == L"m") {//request for metadata
        value storyJSON = storyToJSON(story);
        req.reply(200U, storyJSON);
        return;
    }else if(queries[L"t"] == L"t"){//request for html file
        std::wifstream file;
        string path = "../" + story.path;
        concurrency::streams::fstream::open_istream(utility::conversions::utf8_to_utf16(path), std::ios::in)
            .then([=](concurrency::streams::istream is) {
                req.reply(200U, is, L"text/html");
            });
        return;
    }

}

void handleGetQueryProfile(http_request const& req, QueryMap queries) {
    uint32_t id = handleAuthentication(req);
    if (!id) {
        req.reply(401U);
        return;
    }

    string where = "";
    std::wstring sqltest = L"";

    uint32_t lim = 20;
    if (queries.find(L"lim") != queries.end()) {//results per page
        lim = std::stoi(queries[L"lim"]);
    }
    uint32_t offset = 0;
    if (queries.find(L"p") != queries.end()) {//which page
        offset = lim * (std::stoi(queries[L"p"]) - 1);
    }
    vector<Story> stories = dbp->pullUserStories(id);
    value arr = value::array();
    for (int i = 0; i < stories.size(); i++) {
        value temp = storyToJSON(stories[i]);
        arr[i] = temp;
    }

    req.reply(status_codes::OK, arr);

}

void handleGetFile(http_request const& req) {
    auto path = distFolder + req.relative_uri().path();
    auto extension = path.substr(path.find_last_of('.'));
    auto content_type_it = contentMap.find(extension);
    if (content_type_it == contentMap.end()) { //if the file isn't found
        req.reply(404U);
        return;
    }
    wstring content_type = content_type_it->second;


    concurrency::streams::fstream::open_istream(path, std::ios::in)
        .then([=](concurrency::streams::istream is) {
        req.reply(200U, is, content_type).then([](pplx::task<void> t) { handle_error(t); });
    })
        .then([=](pplx::task<void> t) {
        try
        {
            t.get();
        }
        catch (...)
        {
            // opening the file (open_istream) failed.
            // Reply with an error.
            req.reply(404U).then([](pplx::task<void> t) { handle_error(t); });
        }
    });
}

void handleGetRoot(http_request const& req) {
    concurrency::streams::fstream::open_istream(distFolder + L"index.html", std::ios::in)
        .then([=](concurrency::streams::istream is) {
        req.reply(status_codes::OK, is, L"text/html"); });
}

void handlePost(http_request const& req) {

    auto path = req.relative_uri().to_string();
    std::wcout << L"POST called at " << path << endl;

    //break path into tokens
    vector<std::wstring> tokens;
    wchar_t* pathC = (wchar_t*)path.c_str();
    wchar_t* del = (wchar_t*)L"/";
    wchar_t* helperPtr;
    wchar_t* token = wcstok_s(pathC, del, &helperPtr);
    while (token != NULL)
    {
        tokens.push_back(token);
        token = wcstok_s(nullptr, del, &helperPtr);
    }
    if (tokens.size() == 0) { req.reply(404U); }

    if ((tokens[0]) == L"db") {
        handleDbPost(req, tokens);
    }
    else if (tokens[0] == L"auth") {
        handleLogin(req);
    }
    else req.reply(404);
}

void handleDbPost(http_request const& req, vector<std::wstring> tokens) {
    if (!handleAuthentication(req, 1)) {
        return; //handleAuthentication takes care of the reply
    }
    //Switches can SMD
    if (tokens[1] == L"resort") {
        if (tokens[2] == L"toprated") { 
            dbp->sortTopRated();
            req.reply(200); 
            return; 
        }
        else if (tokens[2] == L"mostviewed") { 
            dbp->sortMostViewed(); 
            req.reply(200); 
            return; 
        }
    }
    else if (tokens[1] == L"update") {
        if (tokens[2] == L"story") {

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
    byte* hash = static_cast<byte*>(malloc(32));
    sha.Final(hash);
    if (compareHashes(hash, user.hash)) {
        req.reply(200U, UserClearToJSON(userClear));
    }
    else req.reply(401U);
    free(hash);//I may be a student, but at least I free my memory
    return;
}

void handlePut(http_request const& req) {

    auto path = req.relative_uri().to_string();
    std::wcout << L"PUT called at " << path << endl;

    //break path into tokens
    vector<std::wstring> tokens;
    wchar_t* pathC = (wchar_t*)path.c_str();
    wchar_t* del = (wchar_t*)L"/";
    wchar_t* helperPtr;
    //rsize_t strmax = path.size();
    wchar_t* token = wcstok_s(pathC, del, &helperPtr);
    while (token != NULL)
    {
        tokens.push_back(token);
        token = wcstok_s(nullptr, del, &helperPtr);
    }
    if (tokens.size() == 0) { req.reply(404U); }


    if ((tokens[0]) == L"db") {
        handleDbPut(req, tokens);
    }
    else if (tokens[0] == L"auth") {
        handleSignup(req);
    }
    else if (tokens[0] == L"new") {
        handlePutNew(req, tokens);
    }
    else req.reply(404);

}

void handlePutNew(http_request const& req, vector<std::wstring> tokens) {
    //Switches can SMD
    if (tokens[1] == L"meta") {
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
    story.id = 0; //Not used. Set to 0 to flag as error.
    story.authorID = authorID;
    try {
        story.title = utility::conversions::utf16_to_utf8(req.extract_json().get()[L"title"].as_string());
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

    if (dbp->addStory(story) != 0) {
        req.reply(201);//HTTP Created
        //system("..SanitizerBuild/netcoreapp3.1/Sanitizer.pdb");
    }
    else {
        req.reply(500);
        cout << "ERROR: failed to add to database" << endl;
    }

}

void handleDbPut(http_request const& req, vector<std::wstring> tokens) {
    //Switches can SMD
    if (tokens[1] == L"add") {
        if (tokens[2] == L"story") {

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
    }else {
        req.reply(500U);
        return;
    }
}


int main(){
    dbp = new DBInterface("54.242.214.211", 33060, "app", "xH8#N7GmtILb", "website");

    const std::wstring base_url = U("http://*:8080");


    web::http::experimental::listener::http_listener listener(base_url);
    
    createContentMap();

    listener.support(web::http::methods::GET, handleGet);

    listener.support(web::http::methods::POST, handlePost);

    listener.support(web::http::methods::PUT, handlePut);

    
    try {
        listener.open().wait();
    }
    catch (web::http::http_exception& ex){
        cout << "HTTP EXCEPTION: " << ex.what() << endl;
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

    std::wcout << std::wstring(U("Listining for requests at: ")) << base_url << std::endl;

    std::string line;
    cout << "Hit Enter to close the listner.\n";
    std::getline(std::cin, line);

    listener.close().wait();
}