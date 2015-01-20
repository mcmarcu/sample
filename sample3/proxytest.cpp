#include "proxy.h"
#include <iostream>

int main()
{
	ProxyInfo proxy = proxy_get_info();
	
	std::cout<<proxy.NicePrint();
}
