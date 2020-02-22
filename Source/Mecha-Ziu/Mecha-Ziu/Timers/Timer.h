#ifndef TIMER_H_
#define TIMER_H_

#include <windows.h>

class CTimer
{
	LARGE_INTEGER	m_liHertz,						
					m_liStartTime,					
					m_liEndTime;
	double			m_dElapsedTime,
					m_dSecondsPerTick;
	bool			m_bActive;

public:

	inline double GetElapsedTime() { return m_dElapsedTime; }

	void	Start(); 
	void	Stop(); 
	void	Reset(); 
	
	CTimer(void);
	~CTimer(void);
};

#endif 