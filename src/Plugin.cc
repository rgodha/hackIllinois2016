#include "Plugin.h"

#include <curl/curl.h>
#include <curl/easy.h>

namespace plugin { namespace HackIllinois_Safe_Browsing { Plugin plugin; } }

using namespace plugin::HackIllinois_Safe_Browsing;

plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "HackIllinois::Safe_Browsing";
	config.description = "Google Safe Browsing integration";
	config.version.major = 0;
	config.version.minor = 1;
	return config;
	}

size_t url_response_handler( void *ptr, size_t size, size_t nmemb, void *stream)
	{
	printf("%d bytes received from URL\n", (int) size);
	return size;
	}

int Plugin::do_url_request()
{
	CURLcode curl_res;
	CURL *curl = curl_easy_init();
	std::string client = "api";
	std::string apikey = "AIzaSyCdA-CmA7dusGVUIw3d9LubMumv-JgqxMg";
	std::string appver = "1.5.2";
	std::string pver = "3.0";

	std::string baseURL = "https://safebrowsing.google.com/safebrowsing/list";
	std::string arguments = "";
	arguments += "client=" + Plugin::url_encode(client)+"&";
	arguments += "key=" + Plugin::url_encode(apikey) + "&";
	arguments += "appver=" + Plugin::url_encode(appver) + "&";
	arguments += "pver=" + Plugin::url_encode(pver);
	
	std::string url_to_req = baseURL + "?" + arguments;
	
	printf("URL: %s\n", url_to_req.c_str());
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, url_to_req.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, url_response_handler);
	printf("Now send the request\n");
	curl_res = curl_easy_perform(curl);
	
	if(curl_res != CURLE_OK) {
		printf("Error: %s\n", curl_easy_strerror(curl_res));
	}
	
	curl_easy_cleanup(curl);
	return 0;
}

string Plugin::url_encode(const string &value) 
{
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        // make sure c is positive for msvc assertion
        if (c >= 0 && (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')) {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << uppercase;
        escaped << '%' << setw(2) << int((unsigned char) c);
        escaped << nouppercase;
    }

    return escaped.str();
}
