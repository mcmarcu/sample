Code samples:
1. sample1
	naturalsort.h naturalsort.cpp
	This is part of a patch I proposed for the FileZilla FTP client that at that time did not have a natural sort order on the file names.
	For a quick overview of the problem you can check http://blog.codinghorror.com/sorting-for-humans-natural-sort-order/
	FileZilla is multi-platform and uses a crosscompiler and mingw to generate windows binaries.
	I had to do a workaround so that I can call StrCmpLogicalW on windows machines, see the WinSorter class.
	
2. sample2
	SCMWrapper.h SCMWrapper.cpp SCMTest.cpp
	Wrapper for working with windows services.
	
3. sample3
	proxy.h proxy.cpp proxytest.cpp
	Get the system proxy on linux systems (GNOME2/3, KDE, SLES, etc)
	
4. sample4
	classdiagram.png serverusage.h usagemetric.h metricalgorithms.h metricalgorithms.cpp
	ServerUsage allows monitoring the resource usage on a linux system. I takes samples every N seconds and returns average loads on specified timespans.
	Three Metrics are implemented: CPU, Memory and LoadAverage. 
	Adding more is easy by module design flexibility: 
			- just derive on the IMetricAlgorithm interface, instantiate and add your class to the metric list in ServerUsage::InitMetrics. 