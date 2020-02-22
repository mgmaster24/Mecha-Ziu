#include "IPack.h"

CIPack::_tEmbededClass CIPack::m_IScript;

CIPack::CIPack()
{
}

CIPack::~CIPack()
{
}

void CIPack::Init(CRenderEngine *pRender, CObjectManager *pObjectMan)
{
	//	Init the interfaces
	m_IScript.m_IScriptRender.Init(pRender);
	m_IScript.m_IScriptObject.Init(pObjectMan);
	m_IScript.m_IScriptAnim.Init();
	m_IScript.m_IScriptParticle.Init();
}