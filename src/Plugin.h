
#ifndef BRO_PLUGIN_HACKILLINOIS_SAFE_BROWSING
#define BRO_PLUGIN_HACKILLINOIS_SAFE_BROWSING

#include <plugin/Plugin.h>

namespace plugin {
namespace HackIllinois_Safe_Browsing {

class Plugin : public ::plugin::Plugin
{
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
};

extern Plugin plugin;

}
}

#endif
