/*********************************************************************************
Filename:		DirectInput.h
Description:	This class will be responsible for handling keyboard,
				mouse and joystick input.
				Credit to Brad Blake at CodeGuru.com and Anton Burdukovskiy
*********************************************************************************/
#ifndef DIRECTINPUT_H_
#define DIRECTINPUT_H_

#define DIRECTINPUT_VERSION 0x0800 

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

enum USING { USEKEYBOARD = 1, USEMOUSE = 2, USEJOYSTICK = 4 };
enum MOUSE { LEFTCLICK = 0, RIGHTCLICK };
enum JOYPAD{ A = 0, B, X, Y, LEFT_BUMPER, RIGHT_BUMPER, BACK, START, LEFT_TOGGLE, RIGHT_TOGGLE };

class CDirectInput
{
	private:
	
	/*Singleton*******************/
	CDirectInput(void);
	virtual ~CDirectInput(void);
	/*****************************/

	//8 Element Array, TRUE if Button [n] is pressed
	BOOL*					m_bMouseButton;
	bool					m_bJoyButton[16];

	//Device objects
	LPDIRECTINPUTDEVICE8	m_pDIJoy;
	LPDIRECTINPUTDEVICE8	m_pDIKeybrd;
	LPDIRECTINPUTDEVICE8	m_pDIMouse;

	//Device State Buffers
	DIJOYSTATE2				m_DIJoyState;
	DIMOUSESTATE2			m_DIMouseState;
	char					m_szKeyState[256];
	HKL						m_hklLayout;
	DWORD					m_dwElements;
	DWORD					m_dwMouseElements;
	DIDEVICEOBJECTDATA		m_BuffKey[256];		
	DIDEVICEOBJECTDATA		m_BuffMouse[256];	
	bool					m_bKeysDown[256]; 

	//DI Object
	IDirectInput8			*m_pDI;

	//DI Device Capabilities
	DIDEVCAPS				m_DIDevCaps;
	bool					m_bActiveDevice[3];

	/********************************************************************************
	Description:	Shutdown direct input
	********************************************************************************/
	void DIShutdown();

	/********************************************************************************
	Description: Joystick callbacks
	*********************************************************************************/
	friend BOOL __stdcall EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
	friend BOOL __stdcall EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );	

	public:
	/********************************************************************************
	Description:	Get an instance to the class
	********************************************************************************/
	static CDirectInput *GetInstance(void);

	/********************************************************************************
	Description:	Initializes direct input
	NOTE:			ToUse = (USEMOUSE | USEKEYBOARD | USEJOYSTICK)
	********************************************************************************/
	bool InitDI(HWND hWnd, HINSTANCE hInstance, DWORD ToUse);

	/********************************************************************************
	Description:	Polls the respective device for input.
					NOTE: virtual 
	********************************************************************************/
	virtual HRESULT ProcessMouse();
	virtual HRESULT ProcessKB();
	virtual HRESULT ProcessJoy();
	virtual void	ProcessDevices();

	/********************************************************************************
	Description:	Returns true if the user pressed cKey.
	********************************************************************************/
	bool	CheckKey(char cKey);
	
	/********************************************************************************
	Description:	Returns the last key the user pressed 
					with buffering action!
	********************************************************************************/
	char	GetLastKey(void);

	/********************************************************************************
	Description:	Returns true if the user pressed cKey.
	********************************************************************************/
	bool	CheckBufferedKey(char cKey);

	/********************************************************************************
	Description:	Returns the last key the user pressed 
					with buffering action!
	********************************************************************************/
	char	GetLastBufferedKey(void);

	/********************************************************************************
	Description:	Returns true if the user pressed bButton
					enum MOUSE { LEFTCLICK = 0, RIGHTCLICK = 1};			
	********************************************************************************/
	bool	CheckMouse(BYTE bButton);

	/********************************************************************************
	Description:	Returns the last key the user pressed 
					with buffering action!
					enum MOUSE { LEFTCLICK = 0, RIGHTCLICK = 1};			
	********************************************************************************/
	bool	CheckBufferedMouse(BYTE button);
	
	/********************************************************************************
	Description:	Returns the current state of the mouse 
					(x,y,z - scroll wheel)
	********************************************************************************/
	void	GetMouseMove(D3DXVECTOR3 *axis);

	/********************************************************************************
	Description:	Returns true if the mouse is untouched
	********************************************************************************/
	bool AllMouseUp();

	/********************************************************************************
	Description:	Returns true if mButton was released
	********************************************************************************/
	bool CheckMouseButtonRelease(BYTE mButton);

	/********************************************************************************
	Description:	Returns true if the user pressed bButton
	enum JOYPAD{ A = 0, B, X, Y, LEFT_BUMPER, RIGHT_BUMPER, BACK, START, LEFT_TOGGLE, RIGHT_TOGGLE };
	********************************************************************************/
	bool	CheckJoypad(BYTE bButton);

	/********************************************************************************
	Description:	Returns true if the user pressed bButton
					with buffering action!
	enum JOYPAD{ A = 0, B, X, Y, LEFT_BUMPER, RIGHT_BUMPER, BACK, START, LEFT_TOGGLE, RIGHT_TOGGLE };
	********************************************************************************/
	bool	CheckBufferedJoypad(BYTE bButton);

	/********************************************************************************
	Description:	Returns true if mButton was released
	********************************************************************************/
	bool CheckJoypadButtonRelease(BYTE mButton);

	/********************************************************************************
	Description:	Returns the normalized direction vector for the left and right triggers
	********************************************************************************/
	void CheckTriggers(float &vDir);

	/********************************************************************************
	Description:	Returns the normalized direction vector for the left 
					control stick.
	********************************************************************************/
	void GetLeftDir(D3DXVECTOR2 *vDir);

	/********************************************************************************
	Description:	Returns the normalized direction vector for the right 
					control stick.
	********************************************************************************/
	void GetRightDir(D3DXVECTOR2 *vDir);
};


#endif // DIRECTINPUT_H_
