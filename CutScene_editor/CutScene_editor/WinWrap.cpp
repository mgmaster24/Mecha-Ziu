////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File: "WinWrap.cpp”
// Author: Anton Burdukovskiy
// Purpose: This file is used to hide the initialization code for a creating a window. 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WinWrap.h"
#include <stdlib.h>
#include <crtdbg.h>
#include "RenderEngine.h"


char* ShortenPath(char* szPath)
{
	int nIndex = 0; 

	for(int i=0; szPath[i]; ++i)
		if(szPath[i] == '\\' || szPath[i] == '/')
			nIndex = i;

	return &(szPath[nIndex+1]); 
}

CWinWrap::CWinWrap(void)
{
	m_hAppInstance = NULL;
	m_hWnd = NULL;
	m_bWindowed = false;
	m_nScreenWidth = 0, m_nScreenHeight = 0;
	m_bAppPaused = false;
}

CWinWrap::~CWinWrap(void) 
{

}

CWinWrap* CWinWrap::GetInstance()
{
	static CWinWrap instance; 
	return &instance; 
}

bool CWinWrap::InitApp(int nScreenWidth, int nScreenHeight, bool bWindowed, HINSTANCE hAppInstance, LPCSTR szWindowTitle)
{	
	//Store our data
	m_hAppInstance = hAppInstance;								//store the Application Instance Handle
	m_nScreenHeight = nScreenHeight;							//store the Screen Height
	m_nScreenWidth = nScreenWidth;								//store the Screen Width 
	m_bWindowed	= bWindowed;									//store the Window Status 
	ShowCursor(true); 

	//Create and Zero Out a Window Class
	WNDCLASSEX  wndClass; ZeroMemory( &wndClass, sizeof( wndClass ) );

	//Setup up the Class
	wndClass.cbSize         = sizeof( WNDCLASSEX );					// size of window structure
	wndClass.lpfnWndProc    = ( WNDPROC ) WndProc;					// message callback
	wndClass.lpszClassName  = "Application";						// class name
	wndClass.hInstance      = hAppInstance;							// handle to application
	//wndClass.hIcon
	wndClass.hCursor        =  LoadCursor( NULL, IDC_ARROW );		// default cursor
	wndClass.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);  // background brush

	//Register the Class
	if ( !RegisterClassEx( &wndClass ) )
		return false; 

	// Setup window style flags
	DWORD dwWindowStyleFlags = m_bWindowed ? (WS_OVERLAPPEDWINDOW | WS_VISIBLE) : ( WS_POPUP | WS_VISIBLE );

	// Setup the desired client area size
	RECT rWindow;
	rWindow.left	= 0;
	rWindow.top		= 0;
	rWindow.right	= m_nScreenWidth;
	rWindow.bottom	= m_nScreenHeight;

	// Get the rect of a window that has the desired client area we just set up. 
	AdjustWindowRectEx(
		&rWindow,										// The Client Rect  
		dwWindowStyleFlags,								// The Window Style Flags that we set up 
		FALSE,											// We don't have a menu
		WS_EX_APPWINDOW);								// Extended Window Style Flag (See MSDN if you really want to know)

	// Calculate the width/height of the new window's dimensions
	int windowWidth		= rWindow.right - rWindow.left;
	int windowHeight	= rWindow.bottom - rWindow.top;

	// create window of registered type
	if(!(m_hWnd = CreateWindowEx(					
		WS_EX_APPWINDOW,												// Extended Window Style Flag (See MSDN if you really want to know)
		"Application", szWindowTitle,									// class name, window title 
		dwWindowStyleFlags,												// window style
		0,			// Window Position X in the middle of the screen 
		0,			// Window Position Y in the middle of the screen
		windowWidth, windowHeight,                         				// width & height of window
		NULL, NULL,                                          			// parent window & menu
		m_hAppInstance,                                      			// handle to application
		NULL )))														// Just Leave This NULL
	{
		return false;													// If CreateWindowEx() Fails, Bail out of the application
		ERROR_MESSAGE("CreateWindowEx() Failed in CWinWrap::InitApp()"); 
	}

	//Check for memory leaks when the program exists if you're in debug
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif 

	return true;														// All is good :) 
}

void CWinWrap::ShutDown()
{
	UnregisterClass( "Application", m_hAppInstance );
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	// Is the application in a minimized or maximized state?
	static bool minOrMaxed = false;
	RECT clientRect = {0, 0, 0, 0};

	switch ( message )
	{
		// WM_ACTIVE is sent when the window is activated or deactivated.
		// We pause the game when the main window is deactivated and 
		// unpause it when it becomes active.
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
			CWinWrap::GetInstance()->m_bAppPaused = true;
		else
			CWinWrap::GetInstance()->m_bAppPaused = false;
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		if( CRenderEngine::GetInstance()->GetDevice() )
		{
			CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth  = LOWORD(lParam);
			CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight = HIWORD(lParam);

			if( wParam == SIZE_MINIMIZED )
			{
				CWinWrap::GetInstance()->m_bAppPaused = true;
				minOrMaxed = true;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				CWinWrap::GetInstance()->m_bAppPaused = false;
				minOrMaxed = true;
				CRenderEngine::GetInstance()->OnLostDevice();
				HR(CRenderEngine::GetInstance()->GetDevice()->Reset(CRenderEngine::GetInstance()->GetPresentParams()));
				CRenderEngine::GetInstance()->OnResetDevice();
			}
			// Restored is any resize that is not a minimize or maximize.
			// For example, restoring the window to its default size
			// after a minimize or maximize, or from dragging the resize
			// bars.
			else if( wParam == SIZE_RESTORED )
			{
				CWinWrap::GetInstance()->m_bAppPaused = false;

				// Are we restoring from a mimimized or maximized state, 
				// and are in windowed mode?  Do not execute this code if 
				// we are restoring to full screen mode.
				if( minOrMaxed && CRenderEngine::GetInstance()->GetPresentParams()->Windowed )
				{
					CRenderEngine::GetInstance()->OnLostDevice();
					HR(CRenderEngine::GetInstance()->GetDevice()->Reset(CRenderEngine::GetInstance()->GetPresentParams()));
					CRenderEngine::GetInstance()->OnResetDevice();
				}
				else
				{
					// No, which implies the user is resizing by dragging
					// the resize bars.  However, we do not reset the device
					// here because as the user continuously drags the resize
					// bars, a stream of WM_SIZE messages is sent to the window,
					// and it would be pointless (and slow) to reset for each
					// WM_SIZE message received from dragging the resize bars.
					// So instead, we reset after the user is done resizing the
					// window and releases the resize bars, which sends a
					// WM_EXITSIZEMOVE message.
				}
				minOrMaxed = false;
			}
		}
		return 0;

	case WM_EXITSIZEMOVE:
		GetClientRect(CWinWrap::GetInstance()->GetWindowHandle(), &clientRect);
		CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth  = clientRect.right;
		CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight = clientRect.bottom;
		CRenderEngine::GetInstance()->OnLostDevice();
		HR(CRenderEngine::GetInstance()->GetDevice()->Reset(CRenderEngine::GetInstance()->GetPresentParams()));
		CRenderEngine::GetInstance()->OnResetDevice();

		return 0;

	case WM_DESTROY: { PostQuitMessage( 0 ); }
					 break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}