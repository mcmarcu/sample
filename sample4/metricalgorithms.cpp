#include <string>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "metricalgorithms.h"


#define MAX_BUFFER 256

std::string LoadAverageMetric::GetName()
{
	return "loadAverage";
}
	
Sample LoadAverageMetric::GetSample()
{
	int nCores = sysconf( _SC_NPROCESSORS_ONLN );

	int nsamples = 3;
	double loadavg[3];
	double cpuLoad = 0;
	if ( getloadavg( loadavg, nsamples ) != -1 )
	{
		cpuLoad = loadavg[2];
	}

	return Sample((cpuLoad*100)/nCores, time(NULL));
}


std::string MemoryMetric::GetName()
{
	return "memoryUsage";
}

int MemoryMetric::GetMemoryKb( const char* a_szMem )
{
	int nResult = 0;
	FILE * fp = NULL;
	char szRes[MAX_BUFFER] = {0};
	char szCmd[MAX_BUFFER] = {0};

	do 
	{
		if ( NULL == a_szMem || 0 == a_szMem[0] )
			break;

		sprintf( szCmd, "grep -i %s /proc/meminfo | cut -d ':' -f 2- | grep [0-9]* -o", a_szMem );

		fp = popen( szCmd, "r" );
		if ( NULL != fp )
		{
			fread(szRes, 1, sizeof(szRes)-1, fp);
			fclose(fp);
		}

		if ( 0 < strlen(szRes) )
		{			
			nResult = atoi( szRes );
		}
	} 
	while ( 0 );

	return nResult;
}
	
Sample MemoryMetric::GetSample()
{
	int totalMem = GetMemoryKb( "memtotal" );

	int freeMem = GetMemoryKb( "memfree" );

	if ( 0 == totalMem )
		return Sample(0, time(NULL));

	double dResult = (double) freeMem / (double) totalMem * 100;

	int memUsed = 100 - ceil( dResult );

	return Sample(memUsed, time(NULL));
}


std::string CPUMetric::GetName()
{
	return "cpuUsage";
}

int CPUMetric::GetCPULoad( unsigned long a_nCpuWorkJiffiesOld, unsigned long a_nCpuTotalJiffiesOld, unsigned long a_nCpuWorkJiffiesNew, unsigned long a_nCpuTotalJiffiesNew )
{
	int nCpuLoad = 0;

	do 
	{
		if ( 0 == a_nCpuWorkJiffiesOld && 0 == a_nCpuTotalJiffiesOld )
			break;

		unsigned long nWorkOverPeriod = a_nCpuWorkJiffiesNew - a_nCpuWorkJiffiesOld;
		unsigned long nTotalPeriod = a_nCpuTotalJiffiesNew - a_nCpuTotalJiffiesOld;

		if ( 0 == nTotalPeriod )
			break;

		double lResult = ((double) nWorkOverPeriod / nTotalPeriod ) * 100;
		nCpuLoad = ceil( lResult );
	} 
	while ( 0 );
	
	return nCpuLoad;
}

unsigned long CPUMetric::GetCpuWorkJiffies()
{
	unsigned long nResult = 0;
	int nTemp = 0;

	for ( int i = 3; i < 6; i++ )
	{
		FILE * fp;
		char szRes[MAX_BUFFER] = {0};
		char szCmd[MAX_BUFFER] = {0};

		sprintf( szCmd, "grep \'\\<cpu\\>\' /proc/stat | cut -d \' \' -f %d", i );

		fp = popen( szCmd, "r" );
		if ( NULL != fp )
		{
			fread(szRes, 1, sizeof(szRes)-1, fp);
			fclose(fp);
		}

		if ( 0 < strlen(szRes) )
		{			
			nTemp = atoi( szRes );
			nResult += nTemp;
		}
	}

	return nResult;
}

unsigned long CPUMetric::GetCpuTotalJiffies()
{
	unsigned long nResult = 0;

	std::string strLine;
	std::string strDelim = " ";	

	int nTemp = 0;

	FILE * fp = NULL;
	char szRes[MAX_BUFFER] = {0};
	char szCmd[MAX_BUFFER] = "grep \'\\<cpu\\>\' /proc/stat | cut -c 6-";

	do 
	{
		fp = popen( szCmd, "r" );
		if ( NULL == fp )
			break;

		fread(szRes, 1, sizeof(szRes)-1, fp);
		fclose(fp);

		strLine = szRes;
		if ( strLine.empty() )
			break;

		// Skip delimiters at beginning.
		std::string::size_type lastPos = strLine.find_first_not_of(strDelim, 0);
		// Find first "non-delimiter".
		std::string::size_type pos     = strLine.find_first_of(strDelim, lastPos);

		while ( std::string::npos != pos || std::string::npos != lastPos )
		{
			// Found a token, add it to the vector.
			std::string strToken = strLine.substr( lastPos, pos - lastPos );
			nTemp = atoi( strToken.c_str() );

			nResult += nTemp;

			// Skip delimiters.  Note the "not_of"
			lastPos = strLine.find_first_not_of( strDelim, pos );
			// Find next "non-delimiter"
			pos = strLine.find_first_of( strDelim, lastPos );
		}
	} 
	while ( 0 );

	return nResult;
}
	
Sample CPUMetric::GetSample()
{

	unsigned long nCpuWorkJiffies1 = GetCpuWorkJiffies();
	unsigned long nCpuTotalJiffies1 = GetCpuTotalJiffies();
	
	sleep(5);
	
	unsigned long nCpuWorkJiffies2 = GetCpuWorkJiffies();
	unsigned long nCpuTotalJiffies2 = GetCpuTotalJiffies();	

	int cpuLoad = GetCPULoad( nCpuWorkJiffies1, nCpuTotalJiffies1, nCpuWorkJiffies2, nCpuTotalJiffies2 );

	return Sample(cpuLoad, time(NULL)); ;
}



