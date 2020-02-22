#include "DirectInput.h"
#include "Constants.h"

//Constuctor
CDirectInput::CDirectInput(void)
{
	//Init all
	m_pDI			= NULL;
	m_pDIKeybrd		= NULL;
	m_pDIMouse		= NULL;
	m_pDIJoy		= NULL;

	m_dwElements	= 16;

	m_bActiveDevice[0] = false;
	m_bActiveDevice[1] = false;
	m_bActiveDevice[2] = false;

	unsigned int i;
	for(i = 0; i < 256; ++i)
		m_bKeysDown[i] = false;

	m_bMouseButton	= new BOOL[8];
	for(i = 0; i < 8; ++i)
		m_bMouseButton[i] = FALSE;

	for(i = 0; i < 128; ++i)
		m_bJoyButton[i] = false;

}

//Destructor
CDirectInput::~CDirectInput(void)
{
	DIShutdown();
	
	if(m_bMouseButton)
	{
		for(unsigned int i = 0; i < 8; ++i)
			m_bMouseButton[i] = FALSE;

		delete m_bMouseButton;
	}
	
}

//Instance
CDirectInput *CDirectInput::GetInstance(void)
{
	static CDirectInput instance;
	return &instance;
}

//Init devices
bool CDirectInput::InitDI(HWND hWnd, HINSTANCE hInstance, DWORD ToUse)
{
	//Create the DI object
	HR(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, 0));

	if(!m_pDI)
		return false;

	//Init keyboard
	if(ToUse & USEKEYBOARD)
	{
		HR(m_pDI->CreateDevice(GUID_SysKeyboard, &m_pDIKeybrd, NULL));

		HR(m_pDIKeybrd->SetDataFormat(&c_dfDIKeyboard));

		HR(m_pDIKeybrd->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))); 

		//Keyboard (Mine)!
		if(m_pDIKeybrd)
			m_pDIKeybrd->Acquire();

		m_hklLayout = GetKeyboardLayout(0);
		m_bActiveDevice[0] = true;
	}

	//Init mouse
	if(ToUse & USEMOUSE)
	{
		HR(m_pDI->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL));
			
		HR(m_pDIMouse->SetDataFormat(&c_dfDIMouse2));

		HR(m_pDIMouse->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)));
		
		//Init the input buffer
		DIPROPDWORD dBuffer;
		dBuffer.diph.dwSize			= sizeof(DIPROPDWORD);
		dBuffer.diph.dwHeaderSize	= sizeof(DIPROPHEADER);
		dBuffer.diph.dwObj			= 0;
		dBuffer.diph.dwHow			= DIPH_DEVICE;
		dBuffer.dwData				= 16;

		//You fail
		HR(m_pDIMouse->SetProperty(DIPROP_BUFFERSIZE, &dBuffer.diph));
		
		//Mouse (Mine)!
		if(m_pDIMouse)
			m_pDIMouse->Acquire();

		m_bActiveDevice[1] = true;
	}

	//Init gamepad
	if(ToUse & USEJOYSTICK)
	{
		if(FAILED(m_pDI->CreateDevice(GUID_Joystick, &m_pDIJoy, NULL)))
			return true;

		HR(m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,
					NULL, DIEDFL_ATTACHEDONLY));

		HR(m_pDIJoy->SetDataFormat(&c_dfDIJoystick2));

		HR(m_pDIJoy->SetCooperativeLevel(hWnd, (DISCL_EXCLUSIVE | DISCL_FOREGROUND)));
		

		m_DIDevCaps.dwSize = sizeof(DIDEVCAPS);

		HR(m_pDIJoy->GetCapabilities(&m_DIDevCaps));
	
		HR(m_pDIJoy->EnumObjects(EnumAxesCallback, (VOID*)hWnd, DIDFT_AXIS));
		

		//Joypad (Mine)!
		if(m_pDIJoy)
			m_pDIJoy->Acquire();

		m_bActiveDevice[2] = true;
	}

	return true;
}

//Get keyboard input
HRESULT CDirectInput::ProcessKB()
{
	if(FAILED(m_pDIKeybrd->GetDeviceState(sizeof(m_szKeyState), (LPVOID)&m_szKeyState)))
	{
		if(FAILED(m_pDIKeybrd->Acquire()))
			return E_FAIL;
		if(FAILED(m_pDIKeybrd->GetDeviceState(sizeof(m_szKeyState), (LPVOID)&m_szKeyState)))
			return E_FAIL;
	}

	return S_OK;
}

//Get joypad input
HRESULT CDirectInput::ProcessJoy()
{
	if(FAILED(m_pDIJoy->Poll()))  
    {
        while(m_pDIJoy->Acquire() == DIERR_INPUTLOST)
		{
			//Loop 
		}

        return E_FAIL; 
    }

	if(FAILED(m_pDIJoy->GetDeviceState( sizeof(DIJOYSTATE2), &m_DIJoyState)))
        return E_FAIL;

	return S_OK;
}

//Get mouse input
HRESULT CDirectInput::ProcessMouse()
{
	HRESULT hr = m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_DIMouseState);
	if(hr == DIERR_INPUTLOST)
	{
		m_pDIMouse->Acquire();
		return E_FAIL;
	}
	
	m_dwMouseElements = (DWORD)256;
	hr = m_pDIMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), m_BuffMouse, &m_dwMouseElements, 0);
	if(hr == DIERR_INPUTLOST)
	{
		m_pDIMouse->Acquire();
		return E_FAIL;
	}

	return S_OK;
}

//If device is active, process 
void CDirectInput::ProcessDevices()
{
	if(m_bActiveDevice[0])
		HR(ProcessKB());

	if(m_bActiveDevice[1])
		HR(ProcessMouse());

	if(m_bActiveDevice[2])
		HR(ProcessJoy());
}

//Check for key press
bool CDirectInput::CheckKey(char cKey)
{	
	 return ((m_szKeyState[cKey] & 0x80) != 0 );
}

//Get last key pressed
char CDirectInput::GetLastKey()
{
	unsigned short	key;
	unsigned int	code;
	BYTE*			curState = NULL;

	GetKeyboardState(curState);

	for(int i = 0; i < 256; ++i)
	{
		if(m_szKeyState[i] & 0x80)
		{
			code = MapVirtualKeyEx(i, 1, m_hklLayout);
			ToAsciiEx(code, i, curState, &key, 0, m_hklLayout);
		}

	}

	return (char)key;
}

//Check for buffered key press
bool CDirectInput::CheckBufferedKey(char cKey)
{
	if((m_szKeyState[cKey] & 0x80) != 0)
	{
		if(!m_bKeysDown[cKey])
		{
			m_bKeysDown[cKey] = true;
			return true;
		}
	}
	else
		m_bKeysDown[cKey] = false;
	
	return false;
}

//Check mouse
bool CDirectInput::AllMouseUp()
{
	if(!m_DIMouseState.rgbButtons[0] && !m_DIMouseState.rgbButtons[1] &&
	   !m_DIMouseState.rgbButtons[2] && !m_DIMouseState.rgbButtons[3] &&
	   !m_DIMouseState.rgbButtons[4] && !m_DIMouseState.rgbButtons[5] &&
	   !m_DIMouseState.rgbButtons[6] && !m_DIMouseState.rgbButtons[7])
	   return true;

	return false;
}

//Check for mouse click
bool CDirectInput::CheckMouse(BYTE button)
{	
	 if(m_DIMouseState.rgbButtons[button] & 0x80)
		 return true;

	return false;
}

//Get current mouse position
void CDirectInput::GetMouseMove(D3DXVECTOR3 *axis)
{
	axis->x = (float)m_DIMouseState.lX;
	axis->y = (float)m_DIMouseState.lY;
	axis->z = (float)m_DIMouseState.lZ;
}

//Check for buffered mouse click
bool CDirectInput::CheckBufferedMouse(BYTE button)
{
	for(unsigned long i = 0; i < m_dwMouseElements; ++i)
	{
		if(m_BuffMouse[i].dwOfs == ((DWORD)(button+12)) && m_BuffMouse[i].dwData & 0x80)
			return true;
	}

	return false;
}

//Check for release
bool CDirectInput::CheckMouseButtonRelease(BYTE mButton)
{
	if(CheckMouse(mButton))
	{
		if(!m_bMouseButton[mButton])
			m_bMouseButton[mButton] = TRUE;
	}
	else
	{	
		if(m_bMouseButton[mButton])
		{
			m_bMouseButton[mButton] = FALSE;
			return true;
		}
	}

	return false;
}

bool CDirectInput::CheckJoypad(BYTE bButton)
{
	return ((m_DIJoyState.rgbButtons[bButton] & 0x80) != 0 );
}

bool CDirectInput::CheckBufferedJoypad(BYTE bButton)
{
	if(CheckJoypad(bButton))
	{
		if(!m_bJoyButton[bButton])
		{
			m_bJoyButton[bButton] = true;
			return true;
		}
	}
	else
	{
		m_bJoyButton[bButton] = false;
	}

	return false;
}

bool CDirectInput::CheckJoypadButtonRelease(BYTE bButton)
{
	if(CheckJoypad(bButton))
	{
		if(!m_bJoyButton[bButton])
		{
			m_bJoyButton[bButton] = true;
		}
	}
	else
	{
		if(m_bJoyButton[bButton])
			m_bJoyButton[bButton] = false;

		return true;
	}

	return false;

}
void CDirectInput::GetLeftDir(D3DXVECTOR2 *vDir)
{
	D3DXVECTOR2 vTemp;
	vTemp.x = (float)m_DIJoyState.lX;
	vTemp.y = (float)m_DIJoyState.lY;

	//X dead zone
	if(vTemp.x < 512 && vTemp.x > 0)
		vTemp.x = 0;
	else if(vTemp.x > -512 && vTemp.x < 0)
		vTemp.x = 0;

	//Y dead zone
	if(vTemp.y < 512 && vTemp.y > 0)
		vTemp.y = 0;
	else if(vTemp.y > -512 && vTemp.y < 0)
		vTemp.y = 0;
	
	D3DXVec2Normalize(vDir, &vTemp);
	//vDir = &vTemp;
}

void CDirectInput::GetRightDir(D3DXVECTOR2 *vDir)
{
/*
	static const float fCenter = 32767.0f;
	static const float fCenterInverse = 1 / (fCenter / 10);

	D3DXVECTOR2 vTemp;

	//Shift the dead zone to a more readable state
	vTemp.x = (float)m_DIJoyState.lRx - fCenter;
	vTemp.y = (float)m_DIJoyState.lRy - fCenter;

	//X dead zone
	if(vTemp.x < 3 && vTemp.x > 0)
		vTemp.x = 0;
	else if(vTemp.x > -3 && vTemp.x < 0)
		vTemp.x = 0;

	//Y dead zone
	if(vTemp.y < 3 && vTemp.y > 0)
		vTemp.y = 0;
	else if(vTemp.y > -3 && vTemp.y < 0)
		vTemp.y = 0;
	
	//D3DXVec2Normalize(vDir, &vTemp);
	//vDir = &vTemp;

	vTemp.x *= fCenterInverse;
	vTemp.y *= fCenterInverse;
	//vTemp /= fCenter;
	vDir->x = vTemp.x;
	vDir->y = vTemp.y;
*/

	D3DXVECTOR2 vTemp;
	vTemp.x = (float)m_DIJoyState.lRx;
	vTemp.y = (float)m_DIJoyState.lRy;

	//X dead zone
	if(vTemp.x < 512 && vTemp.x > 0)
		vTemp.x = 0;
	else if(vTemp.x > -512 && vTemp.x < 0)
		vTemp.x = 0;

	//Y dead zone
	if(vTemp.y < 512 && vTemp.y > 0)
		vTemp.y = 0;
	else if(vTemp.y > -512 && vTemp.y < 0)
		vTemp.y = 0;
	
	D3DXVec2Normalize(vDir, &vTemp);
	//vDir = &vTemp;
}

// 1 <- & -1 ->
void CDirectInput::CheckTriggers(float &vDir)
{
	vDir = (float)m_DIJoyState.lZ;

	if(vDir < 512 && vDir > 0)
		vDir = 0;
	else if(vDir > -512 && vDir < 0)
		vDir = 0;
	else if(vDir > 512)
		vDir = 1;
	else if(vDir < -512)
		vDir = -1;
}

//Clean up
void CDirectInput::DIShutdown()
{
	if(m_pDI)
	{
		if(m_pDIKeybrd)
		{
			m_pDIKeybrd->Unacquire();
			ReleaseCOM(m_pDIKeybrd);
		}

		if(m_pDIMouse)
		{
			m_pDIMouse->Unacquire();
			ReleaseCOM(m_pDIMouse);
		}

		if(m_pDIJoy)
		{
			m_pDIJoy->Unacquire();
			ReleaseCOM(m_pDIJoy); 
		}

		ReleaseCOM(m_pDI); 
	}
}


BOOL __stdcall EnumAxesCallback(const DIDEVICEOBJECTINSTANCE *pdidoi, VOID *pContext)
{
	HWND hDlg = (HWND)pContext;

    DIPROPRANGE diprg; 
    diprg.diph.dwSize			= sizeof(DIPROPRANGE); 
    diprg.diph.dwHeaderSize		= sizeof(DIPROPHEADER); 
    diprg.diph.dwHow			= DIPH_BYID; 
    diprg.diph.dwObj			= pdidoi->dwType; 
    diprg.lMin					= -1000; 
    diprg.lMax					= +1000; 
    
	// Set the range for the axis
	if( FAILED(CDirectInput::GetInstance()->m_pDIJoy->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
		return DIENUM_STOP;

    return DIENUM_CONTINUE;
}

BOOL __stdcall EnumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)
{
	HRESULT hr;

    hr = CDirectInput::GetInstance()->m_pDI->CreateDevice( pdidInstance->guidInstance, &CDirectInput::GetInstance()->m_pDIJoy, NULL );

    if( FAILED(hr) ) 
        return DIENUM_CONTINUE;

    return DIENUM_STOP;
}
