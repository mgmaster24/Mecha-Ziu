#include "FrameCounter.h"
#include <iostream>

CFrameCounter::CFrameCounter(void)
{
	using std::cout;

	m_liHertz.QuadPart = 0; 
	m_liEndTime.QuadPart = 0; 
	m_liStartTime.QuadPart = 0; 
	m_dElapsedTime = 0.0;
	m_dDeltaTime = 0.0;
	m_unFPS = 0;
	m_unFrameCounter = 0;

	if(!QueryPerformanceFrequency(&m_liHertz))
		cout << "<Error> CFrameCounter Constructor: High Resolution Timer is not supported by the hardware.\n";

	m_dSecondsPerTick = 1.0 / m_liHertz.QuadPart;
}

CFrameCounter::~CFrameCounter(void)
{

}

void CFrameCounter::Count()
{
	QueryPerformanceCounter(&m_liStartTime);
	
	if(m_liEndTime.QuadPart)
		m_dDeltaTime = double(m_liStartTime.QuadPart - m_liEndTime.QuadPart) * m_dSecondsPerTick; 

	m_dElapsedTime += m_dDeltaTime; 
	m_unFrameCounter++; 

	if(m_dElapsedTime >= 1.0)
	{
		m_unFPS = m_unFrameCounter;
		m_unFrameCounter = 0; 
		m_dElapsedTime -= 1.0; 
	}

	m_liEndTime = m_liStartTime;
}

void CFrameCounter::Reset()
{
	m_dDeltaTime = 0.0; 
	m_dElapsedTime = 0.0; 
	m_unFPS = 0;
	QueryPerformanceCounter(&m_liEndTime);
}