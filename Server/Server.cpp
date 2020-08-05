
#include <iostream>
#include <fstream>


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

// m_htmlcontentmap contains data about the html contents of the website, their mime types
// key: relative URI path of the HTML content being requested
// value: Tuple where:
// Element1: relative path on the disk of the file being requested
// Element2: Mime type/content type of the file
std::map<utility::string_t, std::tuple<utility::string_t, utility::string_t>> m_htmlcontentmap;
std::wstring distFolder = L"../react-client/dist/"; //the folder where react builds to
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

void handle_get(http_request const& req) {
    auto path = req.relative_uri().path();
    std::wcout << L"GET called at " << path << endl;
    auto content_data = m_htmlcontentmap.find(path);
    if (content_data == m_htmlcontentmap.end())
    {
        req.reply(status_codes::NotFound, U("Path not found")).then([](pplx::task<void> t) { handle_error(t); });
        return;
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

int main()
{
    //start sql connection first
    DBInterface db("127.0.0.1", 33060, "app", "xH8#N7GmtILb", "website");

    db.sortTopRated();

    const std::wstring base_url = U("http://localhost:8080");

    web::http::experimental::listener::http_listener listener(base_url);
    
    createContentMap();

    listener.support(web::http::methods::GET, handle_get);

    listener.open().wait();

    std::wcout << std::wstring(U("Listining for requests at: ")) << base_url << std::endl;

    std::string line;
    cout << "Hit Enter to close the listner.";
    std::getline(std::cin, line);

    listener.close().wait();
}


