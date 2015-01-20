#ifndef METRICALG_H
#define METRICALG_H

#include <string>

struct Sample
{
	Sample(int v, time_t t):value(v),time(t)
	{
	}
	
	
	bool operator < ( const Sample &r ) const
	{
		return time < r.time;
	}
	
	
	int value;
	time_t time;
};


class IMetricAlgorithm
{
public:
	virtual std::string GetName() = 0;
	virtual Sample 		GetSample() = 0;
	
};

class LoadAverageMetric : public IMetricAlgorithm
{
public:
	virtual std::string GetName();
	virtual Sample GetSample();
};

class MemoryMetric : public IMetricAlgorithm
{
public:
	virtual std::string GetName();
	virtual Sample GetSample();
private:
	int GetMemoryKb( const char* a_szMem );
};

class CPUMetric : public IMetricAlgorithm
{
public:
	virtual std::string GetName();
	virtual Sample GetSample();
private:
	unsigned long GetCpuWorkJiffies();
	unsigned long GetCpuTotalJiffies();
	int GetCPULoad( unsigned long a_nCpuWorkJiffiesOld, unsigned long a_nCpuTotalJiffiesOld, unsigned long a_nCpuWorkJiffiesNew, unsigned long a_nCpuTotalJiffiesNew );
};

#endif
