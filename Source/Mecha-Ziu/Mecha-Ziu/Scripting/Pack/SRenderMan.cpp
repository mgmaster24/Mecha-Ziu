#include "SRenderMan.h"
#include "../../Rendering/RenderEngine.h"
#include "../ScriptSystem.h"
#include "../IPack.h"

CSRenderMan::CSRenderMan()	:	m_pRender(0)	{}

void CSRenderMan::Init()
{
	//	Register this class
	CScriptSystem::GetInstance()->RegisterClass<CSRenderMan>(this,"renderSys");
}

void CSRenderMan::Init(CRenderEngine *pRenderEngine)
{
	//	Set the Pointer
	m_pRender = pRenderEngine;

	Init();
}

int CSRenderMan::CreateModelPack()
{
	//	Create a new model pack and return its id
	m_PackArray.AddObject(tModelPack());
	return (signed)(m_PackArray.size() - 1);
}

int CSRenderMan::GetModelID(int nModelPack, int nModel)
{
	//	Check that the pack id is valid
	if(nModelPack < 0 || nModelPack >= (signed)m_PackArray.size()
		|| nModel < 0 || nModel >= 12)
		return - 1;

	//	Return the modelID
	return m_PackArray[nModel].nModelID[nModel];
}

void CSRenderMan::SetModelID(int nModelPack, int nModel, int nSetModelID)
{
	//	Check that the pack id is valid
	if(nModelPack < 0 || nModelPack >= (signed)m_PackArray.size()
		|| nModel < 0 || nModel >= 12)
		return;

	//	Set the modelID
	m_PackArray[nModel].nModelID[nModel] = nSetModelID;
}

int _LoadModel(lua_State *L)
{
	//	Get the Lua String
	const char *szAddress = lua_tostring(L,-1);
	
	//	Load the Model
	unsigned int nModelID = CIPack::IRender()->m_pRender->LoadModel((char*)szAddress);

	//	Pop the String
	lua_pop(L,1);

	//	Push the Model ID
	lua_pushnumber(L,nModelID);

	//	Return the Index;
	return 1;
}

int _LoadTexture(lua_State *L)
{
	//	Get the String
	const char *szAddress = lua_tostring(L,-1);

	//	Load the Texture
	unsigned int nTextureID = CIPack::IRender()->m_pRender->LoadTexture((char*)szAddress);

	//	Pop the String
	lua_pop(L,1);

	//	Push the string onto the stack
	lua_pushnumber(L,nTextureID);

	//	Return the Index
	return 1;
}

int _RenderText(lua_State *L)
{
	//	Get the String
	const char *szAddress = lua_tostring(L,-1);
	
	//	Get the Possition
	int posX = (int)lua_tonumber(L,-2);
	int posY = (int)lua_tonumber(L,-3);

	//	Get the Color
	unsigned int nColor = (unsigned)lua_tonumber(L,-4);

	//	Render the Text
	CIPack::IRender()->m_pRender->RenderText((char*)szAddress,posX,posY,(DWORD)nColor);

	//	Pop the String
	lua_pop(L,4);

	//	Return Nothing
	return 0;
}

////////////////////////////////////////////////////////////////////////////
//
//							Light Help Functions
//
////////////////////////////////////////////////////////////////////////////
double GetLuaTableNum(lua_State *L, int nTableIndex)
{
	lua_pushnumber(L, -1 * nTableIndex);
	lua_gettable(L,nTableIndex - 1);
	return lua_tonumber(L, -1);
}

void SetColorValue(D3DCOLORVALUE &colorValue, unsigned int nColor)
{
	colorValue.a = (float)( 0xff000000 & nColor) / 254;
	colorValue.r = (float)( 0x000000ff & nColor) / 254;
	colorValue.g = (float)( 0x0000ff00 & nColor) / 254;
	colorValue.b = (float)( 0x00ff0000 & nColor) / 254;
}

int SetColorValue(D3DCOLORVALUE &colorValue, lua_State*L, int nIndex)
{
	colorValue.a = (float)GetLuaTableNum(L,nIndex);
	colorValue.r = (float)GetLuaTableNum(L,nIndex - 1);
	colorValue.g = (float)GetLuaTableNum(L,nIndex - 2);
	colorValue.b = (float)GetLuaTableNum(L,nIndex - 3);
	return nIndex - 4;
}

static int _AddLight(lua_State *L)
{
	//	Check that this is a table
	if(!lua_istable(L,-1))
		return 0;

	//	The light structure
	D3DLIGHT9 light;
	int nIndex = -1;

	//	Zero out the light
	ZeroMemory(&light,sizeof(D3DLIGHT9));

	//	Get the Ambient light color
	nIndex = SetColorValue(light.Ambient, L, nIndex);
	
	//	Get the Diffused color
	nIndex = SetColorValue(light.Diffuse, L, nIndex);

	//	Get the Specular color
	nIndex = SetColorValue(light.Specular, L, nIndex);

	//	Get the Possitions
	light.Position.x = (float)GetLuaTableNum(L,nIndex);
	nIndex -= 1;
	light.Position.y = (float)GetLuaTableNum(L,nIndex);
	nIndex -= 1;
	light.Position.z = (float)GetLuaTableNum(L,nIndex);
	nIndex -= 1;

	//	Get the Type
	unsigned nType = (unsigned)GetLuaTableNum(L,nIndex);
	nIndex -= 1;

	//	Get the Range
	light.Range = (float)GetLuaTableNum(L,nIndex);
	nIndex -= 1;

	//	Get the Attunations
	light.Attenuation0 = (float)GetLuaTableNum(L, nIndex);
	nIndex -= 1;
	light.Attenuation1 = (float)GetLuaTableNum(L, nIndex);
	nIndex -= 1;
	light.Attenuation2 = (float)GetLuaTableNum(L, nIndex);
	nIndex -= 1;

	//	Load dependent on light type
	switch(nType)
	{
	case 0:		//	D3DLIGHT_POINT
		{
			//	Set the Light
			light.Type = D3DLIGHT_POINT;
		}
		break;
	case 1:		//	D3DLIGHT_DIRECTIONAL
		{
			//	Set the Light
			light.Type = D3DLIGHT_DIRECTIONAL;

			//	Get the directon
			light.Direction.x = (float)GetLuaTableNum(L, nIndex);
			nIndex -= 1;
			light.Direction.y = (float)GetLuaTableNum(L, nIndex);
			nIndex -= 1;
			light.Direction.z = (float)GetLuaTableNum(L, nIndex);
			nIndex -= 1;
		}
		break;
	case 2:		//	D3DLIGHT_SPOT
		{
			//	Set the Type
			light.Type = D3DLIGHT_SPOT;

			//	Get the directon
			light.Direction.x = (float)GetLuaTableNum(L, nIndex);
			nIndex -= 1;
			light.Direction.y = (float)GetLuaTableNum(L, nIndex);
			nIndex -= 1;
			light.Direction.z = (float)GetLuaTableNum(L, nIndex);
			nIndex -= 1;

			//	Get Theta, Phi and falloff
			light.Theta = (float)GetLuaTableNum(L, nIndex);
			nIndex -= 1;
			light.Phi = (float)GetLuaTableNum(L, nIndex);
			nIndex -= 1;
			light.Falloff = (float)GetLuaTableNum(L, nIndex);
			nIndex -= 1;
		}
		break;
	}

	//	Pop the stack
	lua_pop(L,-nIndex);

	//	Add the light
	CIPack::IRender()->m_pRender->AddLight(&light);

	//	Returns nothing
	return 0;
}
int __EnableLight(lua_State *L)
{
	unsigned int nBool = (unsigned)lua_tonumber(L,-1);
	CIPack::IRender()->m_pRender->GetDevice()->SetRenderState(D3DRS_LIGHTING, (DWORD)nBool);
	lua_pop(L,1);
	return 0;
}

int __SetRenderState(lua_State *L)
{
	unsigned int nOpt2 = (unsigned)lua_tonumber(L,-1);
	unsigned int nOpt1 = (unsigned)lua_tonumber(L,-2);
	CIPack::IRender()->m_pRender->GetDevice()->SetRenderState((D3DRENDERSTATETYPE)nOpt1, nOpt2);
	lua_pop(L,2);
	return 0;
}

int _SetEnemyModel(lua_State *L)
{
	//	Get the Index
	int nIndex = (int)lua_tonumber(L,-2);

	//	Get the Enemy Model
	CIPack::IRender()->m_DefaultModelPack.nModelID[CIPack::ENEMY_1 + nIndex] = (int)lua_tonumber(L,-1);
	
	//	Pop the Stack
	lua_pop(L,2);

	//	Return nothing
	return 0;
}

int _GetEnemyModel(lua_State *L)
{	
	int nIndex = (int)lua_tonumber(L,-1);
	lua_pop(L,1);

	//	Get the Model
	lua_pushnumber(L,CIPack::IRender()->m_DefaultModelPack.nModelID[CIPack::ENEMY_1 + nIndex]);

	//	Return the Value
	return 1;
}

int _SetPlayerModel(lua_State *L)
{
	//	Get the Enemy Model
	CIPack::IRender()->m_DefaultModelPack.nModelID[CIPack::PLAYER] = (int)lua_tonumber(L,-1);
	
	//	Pop the Stack
	lua_pop(L,1);

	//	Return nothing
	return 0;
}

int _GetPlayerModel(lua_State *L)
{
	//	Get the Enemy Model
	lua_pushnumber(L,CIPack::DefaultModelPack(CIPack::PLAYER));

	//	Return nothing
	return 1;
}

int _SetBulletModel(lua_State *L)
{
	//	Get the Index
	unsigned int nIndex = (signed)lua_tonumber(L,-1);

	//	Check this is valid
	if(nIndex < 5)
	{
		//	Get the Enemy Mode
		CIPack::IRender()->m_DefaultModelPack.nModelID[CIPack::BULLET_1 + nIndex] = (signed)lua_tonumber(L,-2);

			//	Pop the Stack
		lua_pop(L,2);
	}

	//	Return nothing
	return 0;
}

int _GetBulletModel(lua_State *L)
{
	//	Get the Index
	unsigned int nIndex = (signed)lua_tonumber(L,-1);

	//	Check that the index is valid
	if(nIndex >= 4)
	{
		//	Push back invalid
		lua_pushnumber(L,-1);
	}
	else
	{
		//	Get the Enemy Model
		lua_pushnumber(L,CIPack::IRender()->m_DefaultModelPack.nModelID[CIPack::BULLET_1 + nIndex]);
	}

	//	Return nothing
	return 1;
}

int _SetPowerModel(lua_State *L)
{
	//	Get the Index
	unsigned int nIndex = (signed)lua_tonumber(L,-1);

	//	Check this is valid
	if(nIndex < 3)
	{
		//	Get the Powerup Model
		CIPack::IRender()->m_DefaultModelPack.nModelID[CIPack::POWER_UP_1 + nIndex] = (signed)lua_tonumber(L,-2);

			//	Pop the Stack
		lua_pop(L,2);
	}

	//	Return nothing
	return 0;
}

int _GetPowerModel(lua_State *L)
{
	//	Get the Index
	unsigned int nIndex = (signed)lua_tonumber(L,-1);

	//	Check that the index is valid
	if(nIndex >= 3)
	{
		//	Push back invalid
		lua_pushnumber(L,-1);
	}
	else
	{
		//	Get the Enemy Model
		lua_pushnumber(L,CIPack::DefaultModelPack((CIPack::MODEL_ID)(CIPack::POWER_UP_1 + nIndex)));
	}

	//	Return nothing
	return 1;
}

int _GetSkyBoxModel(lua_State *L)
{
	//	Push back the skybox mode data
	lua_pushnumber(L,CIPack::DefaultModelPack(CIPack::SKY_BOX));
	return 1;
}

int _SetSkyBoxModel(lua_State *L)
{
	//	Push back the skybox mode data
	CIPack::IRender()->m_DefaultModelPack.nModelID[CIPack::SKY_BOX] = (int)lua_tonumber(L,-1);
	lua_pop(L,1);

	//	Return Nothing
	return 0;
}

int _SetSkyBoxNearModel(lua_State *L)
{
	//	Push back the skybox mode data
	CIPack::IRender()->m_DefaultModelPack.nModelID[CIPack::SKY_BOX_NEAR] = (int)lua_tonumber(L,-1);
	lua_pop(L,1);

	//	Return Nothing
	return 0;
}

int _GetBossModel(lua_State *L)
{
	//	Push back the boss model data
	lua_pushnumber(L,CIPack::DefaultModelPack(CIPack::LEVEL_BOSS));
	return 1;
}

int _SetBossModel(lua_State *L)
{
	//	Push back the boss model data
	CIPack::IRender()->m_DefaultModelPack.nModelID[CIPack::LEVEL_BOSS] = (int)lua_tonumber(L,-1);
	lua_pop(L,1);

	//	Return Nothing
	return 0;
}

int _CreateModelPack(lua_State *L)
{
	//	Create and return the new model pack
	lua_pushnumber(L,CIPack::IRender()->CreateModelPack());
	return 1;
}

int _GetModelID(lua_State *L)
{
	//	Pop the data off the stack
	int nModelID = (int)lua_tonumber(L,-1);
	int nPackID = (int)lua_tonumber(L,-2);
	lua_pop(L,2);

	//	Return the model ID
	lua_pushnumber(L,CIPack::IRender()->GetModelID(nPackID,nModelID));

	//	Return a model ID
	return 1;
}

int _SetModelID(lua_State *L)
{
	//	Pop the data off the stack
	int nModelSet = (int)lua_tonumber(L,-1);
	int nModelID = (int)lua_tonumber(L,-2);
	int nPackID = (int)lua_tonumber(L,-3);
	lua_pop(L,3);

	//	Return the model ID
	CIPack::IRender()->SetModelID(nPackID,nModelID,nModelSet);

	//	Return a model ID
	return 0;
}

int _GS_SetCameraTarget(lua_State *L)
{
	CGameObject *pObject = 0;
	if(lua_islightuserdata(L,-1) == 1)
	{
		//	Check if this is an object
		pObject = (CGameObject*)lua_touserdata(L,-1);
	}
	lua_pop(L,1);

	if(pObject == 0)
	{
		pObject = new CGameObject();
		CIPack::IObject()->m_pObjectManager->Add(pObject);
	}

	//	Set the camera
	CCamera::GetInstance()->InitCameraToTarget(&pObject->GetWorldMatrix());

	return 0;
}

int __RR_SetMaterial(lua_State *L)
{
	int nIndex = -1;
	//	Get the object material
	if(lua_islightuserdata(L,-2) == 1)
	{
		//	Get the Object
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);

		//	Get the Mesh ID
		int nMeshID = (signed)lua_tonumber(L,-1);
		lua_pop(L,2);

		//	Get the ModelID
		unsigned int nModelID = pObject->GetModelIndex();

		//	Check that this is a table
		if(lua_istable(L,-1) == 1)
		{
			//	Check the mesh id
			D3DMATERIAL9 *pMaterial = &CIPack::IRender()->m_pRender->GetMaterial(nModelID,nMeshID);
			
			//	Get the Color
			nIndex = SetColorValue(pMaterial->Ambient, L, nIndex);
			nIndex = SetColorValue(pMaterial->Diffuse, L, nIndex);
			nIndex = SetColorValue(pMaterial->Emissive, L, nIndex);
			nIndex = SetColorValue(pMaterial->Specular, L, nIndex);
			pMaterial->Power = (float)lua_tonumber(L,nIndex);
			nIndex -= 1;
		}
		lua_pop(L,-1 * nIndex);

	}
	else
		lua_pop(L,2);
	return 0;
}

void CSRenderMan::LoadScript(const char *szScript)	{	}

void CSRenderMan::RegisterScriptFunctions(SLua *pLua)
{
	//	Register the Functions
	pLua->RegisterFunction("LoadModel",_LoadModel);
	pLua->RegisterFunction("LoadTexture",_LoadTexture);
	pLua->RegisterFunction("WriteText",_RenderText);

	//	Light Functions
	pLua->RegisterFunction("EnableLight",__EnableLight);
	pLua->RegisterFunction("AddLight",_AddLight);
	
	//	Get and Set
	pLua->RegisterFunction("SetEnemyModel",_SetEnemyModel);
	pLua->RegisterFunction("GetEnemyModel",_GetEnemyModel);
	pLua->RegisterFunction("SetPlayerModel",_SetPlayerModel);
	pLua->RegisterFunction("GetPlayerModel",_GetPlayerModel);
	pLua->RegisterFunction("SetBulletModel",_SetBulletModel);
	pLua->RegisterFunction("GetBulletModel",_GetBulletModel);
	pLua->RegisterFunction("SetPowerUpModel",_SetPowerModel);
	pLua->RegisterFunction("GetPowerUpModel",_GetPowerModel);
	pLua->RegisterFunction("SetSkyBoxModel",_SetSkyBoxModel);
	pLua->RegisterFunction("SetSkyBoxNearModel",_SetSkyBoxNearModel);
	pLua->RegisterFunction("GetSkyBoxModel",_GetSkyBoxModel);

	pLua->RegisterFunction("SetBossModel",_SetBossModel);
	pLua->RegisterFunction("GetBossModel",_GetBossModel);

	//	Model Pack Munipulation
	pLua->RegisterFunction("CreateModelPack",_CreateModelPack);
	pLua->RegisterFunction("GetModelID",_GetModelID);
	pLua->RegisterFunction("SetModelID",_SetModelID);

	pLua->RegisterFunction("SetRenderState",__SetRenderState);
	pLua->RegisterFunction("SetCameraTarget",_GS_SetCameraTarget);
	pLua->RegisterFunction("SetMaterial",__RR_SetMaterial);
}