#ifndef __PROXY_H__
#define __PROXY_H__

#include <algorithm>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

typedef enum
{
	PROXYT_NONE = 0,
	PROXYT_AUTH,
	PROXYT_NOAUTH

} ProxyType;

typedef enum
{
	PROXYD_GNOME2 = 0,
	PROXYD_GNOME3,
	PROXYD_KDE,
	PROXYD_ENV,
	PROXYD_SLES,
	PROXYD_NONE
	
} DetectType;

struct ProxyInfo
{
	ProxyInfo():proxyType(PROXYT_NONE),detectType(PROXYD_NONE),host(),port(0),username(),password()
	{
	}
	ProxyType proxyType;             /* The proxy type.  */
	DetectType detectType;           /* The detection type.  */

	std::string host;           /* The host.        */
	int   port;                 /* The port number. */
	std::string username;       /* The username.    */
	std::string password;       /* The password.    */
	
	std::string NicePrint()
	{
		std::string value;
		char strPort[16]={0};
		sprintf(strPort,"%d",port);
		value += "Host: " + host;
		value += "\n";
		value += "Port: ";
		value += strPort;
		value += "\n";
		value += "Username: " + username;
		value += "\n";
		value += "Password: " + password;
		value += "\n";
		value += "Proxy Type: ";
		if(proxyType == PROXYT_NONE)
				value += "no proxy detected";
		if(proxyType == PROXYT_AUTH)
				value += "proxy with auth";
		if(proxyType == PROXYT_NOAUTH)
				value += "proxy without auth";
		value += "\n";
		value += "Proxy Detected using: ";
		if(detectType == PROXYD_GNOME2)
				value += "gnome2";
		if(detectType == PROXYD_GNOME3)
				value += "gnome3";
		if(detectType == PROXYD_SLES)
				value += "sles";
		if(detectType == PROXYD_KDE)
				value += "kde";
		if(detectType == PROXYD_ENV)
				value += "environment";
		if(detectType == PROXYD_NONE)
				value += "none";
		value += "\n";
		
		return value;
	}
};

ProxyInfo proxy_get_info();

/* index in gproxycmds below*/
#define GNOME_PROXY_MODE 0
#define GNOME_PROXY_USE_SAME_PROXY 1
#define GNOME_PROXY_SOCKS_HOST 2
#define GNOME_PROXY_SOCKS_PORT 3
#define GNOME_PROXY_HTTP_HOST 4
#define GNOME_PROXY_HTTP_PORT 5
#define GNOME_PROXY_HTTP_USER 6
#define GNOME_PROXY_HTTP_PASS 7
#define GNOME_PROXY_USE_AUTH 8

/* detect proxy settings for gnome2/gnome3 */
static const char* gproxycmds[][2] = 
{
	{ "gconftool-2 -g /system/proxy/mode" , "gsettings get org.gnome.system.proxy mode" },
	{ "gconftool-2 -g /system/http_proxy/use_same_proxy", "gsettings get org.gnome.system.proxy use-same-proxy" },
	{ "gconftool-2 -g /system/proxy/socks_host", "gsettings get org.gnome.system.proxy.socks host" },
	{ "gconftool-2 -g /system/proxy/socks_port", "gsettings get org.gnome.system.proxy.socks port" },
	{ "gconftool-2 -g /system/http_proxy/host", "gsettings get org.gnome.system.proxy.http host" },
	{ "gconftool-2 -g /system/http_proxy/port", "gsettings get org.gnome.system.proxy.http port"},
	{ "gconftool-2 -g /system/http_proxy/authentication_user", "gsettings get org.gnome.system.proxy.http authentication-user" },
	{ "gconftool-2 -g /system/http_proxy/authentication_password", "gsettings get org.gnome.system.proxy.http authentication-password" },
	{ "gconftool-2 -g /system/http_proxy/use_authentication", "gsettings get org.gnome.system.proxy.http use-authentication" },
	
};

/* index in kdeproxycmds below */
#define KDE_PROXY_AUTHMODE 0
#define KDE_PROXY_TYPE     1
#define KDE_PROXY_HTTP     2
static const char* kdeproxycmds[] =
{
	"kreadconfig --file kioslaverc --group \"Proxy Settings\" --key AuthMode",
	"kreadconfig --file kioslaverc --group \"Proxy Settings\" --key ProxyType",
	"kreadconfig --file kioslaverc --group \"Proxy Settings\" --key httpProxy"
};



#endif
