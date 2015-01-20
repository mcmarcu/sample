#ifndef USAGEMETRIC_H
#define USAGEMETRIC_H

#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include "metricalgorithms.h"

class UsageMetric
{
public:
	UsageMetric(boost::shared_ptr<IMetricAlgorithm> alg):m_metricAlg(alg)
	{
	}

	~UsageMetric()
	{
	}

	UsageMetric(const UsageMetric& r):m_samples(r.m_samples),m_metricAlg(r.m_metricAlg)
	{
	}
	

	int GetAverage(time_t start, time_t stop)
	{
		boost::lock_guard<boost::mutex> guard(m_samplesMutex);
		unsigned long sum = 0;
		unsigned long count = 0;
		for( std::set<Sample>::iterator it = m_samples.begin(); it != m_samples.end(); ++it ) 
		{
			if( it->time >= start && it->time <= stop ) 
			{
				sum += it->value;
				count ++;
			}
		}
		
		return count ? sum/count : 0;
	}
	
	void RemoveOldSamples(time_t limit)
	{
		boost::lock_guard<boost::mutex> guard(m_samplesMutex);
		for( std::set<Sample>::iterator it = m_samples.begin(); it != m_samples.end(); /* blank */ ) 
		{
			if( it->time < limit ) 
			{
				m_samples.erase( it++ );
			}
			else 
			{
				++it;
			}
		}
	}
	
	void DoSample()
	{
		boost::lock_guard<boost::mutex> guard(m_samplesMutex);
		m_samples.insert(m_metricAlg->GetSample());
	}
	
	std::string GetName()
	{
		return m_metricAlg->GetName();
	}

private:
	std::set<Sample> 	m_samples;
	boost::mutex		m_samplesMutex;
	boost::shared_ptr<IMetricAlgorithm>	m_metricAlg; 
	
};

#endif
