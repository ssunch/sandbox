#ifdef _WIN32
#include <windows.h>
#elif __linux__ 
#include <unistd.h>
#elif __APPLE__
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "include/core.h"


int getCoreNumber(void)
{
#ifdef WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
#elif __linux__
	return sysconf(_SC_NPROCESSORS_ONLN);
#elif __APPLE__
	return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}
