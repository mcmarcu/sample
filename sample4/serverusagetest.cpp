#include <serverusage.h>

int main()
{
	ServerUsage serverUsage(5);
	sleep(20);
	serverUsage.GetSample();
}



