#include "CombatText.h"
#include "../../Rendering/RenderEngine.h"
#include "../../Game.h"

CCombatText::CCombatText(void)
{
	ZeroMemory(&m_szText, sizeof(char)*MAX_TEXT_LENGTH); 
	m_vPosition.x = m_vPosition.y = 0.0f;
	m_fAlpha = 0.0f;
	m_fScale = 0.55f;

	m_vScorePosition.x = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth * 0.05f + 60.0f;
	m_vScorePosition.y = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight - 
						 ((float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight * .95f) - 20.0f;

	m_vVecToScore.x = 0.0f;
	m_vVecToScore.y = 0.0f;

	m_bActive = false;
}

CCombatText::~CCombatText(void)
{
}

void CCombatText::SetPosition(D3DXMATRIX& maPlayer)
{
	D3DXVECTOR3 result(0.0, 0.0, 0.0);
	D3DVIEWPORT9 vp;
	HR(CRenderEngine::GetInstance()->GetDevice()->GetViewport(&vp));

	D3DXVECTOR3 vSource(maPlayer._41, maPlayer._42+2.5f, maPlayer._43);
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	D3DXVec3Project(&result, &vSource, &vp, &CCamera::GetInstance()->GetProjMatrix(), 
					&CCamera::GetInstance()->GetViewMatrix(), &world);

	m_vPosition.x = result.x;
	m_vPosition.y = result.y;
}

bool CCombatText::Update(float fDeltaTime)
{
	if(m_bActive)
	{
		m_fAlpha += 80.0f * fDeltaTime;
		if(m_fAlpha >= 250.0f)
			m_fAlpha = 255.0f;

		m_fScale -= .08f * fDeltaTime;
		if(m_fScale <= 0.25f)
			m_fScale = 0.25f;

		if(m_fAlpha > 100.0f)
		{
			m_vVecToScore = m_vScorePosition - m_vPosition;
			m_vPosition.y += m_vVecToScore.y * fDeltaTime;
			m_vPosition.x += m_vVecToScore.x * fDeltaTime;

			if(m_vVecToScore.x >= -20.0f)
				strcpy_s(m_szText, strlen("+")+1, "+");

			if(m_vVecToScore.x >= -8.0f && m_vVecToScore.y >= -10.0f)
			{
				return false;
			}
		}
	}

	return true;
}

void CCombatText::Render()
{
	if(m_bActive)
		CRenderEngine::GetInstance()->RenderText(m_szText, (int)m_vPosition.x, (int)m_vPosition.y, 
											 D3DCOLOR_ARGB((unsigned int)m_fAlpha,0,255,255), m_fScale);
}

CCombatText& CCombatText::operator=(const CCombatText& combatText)
{	
	if (&combatText == this)
		return *this;

	this->m_fAlpha = combatText.m_fAlpha;
	this->m_fScale = combatText.m_fScale;
	strcpy_s(this->m_szText, strlen(combatText.m_szText)+1, combatText.m_szText);
	this->m_vPosition = combatText.m_vPosition;

	return *this;
}