#include <vector>
#include <string>
#include <windows.h>

class SCMWrapper
{

public:
	SCMWrapper();
	SCMWrapper(std::wstring machineName, std::wstring databaseName);
	~SCMWrapper();

	struct SCM_SERVICE_STATUS_PROCESS {
		std::wstring				serviceName;
		std::wstring                displayName;
		SERVICE_STATUS_PROCESS  ServiceStatusProcess;
	};
	//get a list with info on the installed services
	//returns true if the operation was successful
	bool GetServiceList(std::vector<SCM_SERVICE_STATUS_PROCESS>& services);

	//returns true if the service is installed
	bool IsServiceInstalled(const std::wstring& serviceName);

	#define SERVICE_STATUS_FAIL      0x00000008
	//returns the service status (SERVICE_RUNNING, etc) or SERVICE_STATUS_FAIL in case of failure 
	//precondition: the service needs to be installed
	DWORD GetServiceStatus(const std::wstring& serviceName);

	//returns true if the service is successfully sent a start message
	//precondition: the service needs to be installed & stopped
	bool StartService(const std::wstring& serviceName);
	
	//returns true if the service is successfully sent a stop message
	//precondition: the service needs to be installed & running
	bool StopService(const std::wstring& serviceName);

private:

	SC_HANDLE hSCM;
};
