#include <vector>
#include <iostream>
#include <windows.h>
#include "SCMWrapper.h"

using namespace std;

int main(int argc, char* argv[])
{
	SCMWrapper scm;

	vector<SCMWrapper::SCM_SERVICE_STATUS_PROCESS> installedServices;
	scm.GetServiceList(installedServices);

	cout << "Number of installed services: " << installedServices.size() << endl;

	cout << "AFD is installed?  " << scm.IsServiceInstalled(L"AFD") << endl;
	cout << "XYZ is installed?  " << scm.IsServiceInstalled(L"XYZ") << endl;

	wstring serviceName = L"WSearch";
	if (scm.IsServiceInstalled(serviceName))
	{
		if (scm.GetServiceStatus(serviceName) == SERVICE_RUNNING)
		{
			cout << "Stopping..." << endl;
			scm.StopService(serviceName);
			while (scm.GetServiceStatus(serviceName) != SERVICE_STOPPED)
				Sleep(10);
			cout << "Stopped..." << endl;
			cout << "Starting..." << endl;
			scm.StartService(serviceName);
			while (scm.GetServiceStatus(serviceName) != SERVICE_RUNNING)
				Sleep(10);
			cout << "Started..." << endl;
		}
	}

	system("PAUSE");

	return 0;
}

