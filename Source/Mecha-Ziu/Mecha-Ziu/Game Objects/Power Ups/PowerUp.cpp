#include "PowerUp.h"
#include "../../Rendering/RenderEngine.h"
#include "../../Game.h"
#include "../../Scripting/iPack.h"

CPowerUp::CPowerUp(void)
{
	m_nType = POWERUP;
	m_nPowerUpType = -1;
	D3DXMatrixIdentity(&m_maRotate);
	m_fRotationSpeed = 1.0f;
	m_bCollision = false;

	m_pParticles.SetCapacity(1);
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles[0]->SetSystem(PPICK_UP);
	m_pParticles[0]->Play();
}

CPowerUp::~CPowerUp(void)
{
}

void CPowerUp::Init(int nPUType, D3DXMATRIX& maMatrix)
{
	m_maWorld = maMatrix;
	m_nPowerUpType = nPUType;
	m_bCollision = false;

	switch(nPUType)
	{
	case PU_HEALTH:
		{
			m_unModelIndex = CIPack::DefaultModelPack(CIPack::POWER_UP_1);
			break;
		}
	case PU_ARMOR:
		{
			m_unModelIndex = CIPack::DefaultModelPack(CIPack::POWER_UP_2);
			break;
		}
	case PU_MISSILES:
		{
			m_unModelIndex = CIPack::DefaultModelPack(CIPack::POWER_UP_3);
			break;
		}
	};
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);	
	
	SetSphereCenter();
}

bool CPowerUp::Update(float fDeltaTime)
{
	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Update(fDeltaTime);

	if(m_bCollision)
		return false;

	D3DXMatrixRotationY(&m_maRotate, fDeltaTime * m_fRotationSpeed);
	m_maWorld = m_maRotate * m_maWorld;

	CPlayer* pPlayer = CGame::GetInstance()->GetPlayer();
	
	D3DXVECTOR3 vToPlayer = pPlayer->GetPosition() - this->GetPosition();
	m_vVelocity = vToPlayer * (500 / D3DXVec3LengthSq(&vToPlayer));			// x / D3DXVec2LengthSq(&vToPlayer) -- x is player mass * power up mass

	CGameObject::Update(fDeltaTime);

	return true;
}

void CPowerUp::Render()
{
	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Render(m_maWorld);

	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld);
}

bool CPowerUp::CheckCollisions(CGameObject* pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CPowerUp::HandleCollision(CGameObject* pObject)
{
	if(pObject->GetType() == PLAYER)
		m_bCollision = true;
}