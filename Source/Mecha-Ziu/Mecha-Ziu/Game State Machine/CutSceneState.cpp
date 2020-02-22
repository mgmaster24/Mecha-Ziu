#include "CutSceneState.h"
#include "TutorialState.h"
#include "Level1State.h"
#include "Level2State.h"
#include "Level3State.h"
CCutSceneState::CCutSceneState(CPlayState* pPlay) : CLevelState(pPlay)
{
	m_pPlay = pPlay;
	m_nLevelID = CUT_SCENE;
}
CCutSceneState::~CCutSceneState(void)
{

}
void CCutSceneState::Load(const char* szFileName)
{
	m_pScene = new CCutScene();
	m_pScene->LoadScene(szFileName);
}
void CCutSceneState::Init()
{
	m_pScene->InitScene();	
}
void CCutSceneState::Shutdown()
{
	delete m_pScene;
}
bool CCutSceneState::Update(float fDeltaTime, bool bJustSound)
{

	if(m_pScene->Update(fDeltaTime))
		return true;
	else
	{
		m_pPlay->FadeOut(fDeltaTime);
		if(m_pPlay->m_fAlphaLevel > 235)
		{
			CCamera::GetInstance()->GetLookOffset() = D3DXVECTOR3(0.0f, 0.0f, 10.0f);
			CCamera::GetInstance()->GetPosOffset() = D3DXVECTOR3(0.0f, 3.5f, -6.0f);
			m_pPlay->m_bInScene = false;
			m_pPlay->m_nQuitResult = 2;

			if(m_pPlay->m_nLevel == 0)
				m_pPlay->SwitchState(new CTutorialState(m_pPlay));
			else if(m_pPlay->m_nLevel == 1)
				m_pPlay->SwitchState(new CLevel1State(m_pPlay));	
			else if(m_pPlay->m_nLevel == 2)
				m_pPlay->SwitchState(new CLevel2State(m_pPlay));
			else if(m_pPlay->m_nLevel == 3)
				m_pPlay->SwitchState(new CLevel3State(m_pPlay));
			else
			{
				m_pPlay->m_GameState = m_pPlay->GS_WIN;
				return false;
			}
			CCamera::GetInstance()->SetInCutScene(false);
			CCamera::GetInstance()->SetChaseCamera(true);
		}
		return true;
	}
	return false;
}
void CCutSceneState::Render()
{
	m_pScene->Render();
}
