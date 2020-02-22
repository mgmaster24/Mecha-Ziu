#include "Talbot.h"
#include "../../Game.h"
#include "../Debris.h"
#include "../Factory.h"
#include "../Bullets/Bullet.h"
#include "../../Scripting/IPack.h"
#include "../../Audio/Dialogue.h"
#include "../../User Interface/HUD/Hud.h"

#define HACK_SCALE 0.075f

CTalbot::CTalbot(void) : m_Melee(true)
{
	SetType(BOSS_TALBOT);

	m_nHP = 80000;
	m_fSpeed = 30.0f;

	m_fElapsedDeath = 0.0f;

	m_vVelocity.x = 0.0f;
	m_vVelocity.y = 0.0f;
	m_vVelocity.z = 0.0f;
	m_pInterpolator = 0;

	m_bTargeted = false;

	m_pFMOD = CFMOD::GetInstance();

	m_fStateTime = 0;

	m_maGunMatrix = m_maWorld;
	m_maGunMatrix._42 += 2.0f; 

	m_pParticles.SetCapacity(2);
	for(int i = 0; i < NUM_TALBOT_EFFECTS; i++)
	{	m_pParticles.AddObject(new CParticleSystem());	}

	m_pParticles[TALBOT_HIT]->SetSystem(PHIT);
	m_pParticles[TALBOT_SHIELD_HIT]->SetSystem(PHIT_TALBOT);
	m_pParticles[TALBOT_EXPLOSION]->SetSystem(PEXPLOSION);
	m_pParticles[TALBOT_CHARGE_CANNON]->SetSystem(PCHARGE);

	//	Set Death Explosions
	for(int i = TALBOT_EXP1; i < TALBOT_EXP3 + 1; i++)
		m_pParticles[i]->SetSystem(PEXPLOSION_MICRO);

	m_Agent.SetEnemy(this);

	//	Set the melee data
	m_nMeleeCombo = CIPack::COMBO_1;

	m_nMeleeDPS = 1000;
	m_fMeleeROF = 1.0f;
	m_nRangeDPS = 800;
	m_nRangeFastDPS = 200;
	m_fRangeROF = 0.8f;
	m_fRangeFastROF = 0.08f;

	//	Load the Shield Texture
	m_nShieldTexture = CRenderEngine::GetInstance()->LoadTexture("./Assets/Models/TestModel/goldGlow.tga");

	//	Load the Sounds
	m_nSounds[TAL_HURT] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_EC_Collision.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_SHIELD] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_ShieldColide.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_SHIELD_CHARGE] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_ShieldUp.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_EXPLOSION] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Explosion.ogg", false, FMOD_DEFAULT);

	m_nSounds[TAL_SHOT] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_RegFire.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_CHARGESHOT] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_EC_Fife.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_CHARGESHOT_CHARGE] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Charging.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_CHARGE] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Thruster.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_HEAD_TURRETS] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_TurretFire.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_ROCKET] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Missle_Fire.ogg", false, FMOD_DEFAULT);

	m_nSounds[TAL_MELEE_1] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Sword1.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_MELEE_2] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Sword2.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_MELEE_3] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Sword3.ogg", false, FMOD_DEFAULT);
	m_nSounds[TAL_MELEE_4] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Sword4.ogg", false, FMOD_DEFAULT);

	// 1 = Watch your back, 2 = Going to leave a mark, 3 = Paladins Dismantle, 4 = poor accuracy, 5 = up close and personal
	// 6 = worthy adversey
	m_nDialogues[TAL_TAUNT1] = CDialogue::GetInstance()->LoadDialogue("Assets/Scripts/TalbotTaunt1.dlg");
	m_nDialogues[TAL_TAUNT2] = CDialogue::GetInstance()->LoadDialogue("Assets/Scripts/TalbotTaunt2.dlg");
	m_nDialogues[TAL_TAUNT3] = CDialogue::GetInstance()->LoadDialogue("Assets/Scripts/TalbotTaunt3.dlg");
	m_nDialogues[TAL_TAUNT4] = CDialogue::GetInstance()->LoadDialogue("Assets/Scripts/TalbotTaunt4.dlg");
	m_nDialogues[TAL_TAUNT5] = CDialogue::GetInstance()->LoadDialogue("Assets/Scripts/TalbotTaunt5.dlg");
	m_nDialogues[TAL_TAUNT6] = CDialogue::GetInstance()->LoadDialogue("Assets/Scripts/TalbotTaunt6.dlg");	

	for(int i = 0; i < NUM_TALBOT_DIALOGUE; ++i)
	{
		m_fTauntTimers[i] = 0.0f;
		m_bPlayDialogue[i] = false;
	}
	//Dialogues that don't require timers;
	m_bPlayDialogue[TAL_TAUNT3] = true; 
	m_bPlayDialogue[TAL_TAUNT6] = true; 


	//	Zero out all points
	for(int i = 0; i < TOTAL_TRANSFORMS; i++)
	{
		//m_pMatControlsTransform[i] = 0;
		m_vecControlOffsets[i].x = m_vecControlOffsets[i].y 
			= m_vecControlOffsets[i].z = 0;
	}

	m_fElapsedCharge = 0.0f;
	m_nQuadModelIndex = CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/quad.Model");
	m_nTargetedTextureID = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/Targeted.tga"); 

	m_nEnemyHealth = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/EnemyHealthBar.png");
	m_nTotalHealth = 0;
}

CTalbot::~CTalbot(void)
{
	if(m_pInterpolator != 0)
		delete m_pInterpolator;
}

void CTalbot::Init()
{
}

void CTalbot::OutOfBounds()
{
	//	Set the Talbot to being out of bounds
	m_Agent.OutBounds();
}

void CTalbot::SetControlTransforms(D3DXMATRIX* ptrans, int nIndex)
{
	/*if(nIndex >= TOTAL_TRANSFORMS || nIndex < 0)
	return;
	m_pMatControlsTransform[nIndex] = ptrans;*/
}
void CTalbot::SetControlSphere(D3DXVECTOR3* pvec, int nIndex)
{
	if(nIndex >= TOTAL_TRANSFORMS || nIndex < 0 || pvec == 0)
		return;
	m_vecControlOffsets[nIndex] = *pvec * HACK_SCALE;
}

void CTalbot::CheckTargeted()
{
	if(CGame::GetInstance()->GetPlayer()->GetTargeting() == false)
	{
		m_bTargeted = false;
	}
}

void CTalbot::RenderTargetSelect(int nTextureID, D3DXVECTOR3* vScale, float fYOffset)
{
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);


	D3DXMATRIX maWorld = m_maWorld;
	maWorld._42 += fYOffset;
	D3DXVECTOR3 vPos(maWorld._41, maWorld._42, maWorld._43);	
	D3DXVECTOR3 vVecZ = vPos - CCamera::GetInstance()->GetCameraPosition();

	D3DXVECTOR3 vVecX;
	D3DXVec3Cross(&vVecX, &D3DXVECTOR3(0.0f,1.0f,0.0f), &vVecZ);

	D3DXVECTOR3 vVecY;
	D3DXVec3Cross(&vVecY, &vVecZ, &vVecX);

	D3DXVec3Normalize(&vVecZ, &vVecZ);
	D3DXVec3Normalize(&vVecX, &vVecX);
	D3DXVec3Normalize(&vVecY, &vVecY);

	maWorld._31 = vVecZ.x;
	maWorld._32 = vVecZ.y;
	maWorld._33 = vVecZ.z;

	maWorld._21 = vVecY.x;
	maWorld._22 = vVecY.y;
	maWorld._23 = vVecY.z;

	maWorld._11 = vVecX.x;
	maWorld._12 = vVecX.y;
	maWorld._13 = vVecX.z;

	D3DXMATRIX maScale;
	D3DXMatrixScaling(&maScale, vScale->x, vScale->y, vScale->z);

	maWorld = maScale * maWorld;
	CRenderEngine::GetInstance()->RenderModel(m_nQuadModelIndex, NULL, &maWorld, nTextureID);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void CTalbot::UpdateTauntTimers(float fDeltaTime)
{
	for(int i = 0; i < NUM_TALBOT_DIALOGUE; ++i)
	{
		m_fTauntTimers[i] += fDeltaTime;
	}

	if(m_fTauntTimers[TAL_TAUNT1] > 10.0f)
		m_bPlayDialogue[TAL_TAUNT1] = true;
	else
		m_bPlayDialogue[TAL_TAUNT1] = false;

	if(m_fTauntTimers[TAL_TAUNT2] > 5.0f)
		m_bPlayDialogue[TAL_TAUNT2] = true;
	else
		m_bPlayDialogue[TAL_TAUNT2] = false;

	if(m_fTauntTimers[TAL_TAUNT4] > 40.0f)
		m_bPlayDialogue[TAL_TAUNT4] = true;
	else
		m_bPlayDialogue[TAL_TAUNT4] = false;

	if(m_fTauntTimers[TAL_TAUNT5] > 30.0f)
		m_bPlayDialogue[TAL_TAUNT5] = true;
	else
		m_bPlayDialogue[TAL_TAUNT5] = false;

}

bool CTalbot::IsTalbotDialoguePlaying()
{
	if(m_pFMOD->IsSoundPlaying(CDialogue::GetInstance()->GetCurrDialogueSound(), m_pFMOD->DIALOGUE))
		return true;

	return false;
}

void CTalbot::PlayTauntDialogues(float fDeltaTime)
{
	UpdateTauntTimers(fDeltaTime);
	//Play Dialogue taunts
	if(m_bPlayDialogue[TAL_TAUNT3])
	{
		CDialogue::GetInstance()->SetCurrDialogue(m_nDialogues[TAL_TAUNT3]);
		CDialogue::GetInstance()->DisplayDialogue(true);
		m_bPlayDialogue[TAL_TAUNT3] = false;
	}
	//Play near death dialogue
	if(m_nHP <= (m_nTotalHealth * 0.1f) && m_bPlayDialogue[TAL_TAUNT6] && !IsTalbotDialoguePlaying())
	{
		CDialogue::GetInstance()->SetCurrDialogue(m_nDialogues[TAL_TAUNT6]);
		CDialogue::GetInstance()->DisplayDialogue(true);
		m_bPlayDialogue[TAL_TAUNT6] = false;
	}
	//Play behind the player dialogue
	D3DXMATRIX maPlayerMatrix = CGame::GetInstance()->GetPlayer()->GetWorldMatrix();
	D3DXVECTOR3 vVecBetween = GetPosition() - D3DXVECTOR3(maPlayerMatrix._41, maPlayerMatrix._42, maPlayerMatrix._43);
	D3DXVECTOR3 vNormal = D3DXVECTOR3(maPlayerMatrix._31, maPlayerMatrix._32, maPlayerMatrix._33);
	float fDistance = D3DXVec3Dot(&vNormal, &vVecBetween);
	if(m_bPlayDialogue[TAL_TAUNT1] && fDistance < 0  && !IsTalbotDialoguePlaying())
	{
		CDialogue::GetInstance()->SetCurrDialogue(m_nDialogues[TAL_TAUNT1]);
		CDialogue::GetInstance()->DisplayDialogue(true);
		m_fTauntTimers[TAL_TAUNT1] = 0.0f;
	}
	//Play charging dialogue
	if(m_Agent.IsCharging())
	{
		//	Play the Charge Sound
		if(m_pFMOD->IsSoundPlaying(m_nSounds[TAL_CHARGE],m_pFMOD->SOUNDEFFECT) == false)
			m_pFMOD->Play(m_nSounds[TAL_CHARGE],m_pFMOD->SOUNDEFFECT);

		m_fElapsedCharge += fDeltaTime;
		if(m_fElapsedCharge > 3.0f)
		{
			if(m_bPlayDialogue[TAL_TAUNT2] && !IsTalbotDialoguePlaying())
			{
				CDialogue::GetInstance()->SetCurrDialogue(m_nDialogues[TAL_TAUNT2]);
				CDialogue::GetInstance()->DisplayDialogue(true);
				m_fTauntTimers[TAL_TAUNT2] = 0.0f;
				m_fElapsedCharge = 0.0f;
			}
		}
	}
	else
	{
		//	Check the charge particle
		if(m_pParticles[TALBOT_CHARGE_CANNON]->GetActive() == true)
			m_pParticles[TALBOT_CHARGE_CANNON]->ToggleSystem();
	}

	//Play poor accuracy dialogue
	if(!IsTalbotDialoguePlaying() && m_bPlayDialogue[TAL_TAUNT4] && !IsTalbotDialoguePlaying())
	{
		CDialogue::GetInstance()->SetCurrDialogue(m_nDialogues[TAL_TAUNT4]);
		CDialogue::GetInstance()->DisplayDialogue(true);
		m_fTauntTimers[TAL_TAUNT4] = 0.0f;
	}
}

void CTalbot::InitMeleeModelData(unsigned nModelID, float fAttackRadius)
{
	if(nModelID == -1)
		return;

	//	Set the model index
	m_unModelIndex = nModelID;
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);

	D3DXVECTOR3 vCenter = *m_Sphere.GetCenter();
	m_DamageSphere.SetCenter(vCenter * HACK_SCALE);
	m_DamageSphere.SetRadius(m_Sphere.GetRadius() * HACK_SCALE);
	m_DamageSphere.SetCenter(m_DamageSphere.GetCenter()->x + m_maWorld._41,
		m_DamageSphere.GetCenter()->y + m_maWorld._42, m_DamageSphere.GetCenter()->z + m_maWorld._43);

	//	Set the collision sphere
	m_Sphere = m_DamageSphere;
	m_Sphere.SetRadius(m_Sphere.GetRadius() * 2.5f);
	m_DamageSphere.SetRadius(m_DamageSphere.GetRadius() * 0.8f);

	//	Get the attack radius is invalid radius is passed
	if(fAttackRadius <= 0)
		fAttackRadius = m_Sphere.GetRadius();

	//	Init the melee Data
	m_Melee.InitMeleeData(m_Sphere, fAttackRadius);
}

void CTalbot::DecrimentStats(int nValue)
{
	if (m_nHP > 0)
	{
		m_Agent.MakeAggressive();
		DecrimentHP(nValue);
		m_pFMOD->Play(m_nSounds[TAL_HURT], m_pFMOD->SOUNDEFFECT);

		//	Play the Hurt Object
		m_pParticles[TALBOT_HIT]->Play();
		m_pParticles[TALBOT_HIT]->m_pEmitters[0]->MoveEmitter(D3DXVECTOR3(RAND_FLOAT(-0.2f, 0.2f), 
			RAND_FLOAT(0.0f, m_DamageSphere.GetRadius() + m_DamageSphere.GetRadius()), 0.0f));

		//	Check if we are still alive
		if (m_nHP <= 0)
		{
			//	Stop Playing All Sounds
			for(int i = 0; i < NUM_TALBOT_SOUNDS; i++)
			{
				if(m_pFMOD->IsSoundPlaying(m_nSounds[i],m_pFMOD->SOUNDEFFECT) == true)
					m_pFMOD->StopSound(m_nSounds[i],m_pFMOD->SOUNDEFFECT);
			}

			//	Set we just died
			DeathOfTalbot(0,true);

			//	Add Points to the hud
			CHud::GetInstance()->AddScore(25000);

			//	Unregister the agent
			m_Agent.UnregisterAgent();
			return;
		}
	}
}

bool CTalbot::Update(float fDeltaTime)
{
	//	Check if This is active
	if(m_fSpeed < 0)
	{
		if(m_bPlayDialogue[TAL_TAUNT3] == false)
			return true;

		m_fStateTime += fDeltaTime;
		//	Use state time as dialog timer
		if(m_fStateTime < 0.1f)
			return true;
		else
			m_fStateTime = 0;
	}

	//	Update the interpolator
	if(m_pInterpolator != 0)
		m_pInterpolator->Update(fDeltaTime);

	//	Check if talbot is dead of alive
	if(m_nHP <= 0)
	{
		//	Call the death update
		DeathOfTalbot(fDeltaTime);

		//Play near death dialogue
		if(m_bPlayDialogue[TAL_TAUNT6] && !IsTalbotDialoguePlaying())
		{
			CDialogue::GetInstance()->SetCurrDialogue(m_nDialogues[TAL_TAUNT6]);
			CDialogue::GetInstance()->DisplayDialogue(true);
			m_bPlayDialogue[TAL_TAUNT6] = false;
		}

		if(m_fElapsedDeath <= 0.0f)
		{
			//	Now we can die
			return false;
		}
	}
	else
	{
		PlayTauntDialogues(fDeltaTime);

		CGameObject::Update(fDeltaTime);
		CheckTargeted();
		m_Agent.Update(fDeltaTime);

		//	Check if we are in the melee state
		if(m_Agent.IsMelee() == false)
		{
			if(m_pInterpolator != 0)
			{
				m_pInterpolator->ToggleAnimation(CIPack::COMBO_1,false);
				m_pInterpolator->ToggleAnimation(CIPack::COMBO_2,false);
				m_pInterpolator->ToggleAnimation(CIPack::COMBO_3,false);
				m_pInterpolator->ToggleAnimation(CIPack::COMBO_4,false);
			}
		}
		else
		{
			if(m_bPlayDialogue[TAL_TAUNT5] && !IsTalbotDialoguePlaying())
			{
				CDialogue::GetInstance()->SetCurrDialogue(m_nDialogues[TAL_TAUNT5]);
				CDialogue::GetInstance()->DisplayDialogue(true);
				m_fTauntTimers[TAL_TAUNT5] = 0.0f;
			}
		}

		//	Play sounds by the state
		if(m_Agent.IsGunner() == true)
		{
			//	Check the atack timer for reset
			if(m_Agent.GetAttackTimer() == 0)
			{
				//	Fire the Head Turrets Sound
				if(m_pFMOD->IsSoundPlaying(m_nSounds[TAL_HEAD_TURRETS], m_pFMOD->SOUNDEFFECT) == false)
					m_pFMOD->Play(m_nSounds[TAL_HEAD_TURRETS], m_pFMOD->SOUNDEFFECT);
			}
		}

		if(m_Agent.IsRegularFire() == true)
		{
			//	Check the atack timer for reset
			if(m_Agent.GetAttackTimer() == 0)
			{
				//	Fire the Head Turrets Sound
				if(m_pFMOD->IsSoundPlaying(m_nSounds[TAL_SHOT], m_pFMOD->SOUNDEFFECT) == false)
					m_pFMOD->Play(m_nSounds[TAL_SHOT], m_pFMOD->SOUNDEFFECT);
			}
		}
		else if(m_Agent.IsChargeShot() == true)
		{
			//	Check the charge timer
			if(m_Agent.GetAgentTimer() > 3.3f)
			{
				if(m_Agent.GetAttackTimer() != 0)
				{
					//	Check if the Charging sound is playing
					if(m_pFMOD->IsSoundPlaying(m_nSounds[TAL_CHARGESHOT_CHARGE],m_pFMOD->SOUNDEFFECT) == false)
						m_pFMOD->Play(m_nSounds[TAL_CHARGESHOT_CHARGE],m_pFMOD->SOUNDEFFECT);

					//	Set the charging particles
					if(m_pParticles[TALBOT_CHARGE_CANNON]->GetActive() == false)
						m_pParticles[TALBOT_CHARGE_CANNON]->Play();
				}
				else
				{
					if(m_pFMOD->IsSoundPlaying(m_nSounds[TAL_CHARGESHOT],m_pFMOD->SOUNDEFFECT) == false)
						m_pFMOD->Play(m_nSounds[TAL_CHARGESHOT],m_pFMOD->SOUNDEFFECT);
				}
			}
		}
		else if(m_Agent.IsRocket() == true)
		{
			//	Check for a reset Timer
			if(m_Agent.GetAttackTimer() == 0)
			{
				if(m_pFMOD->IsSoundPlaying(m_nSounds[TAL_ROCKET],m_pFMOD->SOUNDEFFECT) == false)
					m_pFMOD->Play(m_nSounds[TAL_ROCKET],m_pFMOD->SOUNDEFFECT);
			}
		}

		//	Check the Sheild
		if(m_Agent.IsShield() == true)
		{
			//	Check if the shield is charging
			if(m_Agent.ShieldTime() < 0.5f)
			{
				//	Play the Shiled Charge Sound
				if(m_pFMOD->IsSoundPlaying(m_nSounds[TAL_SHIELD_CHARGE],m_pFMOD->SOUNDEFFECT) == false)
				{
					m_pFMOD->SetFrequency(m_pFMOD->SOUNDEFFECT, m_nSounds[TAL_SHIELD_CHARGE], 4.0f);
					m_pFMOD->Play(m_nSounds[TAL_SHIELD_CHARGE],m_pFMOD->SOUNDEFFECT);
				}
			}
		}

		//	Set the New Damage sphere possition
		m_DamageSphere.SetCenter(m_Sphere.GetCenter());

		//	Check that the gun arm is not null
		//if(/*m_pMatControlsTransform[GUN_ARM] != 0 && */m_pVecControlOffsets[GUN_ARM] != 0)
		{
			m_maGunMatrix._41 = m_maWorld._41 + (m_vecControlOffsets[GUN_ARM].x); //* HACK_SCALE);
			m_maGunMatrix._42 = m_maWorld._42 + (m_vecControlOffsets[GUN_ARM].y); //* HACK_SCALE);
			m_maGunMatrix._43 = m_maWorld._43 + (m_vecControlOffsets[GUN_ARM].z); //* HACK_SCALE);
		}

		//	Update the melee
		m_Melee.Update(fDeltaTime, m_Sphere.GetCenter(),m_maWorld);
	}

	//	Update the particle systems
	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Update(fDeltaTime);

	return true;
}

void CTalbot::Render()
{
	//	If no Total Health, Set the Total Health
	if(m_nTotalHealth == 0)
		m_nTotalHealth = m_nHP;

	/////////////////////////////////////////////////////////////////////
	// Hack for model scale
	D3DXMATRIX maScale;
	D3DXMatrixScaling(&m_matHackScale, HACK_SCALE,HACK_SCALE,HACK_SCALE);
	m_matHackScale *= m_maWorld;
	/////////////////////////////////////////////////////////////////////
	CRenderEngine *pRE = CRenderEngine::GetInstance();

	//	Render th Object
	D3DXMATRIX *pTransforms = 0;
	if(m_pInterpolator != 0)
		pTransforms = m_pInterpolator->GetTransforms();

	if(m_nMeleeCombo != -666)
	{
		//	Enable Normalize Normalse and Render the talbot
		pRE->RenderModel(m_unModelIndex, pTransforms, &m_matHackScale);

		//	Check if we have a shield scale
		if(true == m_Agent.IsShield())
		{	

			//	Find the Correct Current Alpha
			float fAlphaTime = m_Agent.ShieldTime()  / 3;

			//	Get the material
			D3DMATERIAL9 *material = 0;
			D3DMATERIAL9 origMat = CRenderEngine::GetInstance()->GetMaterial(m_unModelIndex,0);
			unsigned nSize = pRE->GetMeshCount(m_unModelIndex);
			for(unsigned int i = 0; i < nSize; i++)
			{
				material = &CRenderEngine::GetInstance()->GetMaterial(m_unModelIndex,i);
				material->Specular.a = material->Diffuse.a = fAlphaTime;
			}

			//	Render the Shield as Alpha
			pRE->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			pRE->RenderModel(m_unModelIndex, pTransforms, &m_matHackScale, m_nShieldTexture);
			pRE->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

			//	Reset the Materials
			for(unsigned int i = 0; i < nSize; i++)
			{
				D3DMATERIAL9 *material = &CRenderEngine::GetInstance()->GetMaterial(m_unModelIndex,i);
				(*material) = origMat;
			}
		}
		else if(true == m_Agent.IsCharging())
		{
			//	Get the material
			D3DMATERIAL9 *material = 0;
			D3DMATERIAL9 origMat = pRE->GetMaterial(m_unModelIndex,0);

			unsigned nSize = CRenderEngine::GetInstance()->GetMeshCount(m_unModelIndex);
			for(unsigned int i = 0; i < nSize; i++)
			{
				material = &pRE->GetMaterial(m_unModelIndex,i);
				material->Specular.a = material->Diffuse.a = 0.1f;
				material->Diffuse.r = material->Diffuse.g = material->Diffuse.b = 0.3f;
			}

			float offSet = 1.0f;
			pRE->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			//	Itterate Twice
			for(int i = 0; i < 4; i ++)
			{
				m_matHackScale._41 -= m_matHackScale._31 * offSet;
				m_matHackScale._42 -= m_matHackScale._32 * offSet;
				m_matHackScale._43 -= m_matHackScale._33 * offSet;

				//	Perform multiple renders of talbot
				pRE->RenderModel(m_unModelIndex, pTransforms, &m_matHackScale, m_nShieldTexture);
			}
			pRE->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

			//	Reset the Materials
			for(unsigned int i = 0; i < nSize; i++)
			{
				D3DMATERIAL9 *material = &pRE->GetMaterial(m_unModelIndex,i);
				(*material) = origMat;
			}
		}

		//	Disable Normalize Normals
		//CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS,false);

		//	Render If Selected
		if(m_bTargeted)
			RenderTargetSelect(m_nTargetedTextureID, &D3DXVECTOR3(4.0f, 4.0f, 4.0f), 1.0f);

		//if(m_bInCrosshair)
		RenderTargetSelect(m_nEnemyHealth, &D3DXVECTOR3(float( float(m_nHP)/float(m_nTotalHealth) ), 0.25f, 0.25f), 2.0f);

		//	Render the Hit Colors
		m_pParticles[TALBOT_HIT]->Render(m_maWorld);
		m_pParticles[TALBOT_SHIELD_HIT]->Render(m_maWorld);
	}

	//	Check for death render
	if(m_nHP <= 0)
	{
		m_pParticles[TALBOT_EXPLOSION]->Render(m_maWorld);
		for(int i = TALBOT_EXP1; i < TALBOT_EXP3 + 1; i++)
			m_pParticles[i]->Render(m_maWorld);
	}
}

bool CTalbot::CheckCollisions(CGameObject* pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CTalbot::HandleCollision(CGameObject* pObject)
{
	//	Check for collision with the damage sphere
	bool bDamageSphere = m_DamageSphere.CheckCollision(pObject->GetSphere());

	//	Reset the melee checks
	m_Melee.ResetSwitch();

	//	Check for a melee collision
	bool bMeleeCollision = m_Melee.CheckMeleeCollision(pObject->GetSphere(),m_maWorld);

	//	Handle Collision
	switch(pObject->GetType())
	{
	case PLAYER:
		{
			//	Check if the albot is charging
			if(m_Agent.IsCharging() == false)
			{
				//	Check for Melee Collision
				if(bMeleeCollision == true)
				{
					//	Check the agent melee and fire animation
					if(m_Agent.Melee() == true)
					{
						//	Check if we need to reset the combo
						if(m_nMeleeCombo > CIPack::COMBO_4)
							m_nMeleeCombo = CIPack::COMBO_1;

						//	Turn on the attack animation
						if(m_pInterpolator != 0)
						{
							//	Interpolate through the combonation
							m_pInterpolator->ToggleAnimation(m_nMeleeCombo,true);
						}

						//	Incriment the Melee
						switch(m_nMeleeCombo)
						{
						case CIPack::COMBO_1:	m_pFMOD->Play(m_nSounds[TAL_MELEE_1],m_pFMOD->SOUNDEFFECT);	break;
						case CIPack::COMBO_2:	m_pFMOD->Play(m_nSounds[TAL_MELEE_2],m_pFMOD->SOUNDEFFECT);	break;
						case CIPack::COMBO_3:	m_pFMOD->Play(m_nSounds[TAL_MELEE_3],m_pFMOD->SOUNDEFFECT);	break;
						case CIPack::COMBO_4:	m_pFMOD->Play(m_nSounds[TAL_MELEE_4],m_pFMOD->SOUNDEFFECT);	break;
						};

						//	Incriment the Combo
						m_nMeleeCombo += 1;
					}
					else
					{	bMeleeCollision = false;	}
				}
				else
				{	m_nMeleeCombo = CIPack::COMBO_1;	}

				//	Check for a melee collision
				if(bMeleeCollision == true)
				{
					//	Decriment the players stats
					((CPlayer*)pObject)->DecrimentStats(m_nMeleeDPS);

					//	Get the Velocity
					D3DXVECTOR3 vecVel = pObject->GetVelocity();

					//	Apply normalized Velocity to the player
					D3DXVec3Normalize(&vecVel,&vecVel);
					pObject->SetVelocity(vecVel);
				}

				//	Check collision with the damage sphere
				if(m_nMeleeCombo > CIPack::COMBO_4)
				{
					m_Agent.Repulsed();
					ApplyCollision(pObject, m_fSpeed + m_fSpeed);
					m_nMeleeCombo = CIPack::COMBO_1;
				}

				//	Check if there was no collision with the damage sphere
				if(bDamageSphere == true)
				{
					m_Agent.Repulsed();
					if(m_Agent.IsMelee() == true)
						ApplyCollision(pObject,m_fSpeed * 0.2f);
					else
						ApplyCollision(pObject);
				}
			}
			else if(bDamageSphere == true)
			{
				//	Check if the agent is repulsed
				if(m_Agent.IsRepulsed() == false)
				{
					m_Agent.Repulsed();
					ApplyCollision(pObject);
					((CPlayer*)pObject)->DecrimentStats(m_nMeleeDPS);
				}
			}

		}break;
	case BULLET:
		{
			//	Check if damage sphere was collided with
			if(bDamageSphere == false)
				return;

			//	Check the bulelt type
			CBullet *pBullet = (CBullet*)pObject;
			switch(pBullet->GetBulletType())
			{
			case ENEMY_BULLET:
			case ENEMY_MISSILE:
			case CC_CHARGED:
				break;
			case EC_CHARGED:
				{
					if(((CBulletCharged*)pObject)->GetFiredBy() == BOSS_TALBOT)
						return;

					//	Check if the agent is being repulsed
					if(m_Agent.IsRepulsed() == false)
					{
						//	Repulse the Agent
						m_Agent.Repulsed();

						//	Apply Collision
						ApplyCollision(pObject,m_fSpeed * 1.5f);

						//	Inflict damage to talbot
						unsigned int nDamage = pBullet->GetDamage();
						DecrimentStats(nDamage);
					}
				}
				break;
			case MISSILE:
				{
					CBulletManager::GetInstance()->RemoveBullet((CBullet*)pObject);

					//	Check if the agent is being repulsed
					if(m_Agent.IsRepulsed() == false)
					{
						//	Repulse the Agent
						m_Agent.Repulsed();

						//	Apply Collision
						ApplyCollision(pObject,m_fSpeed * 1.5f);
					}
				}
			default:
				{
					//	Check the shield type
					if(m_Agent.IsShield() == false)
					{
						//	Inflict damage to talbot
						unsigned int nDamage = pBullet->GetDamage();
						DecrimentStats(nDamage);

						//	Remove the bullet
						CBulletManager::GetInstance()->RemoveBullet((CBullet*)pObject);
					}
					else
					{
						if(pBullet->GetDamage() != 0)
						{
							float fRandOffset = RAND_FLOAT(-0.5f,0.5f);
							//	Deflect the bullet
							pBullet->SetAcceleration(pBullet->GetAcceleration() * (-1.0f + fRandOffset));
							pBullet->SetVelocity(pBullet->GetVelocity() * (-1.0f + fRandOffset));
							D3DXMATRIX *pMatrix = &pBullet->GetWorldMatrix();
							pMatrix->_31 = -pMatrix->_31;
							pMatrix->_32 = -pMatrix->_32;
							pMatrix->_33 = -pMatrix->_33;

							//	Play the Shield Hit Sound
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[TAL_SHIELD],m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[TAL_SHIELD],m_pFMOD->SOUNDEFFECT);

							//	Play the Hurt Particle
							m_pParticles[TALBOT_SHIELD_HIT]->Play();
							m_pParticles[TALBOT_SHIELD_HIT]->m_pEmitters[0]->MoveEmitter(D3DXVECTOR3(RAND_FLOAT(-0.2f, 0.2f), 
								RAND_FLOAT(0.0f, m_DamageSphere.GetRadius() + m_DamageSphere.GetRadius()), 0.0f));

							//	Set the Bullet damage as deflected
							pBullet->SetDamage(0);
						}
					}
				}
				break;
			}
		}break;
	};
}

void CTalbot::ApplyCollision(CGameObject *pObject, float fScalar)
{
	//	Force the Player and Talbot back
	D3DXVECTOR3 vecBetween(*pObject->GetSphere()->GetCenter() - *m_Sphere.GetCenter());
	D3DXVec3Normalize(&vecBetween,&vecBetween);

	float fSpeed = 0;

	//	Check for a scalar
	if(fScalar == 0)
	{
		//	Get the Length of the velocities
		D3DXVECTOR3 PlayerVel = pObject->GetVelocity();
		float fAccelTalbot = D3DXVec3Dot(&m_vVelocity,&m_vVelocity);
		float fAccelPlayer = D3DXVec3Dot(&PlayerVel,&PlayerVel);

		//	Normalize velocity
		if(fAccelTalbot < 0)	fAccelTalbot = -fAccelTalbot;
		if(fAccelPlayer < 0)	fAccelPlayer = -fAccelPlayer;

		//	Judge which is greater and get correct speed
		if(fAccelPlayer > fAccelTalbot)
		{
			fSpeed = pObject->GetSpeed();
			if(fSpeed > fAccelPlayer)
				fSpeed = fAccelPlayer;
		}
		else
		{
			fSpeed = m_fSpeed;
			if(fSpeed > fAccelTalbot)
				fSpeed = fAccelTalbot;
		}
	}
	else
	{
		if(fScalar > m_fSpeed)
			fSpeed = m_fSpeed;
		else
			fSpeed = fScalar;
	}

	//	Apply to velicity
	m_vVelocity = vecBetween * -fSpeed;
	pObject->SetVelocity(vecBetween * fSpeed);
}

void CTalbot::DeathOfTalbot(float fDeltaTime, bool bJustDied)
{
	//	Update the death time
	m_fElapsedDeath -= fDeltaTime;
	m_fStateTime += fDeltaTime;

	//	For Each To Meshes fire off a timer
	if(bJustDied == true)
	{
		//	Use the Combo Index as a death Counter
		m_nMeleeCombo = 0;
		m_fElapsedDeath = (0.4f * (CRenderEngine::GetInstance()->GetMeshCount(m_unModelIndex) >> 1)) + 2.0f;
	}
	else
	{
		//	Get the Time Greater then the previous
		if(m_fStateTime > 0.3f)
		{
			//	Reset State Time
			m_fStateTime = 0;

			CRenderEngine *pRE = CRenderEngine::GetInstance();
			//	Get the Number of meches divided by half
			unsigned nTotalMeshes = pRE->GetMeshCount(m_unModelIndex);

			//	 If no meches, kill the mech
			if(m_nMeleeCombo > nTotalMeshes)
			{
				//	Play the Final Explotion
				if(m_pParticles[TALBOT_EXPLOSION]->GetActive() == false)
					m_pParticles[TALBOT_EXPLOSION]->Play();

				//	Disable Talbot View and small particles
				if(m_nMeleeCombo != -666)
				{
					m_nMeleeCombo = -666;

					//	Disable
					for(int i = TALBOT_EXP1; i < TALBOT_EXP3 + 1; i++)
						m_pParticles[i]->ToggleSystem();

					//	Get the player world matrix
					D3DXMATRIX *pMatrix = &CGame::GetInstance()->GetPlayer()->GetWorldMatrix();

					/////////////////////////////////////////////////////////////////////
					// Hack for model scale
					D3DXMatrixScaling(&m_matHackScale, HACK_SCALE,HACK_SCALE,HACK_SCALE);
					m_matHackScale *= m_maWorld;
					/////////////////////////////////////////////////////////////////////

					//	Toss the Talbot Head
					D3DXVECTOR3 vecBetween(pMatrix->_41 - m_maWorld._41, 
						pMatrix->_42 - m_maWorld._42, pMatrix->_43 - m_maWorld._43);
					D3DXVec3Normalize(&vecBetween,&vecBetween);
					
					//	Get and Set the Debrie
					CDebris* pTemp = CFactory::GetInstance()->GetObjectType<CDebris>(NEW_DEBRIS);
					if(pTemp)
					{
						CSphere tempSphere = *(pRE->GetSphereByIndex(m_unModelIndex, 3));
						tempSphere.SetCenter(*tempSphere.GetCenter() + GetPosition());

						pTemp->SetSpeed(20);
						pTemp->SetSphere(tempSphere);
						vecBetween = vecBetween * 20;
						pTemp->SetVelocity(vecBetween);
						pTemp->Init(&m_matHackScale, &vecBetween, m_unModelIndex, 3);
						D3DXMATRIX scaleMatrix;
						D3DXMatrixScaling(&scaleMatrix,3.0f,3.0f,3.0f);

						pTemp->SetWorldMatrix(pTemp->GetWorldMatrix() * scaleMatrix);
						pTemp->SetPosition(D3DXVECTOR3(m_matHackScale._41,m_matHackScale._42,m_matHackScale._43));

						CGame::GetInstance()->GetObjectManager()->Add((CGameObject*)pTemp);
					}
				}

				//	Check the explosion sound
				//if(m_pFMOD->IsSoundPlaying(m_nSounds[TAL_EXPLOSION],m_pFMOD->SOUNDEFFECT) == false)
				m_pFMOD->Play(m_nSounds[TAL_EXPLOSION],m_pFMOD->SOUNDEFFECT);

				m_fStateTime = 0.0;
				return;
			}

			D3DXVECTOR3 vAcceleration;
			CSphere tempSphere;
			D3DXVECTOR3 vModelCenter, vMeshCenter;
			/////////////////////////////////////////////////////////////////////
			// Hack for model scale
			D3DXMatrixScaling(&m_matHackScale, HACK_SCALE,HACK_SCALE,HACK_SCALE);
			m_matHackScale *= m_maWorld;
			/////////////////////////////////////////////////////////////////////

			float fRandOffsetX = RAND_FLOAT(-5,5);
			int nIndex = TALBOT_EXP1 + (m_nMeleeCombo >> 1);
			for(int i = 0; i < nIndex; i++)
			{
				CDebris* pTemp = CFactory::GetInstance()->GetObjectType<CDebris>(NEW_DEBRIS);
				float fRandOffset = RAND_FLOAT(-1,1);

				if(pTemp)
				{
					vModelCenter = *(pRE->GetSphere(m_unModelIndex).GetCenter()); 
					tempSphere = *(pRE->GetSphereByIndex(m_unModelIndex, i));
					vMeshCenter = *(tempSphere.GetCenter()); 
					vAcceleration = (vMeshCenter - vModelCenter) * 20;
					vAcceleration *= fRandOffset;
					vAcceleration.x += fRandOffsetX;

					pTemp->SetSpeed(10.0f);
					pTemp->m_Material.Diffuse.r = 1.0f;
					pTemp->m_Material.Diffuse.g = 0.5f;
					pTemp->m_Material.Diffuse.b = 0.5f;

					pTemp->Init(&m_matHackScale, &vAcceleration, m_unModelIndex, i);
					pTemp->GetWorldMatrix()._11 *= 0.3f;
					pTemp->GetWorldMatrix()._22 *= 0.3f;
					pTemp->GetWorldMatrix()._33 *= 0.3f;
					tempSphere.SetCenter(*(tempSphere.GetCenter()) + GetPosition());
					pTemp->SetSphere(tempSphere);
					CGame::GetInstance()->GetObjectManager()->Add((CGameObject*)pTemp);
				}
			}

			//	Particle One
			unsigned nHalf = m_nMeleeCombo + (nTotalMeshes >> 1);
			if(nHalf > nTotalMeshes)
				nHalf = nTotalMeshes - 1;
			vModelCenter = *(pRE->GetSphere(m_unModelIndex).GetCenter()); 
			tempSphere = *(pRE->GetSphereByIndex(m_unModelIndex, nHalf));
			vMeshCenter = *(tempSphere.GetCenter()); 
			vMeshCenter.y += 2.0f;

			m_pParticles[TALBOT_EXP1]->Play();
			m_pParticles[TALBOT_EXP1]->m_pEmitters[0]->MoveEmitter(vMeshCenter * HACK_SCALE);

			//	Particle Two
			vModelCenter = *(pRE->GetSphere(m_unModelIndex).GetCenter()); 
			tempSphere = *(pRE->GetSphereByIndex(m_unModelIndex, m_nMeleeCombo));
			vMeshCenter = *(tempSphere.GetCenter()); 

			vMeshCenter.y += 2.0f;
			m_pParticles[TALBOT_EXP2]->Play();
			m_pParticles[TALBOT_EXP2]->m_pEmitters[0]->MoveEmitter(vMeshCenter * HACK_SCALE);

			//	Particle Three

			vMeshCenter.z = vMeshCenter.x = 0;
			vMeshCenter.y = 18.0f;
			m_pParticles[TALBOT_EXP3]->Play();
			m_pParticles[TALBOT_EXP3]->m_pEmitters[0]->MoveEmitter(vMeshCenter * HACK_SCALE);

			//	Check the explosion sound
			m_pFMOD->Play(m_nSounds[TAL_EXPLOSION],m_pFMOD->SOUNDEFFECT);

			//	Incriment the Counter
			m_nMeleeCombo += 2;
		}
	}
}