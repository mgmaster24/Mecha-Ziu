/*********************************************************************************
Name:			CombatText.h
Description:	Scrolling combat text that will be used to make the player 
				more aware of their score and multipliers.
*********************************************************************************/

#ifndef COMBATTEXT_H_
#define COMBATTEXT_H_
#include <d3dx9.h>
#define MAX_TEXT_LENGTH 16

class CCombatText
{
private:
	
	char			m_szText[MAX_TEXT_LENGTH];
	D3DXVECTOR2		m_vPosition;
	D3DXVECTOR2		m_vScorePosition;
	D3DXVECTOR2		m_vVecToScore;
	bool			m_bActive;
	float			m_fAlpha;
	float			m_fScale;


public:

	CCombatText(void);
	~CCombatText(void);

	CCombatText &operator=(const CCombatText&);

	/********************************************************************************
	Description:	Accessors and Mutators.
	********************************************************************************/
	char*			GetText()						{ return m_szText; }
	D3DXVECTOR2&	GetPosition()					{ return m_vPosition; }
	float			GetAlpha()						{ return m_fAlpha; }
	bool			GetActive()						{ return m_bActive; }
	void			SetText(char* szText)			{ strcpy_s(m_szText, strlen(szText)+1, szText); } 
	void			SetActive(bool bActive)			{ m_bActive = bActive; }
	
	/********************************************************************************
	Description:	Converts the position passed in to screen coordinates and sets 
					the text's position.
	********************************************************************************/
	void			SetPosition(D3DXMATRIX& maPlayer);

	/********************************************************************************
	Description:	Updates the combat text.
	********************************************************************************/
	bool Update(float fDeltaTime);

	/********************************************************************************
	Description:	Renders the combat text.
	********************************************************************************/
	void Render();
};
#endif 