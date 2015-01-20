#include "SCMWrapper.h"
class ServiceHandle
{
	public:
		ServiceHandle(const SC_HANDLE SCMHandle, const std::wstring& serviceName, const DWORD desiredAccess)
		{
			hService = OpenService(SCMHandle, serviceName.c_str(), desiredAccess);
			if (hService == NULL)
				throw std::runtime_error("could not get service handle");
		}
		SC_HANDLE get()
		{
			return hService;
		}

		~ServiceHandle()
		{
			CloseServiceHandle(hService);
		}
	private:
		SC_HANDLE hService;
};
	

SCMWrapper::SCMWrapper()
{
		hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS | SC_MANAGER_CONNECT);
		if (hSCM == NULL)
			throw std::runtime_error("could not connect to SCM");
}

SCMWrapper::SCMWrapper(std::wstring machineName, std::wstring databaseName)
{
		hSCM = OpenSCManager(machineName.c_str(), databaseName.c_str(), SC_MANAGER_ALL_ACCESS | SC_MANAGER_CONNECT);
		if (hSCM == NULL)
			throw std::runtime_error("could not connect to SCM");
}

SCMWrapper::~SCMWrapper()
{
		CloseServiceHandle(hSCM);
}

bool SCMWrapper::GetServiceList(std::vector<SCM_SERVICE_STATUS_PROCESS>& services)
{
	LPBYTE  pBuf = NULL;
	DWORD  dwBufSize = 0;
	DWORD  dwBufNeed = 0;
	DWORD  dwNumberOfServices = 0;
	BOOL enumStatus = FALSE;

	enumStatus = EnumServicesStatusEx(
		hSCM,
		SC_ENUM_PROCESS_INFO,
		SERVICE_TYPE_ALL,
		SERVICE_STATE_ALL,
		NULL,
		dwBufSize,
		&dwBufNeed,
		&dwNumberOfServices,
		NULL,
		NULL);

	if (enumStatus == FALSE && dwBufNeed <= 0)
	{
		return false;
	}

	dwBufSize = dwBufNeed + 0x10;
	pBuf = static_cast<LPBYTE>(malloc(dwBufSize));

	enumStatus = EnumServicesStatusEx(
		hSCM,
		SC_ENUM_PROCESS_INFO,
		SERVICE_TYPE_ALL,
		SERVICE_STATE_ALL,
		pBuf,
		dwBufSize,
		&dwBufNeed,
		&dwNumberOfServices,
		NULL,
		NULL);

	if (enumStatus == FALSE)
	{
		return false;
	}

	LPENUM_SERVICE_STATUS_PROCESS pInfo = NULL;
	pInfo = reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESS>(pBuf);
	for (ULONG i = 0; i<dwNumberOfServices; i++)
	{
		services.push_back(SCM_SERVICE_STATUS_PROCESS{ pInfo[i].lpDisplayName, pInfo[i].lpServiceName, pInfo[i].ServiceStatusProcess });
	}

	free(pBuf);

	return true;
}

bool SCMWrapper::IsServiceInstalled(const std::wstring& serviceName)
{
	try
	{
		ServiceHandle serv(hSCM, serviceName, SERVICE_QUERY_STATUS);
		SERVICE_STATUS status;
		BOOL ret = ::QueryServiceStatus(serv.get(), &status);
		return ret != FALSE;
	}
	catch (...)
	{
		return false;
	}
}

DWORD SCMWrapper::GetServiceStatus(const std::wstring& serviceName)
{
	ServiceHandle serv(hSCM, serviceName, SERVICE_QUERY_STATUS);
	SERVICE_STATUS status;
	BOOL ret = ::QueryServiceStatus(serv.get(), &status);
	if (ret == FALSE)
	{
		return SERVICE_STATUS_FAIL;
	}
	else
	{
		return status.dwCurrentState;
	}
}

bool SCMWrapper::StartService(const std::wstring& serviceName)
{
	ServiceHandle serv(hSCM, serviceName, SERVICE_START);
	BOOL ret = ::StartService(serv.get(), NULL, NULL);
	return ret != FALSE;
}

bool SCMWrapper::StopService(const std::wstring& serviceName) 
{
	ServiceHandle serv(hSCM, serviceName, SERVICE_STOP);
	SERVICE_STATUS status;
	BOOL ret = ::ControlService(serv.get(), SERVICE_CONTROL_STOP, &status);
	return ret != FALSE;
}

