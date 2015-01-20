#include "proxy.h"

std::string find_program_in_path (const std::string &program)
{
	std::string path;
	char* tmp = std::getenv("PATH");

	if (tmp == NULL)
		path = "/bin:/usr/bin:.";
	else
		path = tmp;
	
	size_t keyIdx = 0;
	std::istringstream is(path);
	std::string part;
	while (std::getline(is, part, ':'))
	{
		std::string fullpath = part+"/"+program;
		struct stat sb;
		if( stat(fullpath.c_str(), &sb) == 0 
			&& (sb.st_mode & S_IXUSR)
			&& !S_ISDIR(sb.st_mode)) 
			return fullpath;
	}
 
  return "";
}


std::string execCMD(const char* cmd) 
{
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[256];
    std::string result = "";
    while(!feof(pipe)) 
	{
    	if(fgets(buffer, 256, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

std::string gnome_proxy_get_parameter(int parameter, DetectType gnome_version)
{
	std::string result;

	do
	{
		if (parameter > GNOME_PROXY_USE_AUTH)
			break;
		if (gnome_version > PROXYD_GNOME3)
			break;
		
		result = execCMD(gproxycmds[parameter][gnome_version]);
		result.erase(std::remove(result.begin(), result.end(), '\''), result.end());
		result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
		result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
		
	}while(0);

	return result;
}

std::string kde_proxy_get_parameter(int parameter)
{
	std::string result;

	do
	{
		if (parameter > KDE_PROXY_HTTP)
			break;
		
		result = execCMD(kdeproxycmds[parameter]);
		result.erase(std::remove(result.begin(), result.end(), '\''), result.end());
		result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
		result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
		
	}while(0);

	return result;
}


DetectType GetCmdVersion()
{
	std::string tmp;

	tmp = execCMD("cat /root/.curlrc 2>/dev/null");
	if (tmp != "") 
		return PROXYD_SLES;
		
	tmp = find_program_in_path("kreadconfig");
	if (tmp != "") 
		return PROXYD_KDE;

	tmp = find_program_in_path("gsettings");
	if (tmp != "") 
		return PROXYD_GNOME3;

	tmp = find_program_in_path("gconftool-2");
	if (tmp != "") 
		return PROXYD_GNOME2;
		
	

	return PROXYD_NONE;
}


//parse this "http://user:passwd@your.proxy.server:port/"
ProxyInfo parse_form_env(std::string envproxy)
{
	ProxyInfo info;
	
	std::string httpTag = "http://";
	int idx = envproxy.find( httpTag, 0 );
	if( idx != std::string::npos )
		envproxy.erase( 0, httpTag.length() );

	std::string credentials;
	std::string server;
	idx = envproxy.find( "@", 0 );
	if( idx != std::string::npos )
	{
		info.proxyType = PROXYT_AUTH;
		credentials = envproxy.substr( 0, idx );
		server = envproxy.substr( idx+1, envproxy.length() );
	}
	else
	{
		info.proxyType = PROXYT_NOAUTH;
		server = envproxy;
	}
	
	if( server == "" )
		return ProxyInfo();
	
	info.host = server;
	idx = server.find( ":", 0 );
	if( idx != std::string::npos )
	{
		info.host = server.substr( 0, idx );
		if( server.length() - (idx+1) > 0)
			info.port = atoi(server.substr( idx+1, server.length() ).c_str());
	}
	
	info.host.erase(std::remove(info.host.begin(), info.host.end(), '\''), info.host.end());
	
	
	info.username = credentials;
	if(credentials != "")
	{
		idx = credentials.find( ":", 0 );
		if( idx != std::string::npos )
		{
			info.username = credentials.substr( 0, idx );
			if( credentials.length() - (idx+1) > 0)
				info.password = credentials.substr( idx+1, credentials.length() ).c_str();
		}
	}

	return info;
}

ProxyInfo env_proxy_get_info()
{
	char* envproxy;
	
	if(((envproxy = std::getenv("HTTP_PROXY")) == NULL) && ((envproxy = std::getenv("http_proxy")) == NULL) && ((envproxy = std::getenv("HTTPPROXY")) == NULL))
		return ProxyInfo();
	

	ProxyInfo info = parse_form_env(envproxy);
	info.detectType = PROXYD_ENV;

	char* user;
	char* password;
	char* port;
	
	if(((user = std::getenv("HTTP_PROXY_USER")) != NULL) || ((user = std::getenv("http_proxy_user")) != NULL) || ((user = std::getenv("HTTPPROXYUSER")) != NULL))
		info.username = user;
	
	if(((user = std::getenv("HTTP_PROXY_PASS")) != NULL) || ((user = std::getenv("http_proxy_pass")) != NULL) || ((user = std::getenv("HTTPPROXYPASS")) != NULL))
		info.password = password;
	
	if(((port = std::getenv("HTTP_PROXY_PORT")) != NULL) || ((port = std::getenv("http_proxy_port")) != NULL) || ((port = std::getenv("HTTPPROXYPORT")) != NULL))
		info.port = atoi(port);
	
	if(info.host == "")
		info.detectType = PROXYD_NONE;
	
	return info;
}

ProxyInfo gnome_proxy_get_info()
{
	ProxyInfo info;
	std::string tmp;
	DetectType gnome_version = GetCmdVersion();

	//proxy mode
	tmp = gnome_proxy_get_parameter(GNOME_PROXY_MODE, gnome_version);

	
	if (tmp == "")
		return ProxyInfo();
		
	if (tmp == "none") 
		return ProxyInfo();

	if (tmp == "auto") 
		return ProxyInfo();

	
	//proxy host/port
	info.host = gnome_proxy_get_parameter(GNOME_PROXY_HTTP_HOST, gnome_version);
	if (info.host == "")
		return ProxyInfo();
	
	tmp = gnome_proxy_get_parameter(GNOME_PROXY_HTTP_PORT, gnome_version);
	if (tmp == "")
		return ProxyInfo();
	info.port = atoi(tmp.c_str());
	
	
	tmp = gnome_proxy_get_parameter(GNOME_PROXY_USE_AUTH, gnome_version);
	if( tmp == "1" || tmp == "true" )
	{
		info.username = gnome_proxy_get_parameter(GNOME_PROXY_HTTP_USER, gnome_version);
		info.password = gnome_proxy_get_parameter(GNOME_PROXY_HTTP_PASS, gnome_version);
	}
	
	info.proxyType = info.username != "" ? PROXYT_AUTH  : PROXYT_NOAUTH;

	return info;
}

std::string curlrc_get_parameter( std::string param )
{
	std::ifstream is( "/root/.curlrc" );
	std::string line;
	std::string value;
	while ( std::getline(is, line) )
	{
		if( line.find(param) != std::string::npos )
		{
			int idxStart = line.find( "\"", 0 );
			if( idxStart != std::string::npos )
			{
				int idxEnd = line.find( "\"", idxStart+1 );
				if( idxEnd != std::string::npos && idxEnd - idxStart -1 > 0)
					value = line.substr( idxStart+1, idxEnd - idxStart - 1);
			}
		}
	}
	is.close();
	
	return value;
}


ProxyInfo sles_proxy_get_info()
{
	ProxyInfo info;
	std::string credentials;
	std::string server;
	info.detectType = PROXYD_SLES;
	
	server = curlrc_get_parameter("--proxy");
	credentials = curlrc_get_parameter("--proxy-user");
	
	std::string httpTag = "http://";
	int idx = server.find( httpTag, 0 );
	if( idx != std::string::npos )
		server.erase( 0, httpTag.length() );
	
	info.host = server;
	idx = server.find( ":", 0 );
	if( idx != std::string::npos )
	{
		info.host = server.substr( 0, idx );
		if( server.length() - (idx+1) > 0)
			info.port = atoi(server.substr( idx+1, server.length() ).c_str());
	}
	
	
	info.username = credentials;
	if(credentials != "")
	{
		idx = credentials.find( ":", 0 );
		if( idx != std::string::npos )
		{
			info.username = credentials.substr( 0, idx );
			if( credentials.length() - (idx+1) > 0)
				info.password = credentials.substr( idx+1, credentials.length() ).c_str();
		}
	}
	
	info.proxyType = info.username != "" ? PROXYT_AUTH  : PROXYT_NOAUTH;
	
	return info;
}

ProxyInfo kde_proxy_get_info()
{

	ProxyInfo info;
	std::string tmp;

	//proxy type
	tmp = kde_proxy_get_parameter(KDE_PROXY_TYPE);

	if (tmp == "" || tmp == "0"/*no proxy*/ || tmp == "2"/*PAC URL*/ || tmp == "3"/*WPAD*/  || tmp == "4"/*use environment*/)
		return ProxyInfo();
		
	//1 == manual

	tmp = kde_proxy_get_parameter(KDE_PROXY_AUTHMODE);//only without auth https://bugs.kde.org/show_bug.cgi?id=232626
	if (tmp != "0")
	{
	}
	
	tmp = kde_proxy_get_parameter(KDE_PROXY_HTTP);
	info = parse_form_env(tmp);
		
	info.proxyType = info.username != "" ? PROXYT_AUTH  : PROXYT_NOAUTH;
	return info;
}

ProxyInfo proxy_get_info()
{
	DetectType cmd_version = GetCmdVersion();
	
	ProxyInfo commandProxy;
	ProxyInfo envProxy;
	
	if(cmd_version == PROXYD_KDE)
		commandProxy = kde_proxy_get_info();
	if(cmd_version == PROXYD_GNOME3 || cmd_version == PROXYD_GNOME2 )
		commandProxy = gnome_proxy_get_info();
	if(cmd_version == PROXYD_SLES )
		commandProxy = sles_proxy_get_info();
		
	commandProxy.detectType = cmd_version;

	envProxy = env_proxy_get_info();
	
	return commandProxy.proxyType != PROXYT_NONE ? commandProxy : envProxy;
}

