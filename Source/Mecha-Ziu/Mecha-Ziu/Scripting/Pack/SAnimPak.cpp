#include "SAnimPak.h"
#include "../ScriptSystem.h"
#include "../IPack.h"
#include "../../Game Objects/Player/Player.h"
#include "../../Game Objects/Bosses/Talbot.h"
#include "../../Rendering/RenderEngine.h"

SAnimPak::SAnimPak()
{
}

SAnimPak::~SAnimPak()
{
}

void SAnimPak::Init()
{
	//	Register with the script system
	CScriptSystem::GetInstance()->RegisterClass<SAnimPak>(this, "animation");
}

unsigned SAnimPak::CreateAnimation(const char *szAnimation)
{
	//	The return animation index
	unsigned nAnimIndex = 0;
	tAnimData animData;

	//	Check for a valid address
	if(szAnimation == 0)
	{
		//	If the string is not null
		nAnimIndex = CRenderEngine::GetInstance()->LoadModel((char *)szAnimation);

		//	Check that the index is valid
		if(nAnimIndex != -1)
		{
			if(CRenderEngine::GetInstance()->GetAnimation(nAnimIndex) != 0)
				animData.nAnimationIndex = (unsigned)nAnimIndex;
		}
	}

	//	Get the animation array size
	nAnimIndex = m_AnimPakArray.size();

	//	Add the animation to the array
	m_AnimPakArray.AddObject(animData);

	//	Return the new animation index
	return nAnimIndex;
}

SAnimPak::tAnimData *SAnimPak::GetAnimData(unsigned nIndex)
{
	//	Check for a valid inde
	if(nIndex >= m_AnimPakArray.size())
		return 0;

	//	Return the index
	return &m_AnimPakArray[nIndex];
}

SAnimPak::tIndexSet *SAnimPak::GetAnimIndex(unsigned nIndex, unsigned nIndexID)
{
	//	Check for a valid index
	if(nIndex >= m_AnimPakArray.size())
		return 0;

	//	Check for a valid index data
	if(nIndexID >= m_AnimPakArray[nIndex].AnimIndexArray.size())
		return 0;

	//	Return the Index
	return &m_AnimPakArray[nIndex].AnimIndexArray[nIndexID];
}

void SAnimPak::AddAnimIndex(unsigned nIndex, unsigned nStartFrame, unsigned nEndFrame)
{
	//	Check for a valid inde
	if(nIndex >= m_AnimPakArray.size())
		return;

	//	Create a new Anim Index
	tIndexSet animIndex;
	animIndex.nStart = nStartFrame;
	animIndex.nEnd = nEndFrame;

	//	Add to the array
	m_AnimPakArray[nIndex].AnimIndexArray.AddObject(animIndex);
}

/********************************************************************************
								Script Functions
********************************************************************************/
static int _CC_CreateAnimData(lua_State *L)
{	
	unsigned nAnimationIndex = 0;
	//	Check if this is a string
	if(lua_isstring(L,-1) == 1)
	{
		nAnimationIndex = CIPack::IAnimation()->CreateAnimation((const char*)lua_tostring(L,-1));
		lua_pop(L,1);
	}
	else
	{
		nAnimationIndex = CIPack::IAnimation()->CreateAnimation(0);
	}

	//	Push back the animation inde
	lua_pushnumber(L,nAnimationIndex);

	//	Return the Index
	return 1;
}

static int _CC_SetAnimID(lua_State *L)
{
	//	Set the animation ID
	CIPack::IAnimation()->GetAnimData((unsigned)lua_tonumber(L,-2))->nAnimationIndex 
		= (unsigned)lua_tonumber(L,-1);
	lua_pop(L,2);
	return 0;
}

static int _CC_SetAnimSize(lua_State *L)
{
	unsigned nIndex = (unsigned)lua_tonumber(L,-2);
	//	Clear the Array Capacity
	CIPack::IAnimation()->GetAnimData(nIndex)
		->AnimIndexArray.SetCapacity((unsigned)lua_tonumber(L,-1));
	
	//	Set the animation index size
	CIPack::IAnimation()->GetAnimData(nIndex)->AnimIndexArray.SetSize(0);
	lua_pop(L,2);

	//	Return nothing
	return 0;
}

static int _CC_AddAnimIndex(lua_State *L)
{
	//	Get the Index
	unsigned nIndex = (unsigned)lua_tonumber(L,-3);
	CIPack::IAnimation()->AddAnimIndex(nIndex,
		(unsigned)lua_tonumber(L,-2),(unsigned)lua_tonumber(L,-1));
	lua_pop(L,3);
	return 0;
}

static int _CC_LoadAnimData(lua_State *L)
{
	const char *szFile = lua_tostring(L,-1);
	
	//	Check for a valid string
	if(szFile != 0)
	{
		//	Load the lua data
		CIPack::IAnimation()->LoadScript(szFile);
		lua_pop(L,1);
	}
	lua_pushnumber(L,CIPack::IAnimation()->GetAnimDataSize());
	return 1;
}

/********************************************************************************
								Script System Functions
********************************************************************************/
void SAnimPak::LoadScript(const char *szScript)
{
	//	Check that the script is valid
	if(szScript == 0)
		return;

	//	The current table index
	int nTableIndex = -1;

	//	Open a new lua state
	lua_State *L = lua_open();
	if(luaL_loadfile(L,szScript) != 0)
		return;

	//	Call the script
	lua_pcall(L,0,0,0);

	std::string szAnimName;
	//	Get the creation string
	lua_getglobal(L, "AnimationFile");
	if(!lua_isstring(L,-1))
		szAnimName = lua_tostring(L,-1);
	lua_pop(L,1);

	//	Create a new Animation File
	unsigned nIndex = this->CreateAnimation(szAnimName.c_str());
	tAnimData *animData = this->GetAnimData(nIndex);

	//	Check if this is null
	if(animData == 0)
	{	lua_close(L);	return;	}

	//	Work animation indicies
	SAnimPak::tIndexSet animIndex;

	//	Retrieve the animations from the script
	lua_getglobal(L,"Idle");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"Forward");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"Retreat");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"Rise");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"Fall");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"StrafeLeft");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"StrafeRight");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"CombatState");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"Combo1");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"Combo2");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"Combo3");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	lua_getglobal(L,"Combo4");
	__TableLoad(animIndex, L);
	animData->AnimIndexArray.AddObject(animIndex);

	//	Close the Script
	lua_close(L);
}

static int __SS_AddAnimIndex(lua_State *L)
{
	//	Check that this is light user data
	if(lua_islightuserdata(L,-5) == 1)
	{
		//	Get the animation data
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-5);

		//	Get the index
		unsigned int nStart = (unsigned)lua_tonumber(L,-4);
		unsigned int nEnd = (unsigned)lua_tonumber(L,-3);
		unsigned int nIndex = (unsigned)lua_tonumber(L,-2);
		bool bLoop = true;
		if(lua_tonumber(L,-1) == 0)
			bLoop = false;

		//	Check the object type
		switch(pObject->GetType())
		{
		case PLAYER:
			{
				CPlayer *pPlayer = (CPlayer*)pObject;
				if(pPlayer->GetInterpolator() != 0)
					pPlayer->GetInterpolator()->AddAnimation(nStart,nEnd,nIndex,bLoop);
			}
			break;
		case ENEMY:
			{
				CEnemy *pEnemy = (CEnemy*)pObject;
				if(pEnemy->GetInterpolator() != 0)
					pEnemy->GetInterpolator()->AddAnimation(nStart,nEnd,nIndex,bLoop);
			}
			break;
		case BOSS_TALBOT:
			{
				CTalbot *pTalbot = (CTalbot*)pObject;
				if(pTalbot->GetInterpolator() != 0)
					pTalbot->GetInterpolator()->AddAnimation(nStart,nEnd,nIndex,bLoop);
			}
			break;
		};
	}
	lua_pop(L,5);

	return 0;
};

static int _SS_AssignAnimPack(lua_State *L)
{
	//	Check that this is light user data
	//if(lua_islightuserdata(L,-2) == 1)
	//{
	//	//	Get the animation data
	//	CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);

	//	//	Get the index
	//	unsigned int nIndex = (unsigned)lua_tonumber(L,-1);
	//	
	//	//	Check that this is valid
	//	if(CIPack::IAnimation()->GetAnimDataSize() <= nIndex)
	//		return 0;

	//	//	Check the object type
	//	switch(pObject->GetType())
	//	{
	//	case PLAYER:
	//		{
	//			CPlayer *pPlayer = (CPlayer*)pObject;
	//			if(pPlayer->GetInterpolator() != 0)
	//			{
	//				SAnimPak::tAnimData *pPac = CIPack::IAnimation()->GetAnimData(nIndex);
	//				unsigned nSize = pPac->AnimIndexArray.size();

	//				//	Itterate through each object in the pack and add it to the player interpolator
	//				for(unsigned i = 0; i < nSize; i++)
	//				{
	//					pPlayer->GetInterpolator()->AddAnimation(pPac->AnimIndexArray[i]);
	//				}
	//			}
	//		}
	//		break;
	//	case ENEMY:
	//		{
	//			CEnemy *pEnemy = (CEnemy*)pObject;
	//		}
	//		break;
	//	};
	//	lua_pop(L,3);
	//}

	return 0;
};

int _UpdateInterpolator(lua_State *L)
{
	if(lua_islightuserdata(L,-2) == 1)
	{
		//	Get the Object
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);

		//	Check if an interpolator already exists
		switch(pObject->GetType())
		{
		case ENEMY:
			{
				//	Check for a valid interpolator
				CInterpolator *pInterp = ((CEnemy*)pObject)->GetInterpolator();
				if(pInterp != 0)
					pInterp->Update((float)lua_tonumber(L,-1));
			}
			break;
		case BOSS_TALBOT:
			{
				//	Check for a valid interpolator
				CInterpolator *pInterp = ((CTalbot*)pObject)->GetInterpolator();
				if(pInterp != 0)
					pInterp->Update((float)lua_tonumber(L,-1));
			}
			break;
		case PLAYER:
			{
				//	Check for a valid interpolator
				CInterpolator *pInterp = ((CPlayer*)pObject)->GetInterpolator();
				if(pInterp != 0)
					pInterp->Update((float)lua_tonumber(L,-1));
			}
			break;
		}
	}
	lua_pop(L,2);
	return 0;
}

int _SetInterpTransforms(lua_State *L)
{
	if(lua_islightuserdata(L,-3) == 1)
	{
		//	Get the Object
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-3);

		//	Check if an interpolator already exists
		switch(pObject->GetType())
		{
		case ENEMY:
			{
			}
			break;
		case BOSS_TALBOT:
			{
				//	Set the apropriot transform
				char *szTransformname = (char*)lua_tostring(L,-2);
				CTalbot *pTalbot = (CTalbot*)pObject;

				//	Check for a valid interpolator
				CInterpolator *pInterp = pTalbot->GetInterpolator();
				if(pInterp != 0)
				{
					D3DXMATRIX *pTransform = 0;
					//	Get and set the transform
					pTransform = pInterp->GetTransformByName(szTransformname);
					if(pTransform == 0)
					{
						if(pObject->GetModelIndex() != -1)
							pTransform = CRenderEngine::GetInstance()->GetTransformByName(
							pObject->GetModelIndex(), szTransformname);
					}
					pTalbot->SetControlTransforms(pTransform,(int)lua_tonumber(L,-1));
				}

			}
			break;
		case PLAYER:
			{
			}
			break;
		}
	}
	lua_pop(L,3);
	return 0;
}

int _SetSphereOffsets(lua_State *L)
{
	//	Check light user data
	if(lua_islightuserdata(L,-3) == 1)
	{
		//	Get the Object
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-3);

		//	Check if an interpolator already exists
		switch(pObject->GetType())
		{
		case ENEMY:
			{
			}
			break;
		case BOSS_TALBOT:
			{
				//	Set the apropriot transform
				char *szTransformname = (char*)lua_tostring(L,-2);
				CTalbot *pTalbot = (CTalbot*)pObject;

				//	Get and set the transform
				if(pObject->GetModelIndex() != -1)
				{
					const D3DXVECTOR3 *pVec = CRenderEngine::GetInstance()->GetSphereByName(
						pObject->GetModelIndex(), szTransformname)->GetCenter();
					pTalbot->SetControlSphere((D3DXVECTOR3*)pVec,(int)lua_tonumber(L,-1));
				}
			}
			break;
		case PLAYER:
			{
			}
			break;
		}
	}
	return 0;
}

int _InitAnimation(lua_State *L)
{
	//	Check for a valid object
	if(lua_islightuserdata(L,-1) == 1)
	{
		//	Get the Object
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-1);
		if(pObject->GetModelIndex() != -1)
		{
			//	Check for a valid animation
			Animation *pAnim = CRenderEngine::GetInstance()->GetAnimation(pObject->GetModelIndex());
			
			//	Check for a valid animation
			if(pAnim != 0)
			{
				//	Get the Type and create the interpolate
				CInterpolator *pInterpolator = new CInterpolator(pAnim);

				//	Check if an interpolator already exists
				switch(pObject->GetType())
				{
				case BOSS_TALBOT:
					{
						CTalbot *pTalbot = (CTalbot*)pObject;
						if(pTalbot->GetInterpolator() != 0)
							delete pTalbot->GetInterpolator();
						pTalbot->SetInterpolator(pInterpolator);
					}
					break;
				case PLAYER:
					{
					}
					break;
				case ENEMY:
					{
					}
					break;
				};
			}
		}
	}
	lua_pop(L,1);
	return 0;
}

void SAnimPak::RegisterScriptFunctions(SLua *pLua)
{
	//	Register the functions
	pLua->RegisterFunction("CreateAnimData",_CC_CreateAnimData);
	pLua->RegisterFunction("SetAnimID",_CC_SetAnimID);
	pLua->RegisterFunction("SetAnimSize",_CC_SetAnimSize);
	pLua->RegisterFunction("AddAnimIndex",_CC_AddAnimIndex);
	pLua->RegisterFunction("AddObjectAnimIndex",__SS_AddAnimIndex);
	pLua->RegisterFunction("AssignAnimPack",_SS_AssignAnimPack);
	pLua->RegisterFunction("LoadAnimData",_CC_LoadAnimData);
	pLua->RegisterFunction("InitInterpolator",_InitAnimation);
	pLua->RegisterFunction("UpdateInterpolator",_UpdateInterpolator);
	pLua->RegisterFunction("SetControlTransforms",_SetInterpTransforms);
	pLua->RegisterFunction("SetSphereOffsets",_SetSphereOffsets);
}

void SAnimPak::__TableLoad(tIndexSet &animIndex, lua_State *L)
{
	//	Check if is stack is nill
	if(!lua_isnil(L,-1))
	{
		animIndex.nStart = animIndex.nEnd = 0;
		return;
	}

	//	The current table index
	int nTableIndex = -1;

	//	Get the start frame
	lua_pushnumber(L, -1 * nTableIndex);
	lua_gettable(L,nTableIndex - 1);
	nTableIndex -= 1;
	//	Check that this is not nil
	if(!lua_isnil(L,-1))
		animIndex.nStart = 0;
	else
		animIndex.nStart = (unsigned)lua_tonumber(L, -1);

	//	Get the end frame

	lua_pushnumber(L, -1 * nTableIndex);
	lua_gettable(L,nTableIndex - 1);
	nTableIndex -= 1;
	//	Check that this is not nil
	if(!lua_isnil(L,-1))
		animIndex.nStart = 0;
	else
		animIndex.nEnd = (unsigned)lua_tonumber(L, -1);

	//	Pop the index
	lua_pop(L,-nTableIndex);
}