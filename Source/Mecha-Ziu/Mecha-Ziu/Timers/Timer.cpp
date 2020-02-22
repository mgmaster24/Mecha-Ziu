#include "Timer.h"
#include <iostream>

CTimer::CTimer(void)
{
	using std::cout; 

	m_liHertz.QuadPart = 0; 
	m_liEndTime.QuadPart = 0; 
	m_liStartTime.QuadPart = 0; 
	m_dElapsedTime = 0.0;
	m_bActive = false;

	if(!QueryPerformanceFrequency(&m_liHertz))
		cout << "<Error> CTimer Constructor: High Resolution Timer is not supported by the hardware.\n";

	m_dSecondsPerTick = 1.0 / m_liHertz.QuadPart;
}

CTimer::~CTimer(void)
{
}

void CTimer::Start()
{
	if(!m_bActive)
	{
		QueryPerformanceCounter(&m_liStartTime); 
		m_bActive = true;
	}	
}

void CTimer::Stop()
{
	if (m_bActive)
	{
		QueryPerformanceCounter(&m_liEndTime);				
		m_dElapsedTime += (double)(m_liEndTime.QuadPart - m_liStartTime.QuadPart) * m_dSecondsPerTick; 
		m_bActive = false;
	}
}

void CTimer::Reset()
{
	m_dElapsedTime = 0; 
}
