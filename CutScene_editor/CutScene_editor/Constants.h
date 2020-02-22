/**************************************************************************************************
Name:			Constants.h
Description:	This file includes all the constants and macros for our game.
**************************************************************************************************/

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

//Constants
const unsigned int MAX_LIGHTS = 8;
const unsigned int MAX_PATH_LENGTH = 128;  
const unsigned int MAX_NAME_LENGTH = 32;
const unsigned int NUM_PLAYABLE_ANIMS = 10;

//Macros
#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

#define RR(low, high)	((rand() % (high - low)) + low)
#define RAND_FLOAT(min,max) (((rand()/(float)RAND_MAX)*((max)-(min)))+(min))

#ifndef ERROR_MESSAGE
#include "WinWrap.h"
#define ERROR_MESSAGE(e) MessageBox(CWinWrap::GetInstance()->GetWindowHandle(), e, "Error", MB_OK); 
#endif

char* ShortenPath(char*);

#ifdef _DEBUG
	#include <dxerr9.h>
	#include <iostream>
	using std::cout;
	#ifndef HR
	#define HR(x)                                     		\
	{                                                 		\
		HRESULT hr = x;                               		\
		if(FAILED(hr))                                		\
		{                                             		\
			DXTrace(__FILE__, __LINE__, hr, #x, FALSE);		\
			cout <<	"\n*D3DError*:" <<						\
			"\nID: " << DXGetErrorString9(hr) <<			\
			"\nFile: " << ShortenPath(__FILE__) <<			\
			"\nLine#:" << __LINE__ << "\n\n";				\
		}													\
	}
	#endif
#else
	#ifndef HR
	#define HR(x) x;
	#endif
#endif 
#endif