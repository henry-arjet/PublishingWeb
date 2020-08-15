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

// m_htmlcontentmap contains data about the html contents of the website, their mime types
// key: relative URI path of the HTML content being requested
// value: Tuple where:
// Element1: relative path on the disk of the file being requested
// Element2: Mime type/content type of the file
std::map<utility::string_t, std::tuple<utility::string_t, utility::string_t>> m_htmlcontentmap;
std::wstring distFolder = L"../react-client/dist/"; //the folder where react builds to
DBInterface *dbp; //can do this as db is created in main

void handleGet(http_request const& req);
void handleGetQuery(http_request const& req);
void handleGetQueryID(http_request const& req, const uint32_t& id);
void handleGetFile(http_request const& req);

void handlePost(http_request const& req);
void handleDbPost(http_request const& req, vector<std::wstring> tokens);
void updateStory(http_request const& req);

void handlePut(http_request const& req);
void handleDbPut(http_request const& req, vector<std::wstring> tokens);
void addStory(http_request const& req);
void handleSignup(http_request const& req);

void createContentMap() {
    m_htmlcontentmap[U("/")] = std::make_tuple(distFolder+U("index.html"), U("text/html"));
    m_htmlcontentmap[U("/bundle.js")] = std::make_tuple(distFolder + U("bundle.js"), U("application/javascript"));
    m_htmlcontentmap[U("/styles.css")] = std::make_tuple(distFolder + U("styles.css"), U("text/css"));

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
    story.rating = std::stoi(obj[L"rating"].as_string());
    story.views = std::stoi(obj[L"views"].as_string());
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

void handleGet(http_request const& req) {

    auto path = req.relative_uri().to_string();
    std::wcout << L"GET called at " << path << endl;
    
    if (req.relative_uri().query().size() != 0) { //yes yes I know != 0 is unnecessary but it's easier to read
        handleGetQuery(req);
    }
    else handleGetFile(req);//I can get away with this
}

void handleGetQuery(http_request const& req) {//For pulling ordered stories from the db
    auto queries = web::uri::split_query(req.relative_uri().query()); //I wrote an entire function for this before realizing there was an inbuilt.

    //I know this pattern sucks. But it should work
    if (queries.find(L"id") != queries.end()) {
        //Basically this function is dedicated to searching, but it triggers on any query. So, I want to have the get request for a specific
        //story be query based. So, I put a little hook in the beginning that diverts the request to a different function
        //if it is asking for a specific story. It's jank AF, but...
        handleGetQueryID(req, std::stoi(queries[L"id"]));
        return;
    }

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

void handleGetQueryID(http_request const& req, const uint32_t& id) {
    //Triggers if a query is called that mentions id. Pulls the specific story and sends the story along with the database entry as a JSON
    Story story = dbp->pullStoryInfo(id);
    value storyJSON = storyToJSON(story);

    std::wifstream file;
    string path = "../" + story.path;
    file.open(path, std::ios::binary);
    if (!file.is_open()) {
        cout << "Unable to open file at " << path << endl;
        req.reply(404u); //404 as this likely means the path is incorrect
        return;
    }
    value text;
    file >> text;

    value obj = value::object();
    obj[L"properties"] = storyJSON;
    obj[L"text"] = text;
    req.reply(200U, obj);
}

void handleGetFile(http_request const& req) {
    auto path = req.relative_uri().path().substr(req.relative_uri().path().find_last_of('/'));
    auto content_data = m_htmlcontentmap.find(path);
    //std::wcout << L"Searching for file at " << path << endl;
    if (content_data == m_htmlcontentmap.end()) { //Cant find file, most likely should be handeld client side by router
        content_data = m_htmlcontentmap.find(L"/");//Give home to client and let react-router figure it out
    }
    auto file_name = std::get<0>(content_data->second);
    auto content_type = std::get<1>(content_data->second);

    concurrency::streams::fstream::open_istream(file_name, std::ios::in)
        .then([=](concurrency::streams::istream is) {
        req.reply(status_codes::OK, is, content_type).then([](pplx::task<void> t) { handle_error(t); });
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
            req.reply(status_codes::InternalError).then([](pplx::task<void> t) { handle_error(t); });
        }
    });
}

void handlePost(http_request const& req) {

    auto path = req.relative_uri().to_string();
    std::wcout << L"POST called at " << path << endl;

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
        handleDbPost(req, tokens);
    }
    else req.reply(404);
}

void handleDbPost(http_request const& req, vector<std::wstring> tokens) {
    //Switches can SMD
    if (tokens[1] == L"resort") {
        if (tokens[2] == L"toprated") { dbp->sortTopRated(); req.reply(200); return; }
        else if (tokens[2] == L"mostviewed") { dbp->sortMostViewed(); req.reply(200); return; }
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

    //stream >> v;
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
    else req.reply(404);

}

void handleDbPut(http_request const& req, vector<std::wstring> tokens) {
    //Switches can SMD
    if (tokens[1] == L"add") {
        if (tokens[2] == L"story") {

            addStory(req);
        }
    }
else req.reply(404);
}

void addStory(http_request const& req) {

    Story story;
    story.id = 0; //Not used. Set to 0 to flag as error.

    story = JSONToStory(req.extract_json().get());

    if (dbp->addStory(story) == true) {
        req.reply(201);//HTTP Created
    }
    else {
        req.reply(500);
        cout << "ERROR: failed to add to database" << endl;
    }
}

void handleSignup(http_request const& req) {
    UserClear userClear;
    userClear = JSONToUserClear(req.extract_json().get());
    if (dbp->findUser(userClear.username) != 0) {
        cout << "Failed to add user, already exists" << endl;
        req.reply(409U);//conflict
        return;
    }
    cout << "Let's do some crypto!" << endl;
    using CryptoPP::byte;

    CryptoPP::SHA256 sha;
    sha.Update((const byte*)userClear.password.data(), userClear.password.size());

    byte* salt = static_cast<byte*>(malloc(32));//256 bytes
    CryptoPP::NonblockingRng rnjesus = CryptoPP::NonblockingRng();
    rnjesus.GenerateBlock(salt, 32);
    sha.Update(salt, 32);
    byte* hash = static_cast<byte*>(malloc(32));
    sha.Final(hash);

    User user;
    user.id = 0; //Will be replaced by autoincrement
    user.username = userClear.username;
    user.hash = hash;
    user.salt = salt;
    user.privilege = 0;
    free(salt); free(hash);

    if (dbp->addUser(user)) {
        req.reply(201U);
        return;
    }else {
        req.reply(500U);
        return;
    }
}


int main()
{
    //start sql connection first
    DBInterface db = DBInterface("70.113.85.67", 33060, "app", "xH8#N7GmtILb", "website");
    

    handleGetQueryID(http_request(), 6);

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


