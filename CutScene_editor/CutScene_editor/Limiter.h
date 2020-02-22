#ifndef LIMITER_H_
#define LIMITER_H_

#include <windows.h>

class CLimiter
{
	double				m_dElapsedTime;
	LARGE_INTEGER		m_liHertz,						
						m_liEndTime,					
						m_liStartTime;					
public:

	CLimiter(void);
	~CLimiter(void);

	bool Limit(double dInterval); 
};

#endif 