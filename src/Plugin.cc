#include "Plugin.h"

#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include "ChunkData.pb.h"

namespace plugin { namespace HackIllinois_Safe_Browsing { Plugin plugin; } }

using namespace plugin::HackIllinois_Safe_Browsing;

// Bro information about the Plugin
plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "HackIllinois::Safe_Browsing";
	config.description = "Google Safe Browsing integration";
	config.version.major = 0;
	config.version.minor = 1;
	return config;
	}

// Used by the URL response callback function
struct MemoryStruct {
	char* memory;
	size_t  size;
	};

// The URL response callback function
static size_t url_response_handler(void *contents, size_t size, size_t nmemb, void *userp)
	{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
		}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size]  = 0;

	printf("%s\n", mem->memory);

	return realsize;
	}

void parseData(const ChunkData& c_data, size_t c_len) 
	{	
	for(int i=0; i < c_data.add_numbers_size(); i++) {
		printf("Chunk Number: %d\n",c_data.chunk_number());
		if(c_data.has_chunk_type()) {
			ChunkData::ChunkType c_type = c_data.chunk_type();
			switch(c_type) {
				case ChunkData::ChunkType::ChunkData_ChunkType_ADD:
					printf("Chunk Type: ADD\n");
					break;

				case ChunkData::ChunkType::ChunkData_ChunkType_SUB:
					printf("Chunk Type: SUB\n");
					break;
			}
		}
		
	    	if(c_data.has_prefix_type()) {	
			ChunkData::PrefixType p_type = c_data.prefix_type();
			switch(p_type) {
				case ChunkData::PrefixType::ChunkData_PrefixType_PREFIX_4B:
					printf("Prefix is 4B\n");
					break;

				case ChunkData::PrefixType::ChunkData_PrefixType_FULL_32B:
					printf("Prefix is 32B\n");
					break;
			}
		}
		/* hash is included for each hash 
 		 * TODO
		 **/
		if(c_data.has_hashes()) {
			printf("Have Hashes\n");	
		}
	}
	return;
	}

// The URL response callback function for protobuffers
static size_t url_response_handler_proto_buff(void *contents, size_t size, size_t nmemb, void *userp)
	{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
		}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size]  = 0;

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	uint32 chunk_len = (mem->memory[0] & 0xff) << 24 |
		           (mem->memory[1] & 0xff) << 16 |
		           (mem->memory[2] & 0xff) << 8  |
		           (mem->memory[3] & 0xff);

	printf("Parsing chunk of length %d\n", chunk_len);
	// TODO: Parse the chunk data using protocol buffers
	char* chunk_data = mem->memory + 4;
	
	//ChunkData::
	// Figure out how to do something like:
	//ProtoBuffer->Parse(chunk_data, chunk_len);
	// Parse the data.
	//parseData(chunk_data, chunk_len);

	return realsize;
	}

// Implements HTTP Response for List from Google Safe Browsing API documentation
int Plugin::download_list_types()
{
	CURLcode curl_res;
	CURL *curl = curl_easy_init();

	struct MemoryStruct chunk;

	chunk.memory = (char *)malloc(1); /* will be grown as needed by the realloc above */
	chunk.size   = 0;       /* no data at this point */

	const char* baseURL = "https://safebrowsing.google.com/safebrowsing/list";
	const char* client = "api";
	const char* apikey = "AIzaSyCdA-CmA7dusGVUIw3d9LubMumv-JgqxMg";
	const char* appver = "1.5.2";
	const char* pver = "3.0";

	char url[256];
	snprintf(url, 256, "%s?client=%s&key=%s&appver=%s&pver=%s", baseURL, client, apikey, appver, pver);

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, url_response_handler);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_res = curl_easy_perform(curl);
	
	if(curl_res != CURLE_OK) {
		printf("Error: %s\n", curl_easy_strerror(curl_res));
	}

	// TODO: Figure out what to do with the result. Right now, we're just printing it.
	
	curl_easy_cleanup(curl);
	return 0;
}

// Implements first part HTTP Response for Data from Google Safe Browsing API documentation
int Plugin::download_dataset_for_list(char* list_name)
{
	CURLcode curl_res;
	CURL *curl = curl_easy_init();

	struct MemoryStruct chunk;

	chunk.memory = (char *)malloc(1); /* will be grown as needed by the realloc above */
	chunk.size   = 0;       /* no data at this point */

	const char* baseURL = "https://safebrowsing.google.com/safebrowsing/downloads";
	const char* client = "api";
	const char* apikey = "AIzaSyCdA-CmA7dusGVUIw3d9LubMumv-JgqxMg";
	const char* appver = "1.5.2";
	const char* pver = "3.0";

	char url[256];
	snprintf(url, 256, "%s?client=%s&key=%s&appver=%s&pver=%s", baseURL, client, apikey, appver, pver);
	char list[64];
	snprintf(list, 64, "%s;\n", list_name);

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, list);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, url_response_handler);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_res = curl_easy_perform(curl);
	
	if(curl_res != CURLE_OK) {
		printf("Error: %s\n", curl_easy_strerror(curl_res));
	}

	// TODO: Parse the result, and query the redirection URLs
	// (start with u:) with download_redirect_data_for_list
	
	
	curl_easy_cleanup(curl);
	return 0;
}

int Plugin::download_redirect_data_for_list(char* redirect_url)
	{
	CURLcode curl_res;
	CURL *curl = curl_easy_init();

	struct MemoryStruct chunk;

	chunk.memory = (char *)malloc(1); /* will be grown as needed by the realloc above */
	chunk.size   = 0;       /* no data at this point */

	char url[1024];
	snprintf(url, 1024, "https://%s", redirect_url);

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, url_response_handler_proto_buff);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_res = curl_easy_perform(curl);
	
	if(curl_res != CURLE_OK) {
		printf("Error: %s\n", curl_easy_strerror(curl_res));
	}

	curl_easy_cleanup(curl);
	return 0;
	}
