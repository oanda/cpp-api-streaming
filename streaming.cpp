#include <iostream>
#include <iterator>
#include <string>
#include <sstream>

#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>

using namespace Poco;
using namespace Poco::Net;
using namespace std;

// Edit these global variables with your own information

std::string account_id = "1234567";
std::string access_token = "ACCESS-TOKEN";
std::string instruments = "EUR_USD,USD_CAD,EUR_JPY";
std::string domain = "https://stream-fxpractice.oanda.com";

/*****************************
The domain variable should be:

For Sandbox    -> https://stream-sandbox.oanda.com
For fxPractice -> https://stream-fxpractice.oanda.com
For fxTrade    -> https://stream-fxtrade.oanda.com
******************************/

void handleStream(streambuf* stream_buffer)
{
    std::istreambuf_iterator<char> eos; // end-of-range iterator
    std::istreambuf_iterator<char> iit (stream_buffer); // stream iterator
    string str;

    while (iit!=eos) {
        ostringstream oss;
        while ( !(*iit == '\n' || iit == eos) ) {
            oss << *iit++;
        }

        //connection closed before tick completed
        if (iit == eos) {
            break;
        }

        //print the tick
        cout << oss.str() << endl;

        *iit++;
    }
}

int main ()
{
    try {
        // example code - do not verify SSL credentials. Production clients MUST
        // verify the SSL certificate chain to ensure rates are authentic
        const Context::Ptr context = new Context(Context::TLSV1_CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

        // prepare session
        URI uri(domain + std::string("/v1/prices?accountId=") + account_id + std::string("&instruments=") + instruments);

        HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
        session.setKeepAlive(true);

        // prepare path
        string path(uri.getPathAndQuery());
        if (path.empty()) path = "/";

        // send request
        HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
        req.set("Authorization", std::string("Bearer ") + access_token);
        session.sendRequest(req);

        // get response
        HTTPResponse res;
        istream& rs = session.receiveResponse(res);

        // handle response
        ostringstream out_string_stream;
        handleStream(rs.rdbuf());
    }
    catch (const Exception &e)
    {
        cerr << e.displayText() << endl;
    }
}

