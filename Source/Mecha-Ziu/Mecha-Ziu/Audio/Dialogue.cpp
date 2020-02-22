#include "Dialogue.h"
#include "../Rendering/RenderEngine.h"
#include "../DirectInput.h"
#include "../Scripting/ScriptSystem.h"
#include "../Game.h"
#include "../User Interface/HUD/Hud.h"

const unsigned DLGBOXBUFF = 40;
const unsigned LINE_LENGTH = 45;

void CDialogue::LoadDialogueTexture()
{
	m_nDialogueBox = (signed)CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/DialogueBase.png");
}

bool CDialogue::Render(void)
{
	if(m_bRender)
	{
		if(m_Dialogues.size() == 0)
			return false;
		CRenderEngine* pRE = CRenderEngine::GetInstance();
		int nWidth = pRE->GetPresentParams()->BackBufferWidth;
		int nHeight = pRE->GetPresentParams()->BackBufferHeight;
		int nTexWidth = pRE->GetTextureWidth(m_nDialogueBox);
		int nTexHeight = pRE->GetTextureHeight(m_nDialogueBox);
		float fX = (((float)(nWidth * .5f) - nTexWidth * .5f)), fY = 0.0f;
		if(CCamera::GetInstance()->GetInScene())
			fY = (float)(nHeight - (DLGBOXBUFF + (nTexHeight* .5f)));
		else
			fY = (float)(DLGBOXBUFF);

		tTextureParams tParams;
		tParams.unTextureIndex = m_nDialogueBox;
		tParams.vScale = D3DXVECTOR2(1.2f, m_fInterScale);
		tParams.bCenter = false;
		tParams.vPosition = D3DXVECTOR2(fX,fY);
		tParams.color = D3DCOLOR_ARGB(127,255,255,255);
		pRE->RenderTexture(tParams);

		D3DCOLOR color = D3DCOLOR_ARGB(int(m_fInterText),255,255,255);
		float fLineBuff = 0.0f;
		
		for(unsigned i = 0; i < m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].uiNumStrings; ++i)
		{
			pRE->RenderText(const_cast<char*>(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].pszString[i].c_str()), 
				(int)(fX + 5.0f), (int)(fY + fLineBuff), color);
			fLineBuff += 20.0f;
		}
		return true;
	}
	return false;
}
int CDialogue::LoadDialogue(const char* szFileName)
{
	fstream fIn(szFileName, std::ios_base::in);
	
	// TODO: Dialogues need to be loaded in via scripts.
	// temp char buffer for catching number of strings
	char cBuff[3];
	ZeroMemory(cBuff, sizeof(char)*3);
	// temp int value of string read from file
	int nTemp = 0;
	// read in the number of strings for this dialogue
	fIn.read(cBuff, 2);
	nTemp = atoi(cBuff);
	
	// create the dialogue with the desired number of strings
	tDialogue temp(nTemp);

	string szSoundToLoad;
	for(unsigned i = 0; i < temp.uiNumDlgTexts; ++i)
	{
		szSoundToLoad.clear();
		char cIn = 0;
		// read in the file name for the associated sound
		fIn.read(&cIn,1);
		if(fIn.eof())
			break;

		while(cIn != ';')
		{
			if(cIn == 10)
			{
				fIn.read(&cIn,1);
				continue;
			}
			szSoundToLoad += cIn;
			fIn.read(&cIn,1);
			if(fIn.eof())
				break;
		}
		if(szSoundToLoad.size() != 0)
			temp.pTexts[i].nTextSound = CFMOD::GetInstance()->LoadSound(szSoundToLoad.c_str(), true, FMOD_DEFAULT);
		
		fIn.read(cBuff,2);
		nTemp = atoi(cBuff);
		temp.pTexts[i].uiNumStrings = nTemp;
		temp.pTexts[i].pszString = new string[temp.pTexts[i].uiNumStrings];
		cIn = 0;
		for(unsigned j = 0; j < temp.pTexts[i].uiNumStrings; ++j)
		{
			fIn.read(&cIn,1);
			while(cIn != ';')
			{
				temp.pTexts[i].pszString[j] += cIn;
				fIn.read(&cIn,1);
				if(fIn.eof())
					break;
			}
		}
		// read in read one byte for null
		fIn.read(&cIn,1);
		if(fIn.eof())
			break;
	}
	m_Dialogues.push_back(temp);
	return (signed)(m_Dialogues.size() - 1);
}

bool CDialogue::Update(float fElapsed)
{
	if(m_bRender)
	{	
		CDirectInput* pDI = CDirectInput::GetInstance();
		
		if(m_Dialogues.size())
		{
			if(m_nIndex < (signed)(m_Dialogues[m_nCurrDialogue].uiNumDlgTexts))
				if(pDI->CheckBufferedKey(DIK_RETURN))
					m_bSkip = true;
			if(m_bSkip && m_uiState != SCALEBOX_IN)
			{
				m_uiState = FADETEXT_IN;
				m_fInterText = 0.0f;
				if(!(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound == -2))
				{
					if(CFMOD::GetInstance()->IsSoundPlaying(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound, CFMOD::DIALOGUE))
						CFMOD::GetInstance()->StopSound(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound, CFMOD::DIALOGUE);
				}
				m_nIndex++;
				m_bSkip = false;
				if(m_nIndex >= (signed)m_Dialogues[m_nCurrDialogue].uiNumDlgTexts)
				{	
					m_uiState = SCALEBOX_OUT;
					m_nIndex = (signed)m_Dialogues[m_nCurrDialogue].uiNumDlgTexts - 1;
				}
			}

			switch(m_uiState)
			{
			case FADETEXT_IN:
				{
					m_fInterText += 255 * fElapsed;
					if(!(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound == -2))
					{
						// we divide by 1000 to get the track length in seconds.  980 give us a little extra time to display.
						if(CCamera::GetInstance()->GetInScene())
							SetDisplayTime((float)(CFMOD::GetInstance()->GetTrackLength(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound, CFMOD::DIALOGUE)/1030));
						else
							SetDisplayTime((float)(CFMOD::GetInstance()->GetTrackLength(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound, CFMOD::DIALOGUE)/980));

					}		
					if(m_fInterText >= 255.0f)
					{
						if(!(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound == -2))
						{
							if(!(CFMOD::GetInstance()->IsSoundPlaying(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound, CFMOD::DIALOGUE)))
								CFMOD::GetInstance()->Play(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound, CFMOD::DIALOGUE);
						}
						m_fInterText = 255.0f;
						m_uiState = ALWAYS;
					}			
				}
				break;
			case FADETEXT_OUT:
				{
					if(CGame::GetInstance()->GetTutorialMode() && m_uiState != ALWAYS)
						break;
					m_fInterText -= 255 * fElapsed;
					if(m_fInterText < 0.0f)
					{
						m_fInterText = 0.0f;
						m_uiState = FADETEXT_IN;
						m_nIndex++;
					}
					if(m_nIndex >= (signed)m_Dialogues[m_nCurrDialogue].uiNumDlgTexts)
					{
						m_uiState = SCALEBOX_OUT;
						m_bSkip = false;
						m_nIndex = 0;
					}
				}
				break;
			case SCALEBOX_IN:
				{
					m_fInterScale += 1.5f * fElapsed;

					if(m_fInterScale > 0.7f)
					{
						m_fInterScale = 0.7f;	
						m_uiState = FADETEXT_IN;
					}
				}
				break;
			case SCALEBOX_OUT:
				{
					m_fInterScale -= 1.5f * fElapsed;
					if(m_fInterScale < 0.0f)
					{
						m_nIndex = 0;
						m_fInterScale = 0.0f;
						m_uiState = SCALEBOX_IN;
						m_bRender = false;
						if(!(CCamera::GetInstance()->GetInScene()))
						{
							CGame::GetInstance()->GetPlayer()->SetLock(false);
							CHud::GetInstance()->SetWinCondition(0);
						}
					}				
				}
				break;
			case ALWAYS:
				{
					// add time to display the text
					m_fDisplayTextTimer += fElapsed;
					
					if(m_fDisplayTextTimer > m_fDisplayTime)
					{
						m_uiState = FADETEXT_OUT;
						m_fDisplayTextTimer = 0.0f;
					}	
				}
				break;
			};
		}
	}
	return true;
}


bool CDialogue::Reset(void)
{
	if(m_fDisplayTextTimer)
	{
		m_fDisplayTextTimer = 0.f;
		return true;
	}
	return false;
}

void CDialogue::Init()
{
	//	Register this call with the script system
	CScriptSystem::GetInstance()->RegisterClass<CDialogue>(this,"dialogue");
}
bool CDialogue::Shutdown(void)
{
	if(m_Dialogues.size())
	{
		if(!(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound == -2))
		{
			if(CFMOD::GetInstance()->IsSoundPlaying(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound, CFMOD::DIALOGUE))
				CFMOD::GetInstance()->StopSound(m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound, CFMOD::DIALOGUE);
		}
	}
	m_bRender = false;
	m_bSkip = false;
	m_nDialogueBox = -1;
	m_fDisplayTime = 7.0f;
	m_fDisplayTextTimer = 0.0f;
	m_fDisplayTime = 0.0f;
	m_fInterScale = 0.0f;
	m_fInterText = 0.0f;
	m_nCurrDialogue = 0;
	m_nIndex = 0;
	m_uiState = SCALEBOX_IN;
	vector<tDialogue>::iterator vIter = this->m_Dialogues.begin();
	while(vIter != this->m_Dialogues.end())
	{
		if((*vIter).pTexts != NULL)
		{
			for(unsigned i = 0; i < (*vIter).uiNumDlgTexts; ++i)
				delete [] (*vIter).pTexts[i].pszString;
			delete [](*vIter).pTexts;
		}
		++vIter;
	}
	this->m_Dialogues.clear();
	return true;
}

static int _DL_LoadDiag(lua_State *L)
{
	std::string szFileName = lua_tostring(L,-1);
	lua_pop(L,1);
	lua_pushnumber(L,CDialogue::GetInstance()->LoadDialogue(szFileName.c_str()));
	return 1;
}

static int _DL_SetDiag(lua_State *L)
{
	CDialogue::GetInstance()->SetCurrDialogue((signed)lua_tonumber(L,-1));
	lua_pop(L,1);
	return 0;
}

static int _DL_DisplayDialog(lua_State *L)
{
	bool bTrue = false;
	
	//	Check for a value
	if(!lua_isnil(L,-1))
	{
		if(lua_isnumber(L,-1) == 1)
		{
			if(lua_tonumber(L,-1) == 1)
				bTrue = true;
		}
		lua_pop(L,1);
	}

	//	Render the Dialog
	CDialogue::GetInstance()->DisplayDialogue(bTrue);
	return 0;
}

void CDialogue::LoadScript(const char *szScript)	{}

void CDialogue::RegisterScriptFunctions(SLua *pLua)
{
	pLua->RegisterFunction("LoadDialogue",_DL_LoadDiag);
	pLua->RegisterFunction("SetDialogue",_DL_SetDiag);
	pLua->RegisterFunction("DisplayDialogue",_DL_DisplayDialog);
}
