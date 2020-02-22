#ifdef _DEBUG
#include <vld.h>
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

//#define _GONE_GOLD
//#include "CLogger.h"

#include "WinWrap.h"
#include "Game.h"
#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600
#define WINDOWED		true

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

int main()
{
	// Get the handle to the instance of this console application
	char t[500];
    GetConsoleTitleA( t, 500 ); 	
	HWND hWnd = FindWindowA( NULL, t );
	MoveWindow(hWnd, 805, 0, 700, 400, true);
	LONG test = GetWindowLong(hWnd, GWL_HINSTANCE);
	HINSTANCE hInstance = HINST_THISCOMPONENT; 
	CWinWrap* pApp = CWinWrap::GetInstance(); 
	
	//	Open and Init the Logger
	//CLogger::OpenLog("SH_LogData.txt");

	//1. Window Initialization 
	if( ! pApp->InitApp(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOWED, hInstance, "Mecha-Ziu") )
		return 0; 

	//2. Game Initialization
	CGame *game = CGame::GetInstance();
	game->Init(pApp->GetWindowHandle(),SCREEN_HEIGHT, SCREEN_WIDTH, WINDOWED, false);

	//	Write out log data
	//CLogger::LogData("Mech-Ziu Inited");

	//3. Main Message Loop
	MSG msg; ZeroMemory( &msg, sizeof( msg ) );
	while ( msg.message != WM_QUIT )                                                                                                                         
    {
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        { 
            TranslateMessage( &msg ); DispatchMessage( &msg ); 
        }
		//Game Main goes here
		if(game->Run() == GAME_SHUTDOWN)
		{
			break;
		}
    }

	//4. Unregister Everything, De-Allocate Memory and ShutDown
	game->Shutdown();

	//	Write out log data
	//CLogger::LogData("Mech-Ziu ShutDown");

	//	Shutdown the logger and close the log
	//CLogger::CloseLog();

	return 0; 
}
