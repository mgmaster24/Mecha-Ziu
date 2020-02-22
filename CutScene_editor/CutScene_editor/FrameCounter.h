#ifndef FRAMECOUNTER_H_
#define FRAMECOUNTER_H_

#include <windows.h>

class CFrameCounter
{
	unsigned int	m_unFPS,
					m_unFrameCounter; 
	double			m_dElapsedTime,
					m_dSecondsPerTick,
					m_dDeltaTime;
	LARGE_INTEGER	m_liHertz,						
					m_liStartTime,					
					m_liEndTime;	
public:

	CFrameCounter(void);
	~CFrameCounter(void);

	inline unsigned int GetFPS()		{ return m_unFPS; }
	inline double		GetDeltaTime()	{ return m_dDeltaTime; }
	
	void Count(); 
	void Reset();
};

#endif 