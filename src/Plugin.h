
#ifndef BRO_PLUGIN_HACKILLINOIS_SAFE_BROWSING
#define BRO_PLUGIN_HACKILLINOIS_SAFE_BROWSING

#include <iostream>
#include <curl/curl.h>
#include <curl/easy.h>
#include <cstring>
#include <sstream>
#include <iomanip>

#include <plugin/Plugin.h>

namespace plugin {
namespace HackIllinois_Safe_Browsing {

class Plugin : public ::plugin::Plugin
{
public:
	int download_list_types();
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	std::string url_encode(const std::string &value);
};

extern Plugin plugin;

}
}

#endif
