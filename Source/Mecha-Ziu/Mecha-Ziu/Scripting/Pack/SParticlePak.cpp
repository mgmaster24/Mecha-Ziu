#include "SParticlePak.h"
#include "../ScriptSystem.h"
#include "../../Game Objects/GameObject.h"
#include "../../Game Objects/Player/Player.h"
#include "../../Game Objects/Enemies/Enemy.h"
#include "../../Game Objects/Bullets/Bullet.h"
#include "../../Particles/ParticleSystem.h"
#include "../IPack.h"

SParticlePak::SParticlePak()
{
	m_nLastParticleLoaded = 0;
}

SParticlePak::~SParticlePak()
{
}

void SParticlePak::Init()
{
	//	Zero out the particle count
	m_nLastParticleLoaded = 0;

#ifdef RegisterClass
	//	Register with the script system
	CScriptSystem::GetInstance()->_Register_Class<SParticlePak>(this,"particle");
#else
	//	Register with the script system
	CScriptSystem::GetInstance()->RegisterClass<SParticlePak>(this, "particle");
#endif
}

/*************************************************
				Script Interface
*************************************************/
int _PP_LoadParticle(lua_State *L)
{
	//	Check that this is a string
	if(lua_isstring(L,-1) == 1)
	{
		//	Catch and pop the string
		std::string szParticleName = lua_tostring(L,-1);
		lua_pop(L,1);

		//	Check the size
		if(szParticleName.size() <= 0)
		{
			lua_pushnumber(L,-1);
			return 1;
		}

		//	Create a temporary particle system
		CParticleSystem particleSystem;

		signed nReturnVal = 0;
		//	Check that the file loads successfully
		if(particleSystem.Load(szParticleName.c_str(),"none") == false)
		{
			nReturnVal = -1;
		}
		else
		{
			//	Incriment the last particle used
			nReturnVal = (signed)CIPack::IParticle()->m_nLastParticleLoaded;
			CIPack::IParticle()->m_nLastParticleLoaded += 1;
		}

		//	Push back the value
		lua_pushnumber(L,nReturnVal);
	}

	return 1;
}

int _PP_ApplyParticle(lua_State *L)
{
	//	Check that object is valid object
	if(lua_islightuserdata(L,-2) == 1)
	{
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);
		signed int nParticleSys = (signed)lua_tonumber(L,-1);

		CParticleSystem *pParticle = new CParticleSystem();
		if(nParticleSys != -1)
			pParticle->SetSystem((EFFECTS)nParticleSys);
		pObject->AddParticleSystem(pParticle);
	}

	//	Pop the objects off the stack
	lua_pop(L,2);
	return 0;
}

int _PP_ToggleSystem(lua_State *L)
{
	//	Check that object is valid object
	if(lua_islightuserdata(L,-2) == 1)
	{
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);
		signed int nParticleSys = (signed)lua_tonumber(L,-1);

		CParticleSystem *pParticle = pObject->GetParticleSystem(nParticleSys);
		if(pParticle == 0)
			return 0;

		//	Toggle the particle system
		pParticle->ToggleSystem();
	}

	//	Pop the objects off the stack
	lua_pop(L,2);
	return 0;
}


/*************************************************
			Script System Functions
*************************************************/
void SParticlePak::LoadScript(const char *szScript)
{
}

void SParticlePak::RegisterScriptFunctions(SLua *pLua)
{
	//	Register the functions
	pLua->RegisterFunction("LoadParticle",_PP_LoadParticle);
	pLua->RegisterFunction("ApplyParticle",_PP_ApplyParticle);
	pLua->RegisterFunction("ToggleParticle",_PP_ToggleSystem);
}