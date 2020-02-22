/*********************************************************************
*	File Name: Dialogue.h
*
*	Desciption: Handles all in game Dialogues
*********************************************************************/
#ifndef _C_DIALOGUE_H_
#define _C_DIALOGUE_H_

// windows stuff
#include <windows.h>

// file input stream
#include <fstream>
using std::fstream;

// storage mediums
#include <vector>
using std::vector;

// string
#include <string>
using std::string;

// include FMOD to get its functionality
#include "FMOD_wrap.h"

const unsigned MAX_STRING = 256;

enum { ALWAYS = 0, FADETEXT_OUT, FADETEXT_IN, SCALEBOX_OUT, SCALEBOX_IN, PAUSE };

class SLua;
typedef struct _tDlgText
{
	unsigned uiNumStrings;
	int		 nTextSound;
	string*	 pszString;
	_tDlgText(void) : nTextSound(-2) {}
}tDlgText;
// stuct that defines one dialogue to be displayed.
typedef struct _tDialogue
{
	unsigned		uiNumDlgTexts;
	// sounds associated with this dialogue
	// strings to be used for this dialogue
	tDlgText*			pTexts;			

	_tDialogue(void) : pTexts(NULL), uiNumDlgTexts(0){}
	_tDialogue(int nNumTexts) : uiNumDlgTexts((unsigned)nNumTexts)
	{
		pTexts = new tDlgText [nNumTexts];
	}
}tDialogue;

class CDialogue
{
	bool				m_bRender;
	bool				m_bSkip;
	unsigned			m_uiState;
	int					m_nIndex;
	// id used for the dialogue box texture
	int					m_nDialogueBox; 
	int					m_nCurrDialogue;
	// current elapsed time
	float				m_fDisplayTextTimer;
	float				m_fDisplayTime;
	float				m_fInterScale;
	float				m_fInterText;
	// vector of all possible dialouges with the game
	vector<tDialogue>	m_Dialogues;
	
	CDialogue(void) : m_nDialogueBox(-1), m_fDisplayTextTimer(0.f), m_fInterText(0), m_fDisplayTime(7.0f),
			m_fInterScale(0.f), m_nIndex(0), m_bRender(false), m_bSkip(false), m_uiState(SCALEBOX_IN){}
	CDialogue(const CDialogue& ref){}
	CDialogue& operator =(const CDialogue& ref){} 
	~CDialogue(void){}

public:

	// Accessors
	vector<tDialogue>& GetDialogues(void) { return m_Dialogues; }
	int GetCurrDialogue(void)			  {	return m_nCurrDialogue;	} 
	int GetCurrDialogueSound(void)		  { return m_Dialogues[m_nCurrDialogue].pTexts[m_nIndex].nTextSound; }
	float GetDisplayTime(void)			  { return m_fDisplayTime; }
	bool IsDialogueDisplaying()			  { return m_bRender; }
	// Mutators
	void SetCurrDialogue(int nDialogueID) { 
											m_nCurrDialogue = nDialogueID; 
											m_fDisplayTextTimer = 0.f;
											m_bRender = false;
										  }
	void SetDisplayTime(float fDisplayTime) { m_fDisplayTime = fDisplayTime; }
	void SetState(unsigned int uiState) { m_uiState = uiState; }
	void Skip()								{ m_bSkip = true; }
	/*********************************************************************
	*	Name: GetInstance
	*
	*	Desciption: Returns the single static instance of this class
	*********************************************************************/
	static CDialogue* GetInstance(void)	  { static CDialogue instance; return &instance; } 
	/*********************************************************************
	*	Name: DisplayDialogue
	*
	*	Desciption: Decides if to display the dialogue
	*********************************************************************/
	void DisplayDialogue(bool bRender) { m_bRender = bRender; }
	/*********************************************************************
	*	Name: Render
	*
	*	Desciption: Renders the dialogue box and text
	*********************************************************************/
	bool Render(void);
	/*********************************************************************
	*	Name: LoadDialogues
	*
	*	Desciption: Loads a dialogues and stores it 
	*				in the list of dialogues
	**********************************************************************/
	int LoadDialogue(const char* szFileName);

	void LoadDialogueTexture();

	/*********************************************************************
	*	Name: AddDialogue
	*
	*	Desciption: Adds a dialogues and stores it 
	*				in the list of dialogues via Lua scripts
	**********************************************************************/
	int AddDialogue(tDialogue& dialogueToAdd) { m_Dialogues.push_back(dialogueToAdd); return (int)(m_Dialogues.size() - 1); }
	/*********************************************************************
	*	Name: Update
	*
	*	Desciption: Updates the elapsed time for accurate 
	*				display of dialogue and associated sound
	*********************************************************************/
	bool Update(float fElapsed);
	/*********************************************************************
	*	Name: Reset
	*
	*	Desciption: Resets the time on restart
	*********************************************************************/
	bool Reset(void);
	/*********************************************************************
	*	Name: Init
	*
	*	Desciption: Init local variables and register with the script system
	*********************************************************************/
	void Init();
	/*********************************************************************
	*	Name: Shutdown
	*
	*	Desciption: Cleans up all memory allocated by this system
	*********************************************************************/
	bool Shutdown(void);

	/*********************************************************************
	*	Name: LoadScript
	*
	*	Desciption: Load a dialog script using lua
	*********************************************************************/
	void LoadScript(const char *szScript);

	/*********************************************************************
	*	Name: RegisterScriptFunctions
	*
	*	Desciption: Register functions for use with the lua system
	*********************************************************************/
	void RegisterScriptFunctions(SLua *pLua);

};
#endif