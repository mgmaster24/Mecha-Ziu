#include "CLogger.h"
#include <iostream>

#ifndef _GONE_GOLD
//	Stream for writting to a file
std::fstream CLogger::m_fsLogger;
#endif

CLogger::CLogger(void)	{}

CLogger::~CLogger(void)	{}

bool CLogger::OpenLog(const char *szFile)
{
#ifndef _GONE_GOLD
	//	Open the file
	m_fsLogger.open(szFile, std::ios_base::out | std::ios_base::trunc);

	//	Check if the file is open
	return m_fsLogger.is_open();
#endif
}

void CLogger::LogData(const char *szStatus)
{
#ifndef _GONE_GOLD
	//	Write out the log data
	m_fsLogger << "File : " << __FILE__ << "\nLine :" << __LINE__ << 
		"\nLog : " << szStatus << '\n';
	std::cout << szStatus << '\n';
#endif
}

void CLogger::CloseLog(void)
{
#ifndef _GONE_GOLD
	//	Close the file
	m_fsLogger.close();
#endif
}