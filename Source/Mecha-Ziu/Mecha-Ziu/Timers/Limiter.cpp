#include "Limiter.h"
#include <iostream>

CLimiter::CLimiter(void)
{
	using std::cout;

	m_liHertz.QuadPart = 0; 
	m_liEndTime.QuadPart = 0; 
	m_liStartTime.QuadPart = 0; 
	m_dElapsedTime = 3600.0;			// initial amount in seconds, equal to an hour

	if(!QueryPerformanceFrequency(&m_liHertz))
		cout << "<Error> CLimiter Constructor: High Resolution Timer is not supported by the hardware.\n";
}

CLimiter::~CLimiter(void)
{
}

bool CLimiter::Limit(double dInterval)
{
	if(dInterval * m_liHertz.QuadPart <= (double)(m_liEndTime.QuadPart - m_liStartTime.QuadPart))
	{
		QueryPerformanceCounter(&m_liStartTime);
		return true;
	}
	else	
	{
		QueryPerformanceCounter(&m_liEndTime);
		return false;
	}
}