/*********************************************************************************
Filename:		WinWrap.h
Description:	This class contains all the data and methods necessary for win32 
				initialization.
*********************************************************************************/

#ifndef CWINWRAP_H_
#define CWINWRAP_H_

#ifndef ERROR_MESSAGE
#define ERROR_MESSAGE(e) MessageBox(CWinWrap::GetInstance()->GetWindowHandle(), e, "Error", MB_OK); 
#endif 

#include <windows.h>

//This is the Message Handler. 
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

class CWinWrap
{
private:

	HINSTANCE   m_hAppInstance;								//Handle to our Application 
	HWND        m_hWnd;										//Handle to our Window
	bool		m_bWindowed;								//Is the application running in Windowed or Full Screen Mode. 
	int			m_nScreenWidth, m_nScreenHeight;			//Window Width and Height

	/*************************   CONSTRUCTORS   ***************************/

	//There can only be one instance. Use GetInstance() to get access to it. 

	CWinWrap(void);
	CWinWrap(const CWinWrap& ); 
	CWinWrap& operator=(const CWinWrap& );

	/**********************************************************************/

public:	

	bool		m_bAppPaused;								//Specifies if the app is active or paused
	
	virtual ~CWinWrap(void);

	//The only access point to this class 
	static CWinWrap* GetInstance(); 

	/**************************  ACCESSORS *****************************/

	inline HINSTANCE	GetAppInstance()	const { return m_hAppInstance;	}
	inline HWND			GetWindowHandle()	const { return m_hWnd;			}
	inline bool			GetWindowedStatus()	const { return m_bWindowed;		}
	inline int			GetWindowWidth()	const { return m_nScreenWidth;	}
	inline int			GetWindowHeight()	const { return m_nScreenHeight; }
	inline bool			IsAppPaused()		const { return m_bAppPaused;	}

	/*******************************************************************/

	//Used to Initialize and Show the Window
	bool InitApp(int nScreenWidth, int nScreenHeight, bool bWindowed, HINSTANCE hAppInstance, LPCSTR szWindowTitle);

	//The function this class needs to call when it is no longer needed. 
	void ShutDown();
};

#endif 
