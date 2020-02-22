/**************************************************************************
File Name	:	CLogger

Description	:	Write error of system information out to a file
**************************************************************************/
#ifndef COLD_LOGGER_H
#define COLD_LOGGER_H
#include <fstream>

#ifdef _GONE_GOLD

#define CLogger::LogData(x) x
#define CLogger::OpenLog(x) x
#define CLogger::CloseLog() 

#else 

class CLogger
{
private:
#ifndef _GONE_GOLD
	static std::fstream CLogger::m_fsLogger;
#endif
private:
	//	Constructor, copy constructor and assignment operator
	CLogger(void);
	CLogger(CLogger&);
	CLogger &operator=(CLogger&);
	CLogger *operator=(CLogger*);
public:
	//	Destructor
	~CLogger(void);

	/**************************************************************************
	Description	:	Open a file to write data to
	**************************************************************************/
	static bool OpenLog(const char *szFile);

	/**************************************************************************
	Description	:	Write data out to a file
	**************************************************************************/
	static void LogData(const char *szStatus);

	/**************************************************************************
	Description	:	Close the logging file
	**************************************************************************/
	static void CloseLog(void);
};

#endif   // _GONE_GOLD

#endif	 // COLD_LOGGER_H