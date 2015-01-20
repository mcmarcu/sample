#ifndef SERVERUSAGE_H
#define SERVERUSAGE_H

#include <list>
#include <boost/thread.hpp>
#include "osent.h"
#include "usagemetric.h"




class ServerUsage
{

public:
	ServerUsage(int sampleFrequenySeconds):m_sampleFrequency(sampleFrequenySeconds*1000)
	{
		InitMetrics();
		StartMonitoring();
	}
	~ServerUsage()
	{
		StopMonitoring();
		UninitMetrics();
	}
	
	void GetStatus(time_t start, time_t end, std::list<std::pair<int,std::string>>& status)
	{
		status.clear();
		for( std::list<UsageMetric>::iterator it = m_metrics.begin(); it != m_metrics.end(); ++it )
		{
			status.push_back( std::make_pair( it->GetAverage(start,end), it->GetName()) );
		}

		AddOverallUsage(status);
	}

	void RemoveOldSamples(time_t limit)
	{
		for( std::list<UsageMetric>::iterator it = m_metrics.begin(); it != m_metrics.end(); ++it )
		{
			it->RemoveOldSamples(limit);
		}

	}

private:

	void AddOverallUsage(std::list<std::pair<int,std::string>>& status)
	{
		int count = 0;
		int sum = 0;
		
		for( std::list<std::pair<int,std::string>>::iterator it = status.begin(); it != status.end(); ++it ) 
		{
				sum+=it->first;
				count++;
		}
		status.push_back( std::make_pair(sum/count, "overallUsage") );
	}
	
	void InitMetrics ()
	{
		boost::shared_ptr<IMetricAlgorithm> loadAverage(new LoadAverageMetric());
		m_metrics.push_back(UsageMetric(loadAverage));

		boost::shared_ptr<IMetricAlgorithm> memory(new MemoryMetric());
		m_metrics.push_back(UsageMetric(memory));

		boost::shared_ptr<IMetricAlgorithm> cpu(new CPUMetric());
		m_metrics.push_back(UsageMetric(cpu));

	}
	void UninitMetrics ()
	{
		m_metrics.clear();
	}
	
	std::list<UsageMetric> 	m_metrics;
	
	void StartMonitoring()
	{
		CREATE_EVENT(m_wakeUp, false, false);
		m_stop = false;
		m_monitorThread = boost::thread( boost::bind( &ServerUsage::MonitorThread, this ) );
	}
	
	void StopMonitoring()
	{
		
		m_stop = true;
		SET_EVENT(m_wakeUp);

		try
		{
			m_monitorThread.join();
		}
		catch(std::exception&)
		{
		}

		DESTROY_EVENT(m_wakeUp);

	}
	
	int 					m_sampleFrequency;
	boost::thread 			m_monitorThread;
	EVENT_T 				m_wakeUp;
	bool    				m_stop;
	
	
	void MonitorThread()
	{
		do
		{	
			for( std::list<UsageMetric>::iterator it = m_metrics.begin(); it != m_metrics.end(); ++it ) 
			{
				it->DoSample();
			}
			
			WAIT_EVENT(m_wakeUp, m_sampleFrequency);
		
		} while (!m_stop);
	}
};

#endif
