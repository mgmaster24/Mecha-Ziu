#include "ScriptSystem.h"

CScriptSystem::CScriptSystem(void)
{
}

CScriptSystem::~CScriptSystem(void)
{
}

CScriptSystem *CScriptSystem::GetInstance()
{
	//	Create and return the only instance of the class
	static CScriptSystem Instance;
	return &Instance;
}
bool CScriptSystem::Init(const char *szProcFile, const char *szProcFunction, bool bCallEntrance)
{
	//	Clear All scripts currently in the system
	ClearScripts();

	//	Check that the pointers are not null
	if(szProcFile == 0 || szProcFunction == 0)
		return false;

	//	Register this class
	RegisterClass<CScriptSystem>(this, "scriptSys");

	//	Load the external scripts
	if(LoadExternalScript(szProcFile,szProcFunction,bCallEntrance,true) == -1)
		return false;

	//	return successfull
	return true;
}

void CScriptSystem::Shutdown(void)
{
	//	Clear All Scripts
	ClearScripts();
}

void CScriptSystem::ClearScripts()
{
	int nSize = m_LuaStateArray.size();
	//	Itterate and clear the scripts
	int i = 0;
	for(i = 0; i < nSize; i++)
	{
		//	Check that this is not already null
		if(m_LuaStateArray[i] != 0)
		{
			delete m_LuaStateArray[i];
			m_LuaStateArray[i] = 0;
		}
	}

	//	Get the Size of the reg array
	nSize = m_ScriptRegInfo.size();
	for(i = 0; i < nSize; i++)
	{
		//	Check that this is not already null
		if(m_ScriptRegInfo[i] != 0)
		{
			delete m_ScriptRegInfo[i];
			m_ScriptRegInfo[i] = 0;
		}
	}

	//	Clear the Array
	m_LuaStateArray.clear();
	m_LuaStateArray.SetSize(0);
}
SLua *CScriptSystem::GetLuaInterface(lua_State *L)
{
	//	Pointer to teh current lua state
	lua_State *pLuaState = 0;

	//	Itterate and find the state
	unsigned int nSize = m_LuaStateArray.size();

	//	Itterate and find the Lua State
	for(unsigned int i = 0; i < nSize; i++)
	{
		//	Check that the array is not null
		if(m_LuaStateArray[i] == 0)
			continue;

		//	Get the Lua Interface
		pLuaState = m_LuaStateArray[i]->LuaInterface.GetLuaState();

		//	Check for a matching state
		if(pLuaState == L)
			return &m_LuaStateArray[i]->LuaInterface;
	}

	//	Return Nothing
	return 0;
}

SLua *CScriptSystem::GetLuaIndex(int nIndex)	
{
	if((unsigned)nIndex >= m_LuaStateArray.size())
		return 0;

	//	Return the lua interface
	return &m_LuaStateArray[nIndex]->LuaInterface;
}

int CScriptSystem::_GetScriptID(int nHash)
{
	int nIndex = -1;
	//	Get the Number of Objects
	unsigned int nSize = m_LuaStateArray.size();
	for(unsigned int i = 0; i < nSize; i++)
	{
		//	Check that the array is not null
		if(m_LuaStateArray[i] == 0)
			continue;

		//	Check for a match
		if(m_LuaStateArray[i]->nHashValue == nHash)
		{
			nIndex = i;
			break;
		}
	}

	//	Return the Index
	return nIndex;
}
int CScriptSystem::_GetScriptID(lua_State *L)
{
	int nLuaIndex = -1;

	//	Get the Size of the Lua Index
	int nSize = (signed)m_LuaStateArray.size();;

	//	Itterate and find the Lua State
	for(int i = 0; i < nSize; i++)
	{
		//	Check that the array is not null
		if(m_LuaStateArray[i] == 0)
			continue;

		//	Check for a matching state
		if(m_LuaStateArray[i]->LuaInterface.GetLuaState() == L)
		{
			//	Set the index
			nLuaIndex = i;
			break;
		}
	}
	//	Return the Index
	return nLuaIndex;
}
int CScriptSystem::_GetScriptID(SLua *lua)
{
	//	If invalid, return
	if(lua == 0)
		return - 1;

	//	Get the Lua State
	return _GetScriptID(const_cast<lua_State*>(lua->GetLuaState()));
}

int CScriptSystem::_ScriptHash(const char *szScript)
{
	const int HashItter = 9;
	//	Get the String Length
	unsigned int nStrLength = (unsigned)strlen(szScript);

	//	Check that the string is less then 9
	if(nStrLength > HashItter)	nStrLength = HashItter;

	//	The hashed value
	int nHashValue = -1;

	//	Get the Half Value
	int half = (szScript[0] >> 1);

	//	Itterate and Hash the string
	for(unsigned int i = 0; i < nStrLength; i++)
		nHashValue += (szScript[i] << 1) + half;

	//	Return the Hash Value
	return nHashValue;
}
void CScriptSystem::_LoadScriptCall(int nIndex, const char *szScript)
{
	//	Check for Valid Index
	if(nIndex == -1)
		return;

	//	Call the function at the index
	m_ScriptRegInfo[nIndex]->cScriptFunc.LoadScript(szScript);
}

int CScriptSystem::LoadExternalScript(const char *szScriptName, const char *szEntrance,
				bool bCallEntrance, bool bCallScript)
{
	int scriptIndex = -1;
	int emptyIndex = -1;
	tSLuaInterface *luaInterface = 0;

	//	Check for a valid pointer
	if(szScriptName == 0 || szEntrance == 0)
		return scriptIndex;

	//	Itterate and check if this script already exists
	unsigned int nIndex = m_LuaStateArray.size();
	for(unsigned i = 0; i < nIndex; i++)
	{
		//	Check for an empty index
		if(m_LuaStateArray[i] == 0)
		{
			emptyIndex = i;
			break;
		}
		
		//	Check if this already exists
		if(_stricmp(szScriptName,m_LuaStateArray[i]->szFileName.c_str()) == 0)
		{
			//	Set the lut interface pointer
			luaInterface = m_LuaStateArray[i];

			//	Be sure that the current file is closed
			m_LuaStateArray[i]->LuaInterface.CloseFile();

			//	Set the Index
			scriptIndex = i;
			break;
		}
	}

	//	Check the script index
	if(scriptIndex == -1)
	{
		//	Create a new Index
		luaInterface = new tSLuaInterface();

		//	Open the Lua File
		if(luaInterface->LuaInterface.OpenFile(szScriptName) == false)
			return -1;

		//	Set the File Name
		luaInterface->szFileName = szScriptName;

		//	Check if this is being placed in an empty index
		if(emptyIndex == -1)
		{
			//	Push the interface onto the array
			m_LuaStateArray.AddObject(luaInterface);

			//	Set the Script Index
			scriptIndex = m_LuaStateArray.size() - 1;
		}
		else
		{
			//	Set the script index
			scriptIndex = emptyIndex;

			//	Place the object at the proper index
			m_LuaStateArray[scriptIndex] = luaInterface;
		}
	}
	else
	{
		//	Open the Lua File
		if(luaInterface->LuaInterface.OpenFile(szScriptName) == false)
			return -1;
	}

	//	Set the Entrance
	luaInterface->LuaInterface.SetEntrance(szEntrance);

	//	Register the script system functions
	RegisterScriptFunctions(&luaInterface->LuaInterface);

	//	Call the script
	if(bCallScript == true)
	{
		if(luaInterface->LuaInterface.CallFunction() == false)
			scriptIndex = -1;
	}

	//	Call the Entrance Function
	if(bCallEntrance == true)
	{
		if(luaInterface->LuaInterface.CallFunction(szEntrance,0,0) == false)
			scriptIndex = -1;
	}

	//	Check the Script Index
	if(scriptIndex == -1)
	{
		RemoveScript(m_LuaStateArray.size() - 1);
	}

	//	return successfull
	return scriptIndex;
}

bool CScriptSystem::RunScript(SLua *lua, bool bCallEntrance, bool bCallScript)
{
	//	Bool to Check Returns
	bool bReturn = false;

	//	Run the Selected Script
	if(bCallScript == true)
		bReturn = lua->CallFunction();
	//	Call the Script Entrance
	if(bCallEntrance == true)
		bReturn = lua->UseScript();

	//	Return the execution
	return bReturn;
}
bool CScriptSystem::RunScript(int nScriptIndex, bool bCallEntrance, bool bCallScript)
{
	//	Bool to Check Returns
	bool bReturn = false;

	//	Run the Selected Script
	if(bCallScript == true)
		bReturn = m_LuaStateArray[nScriptIndex]->LuaInterface.CallFunction();
	//	Call the Script Entrance
	if(bCallEntrance == true)
		bReturn = m_LuaStateArray[nScriptIndex]->LuaInterface.UseScript();

	//	Return the execution
	return bReturn;
}

void CScriptSystem::RemoveScript(int nIndex)
{
	//	Check that the index is valid
	if(nIndex < 0 || nIndex >= (signed)m_LuaStateArray.size())
		return;

	if(m_LuaStateArray[nIndex] == 0)
		return;

	//	Close the State
	m_LuaStateArray[nIndex]->LuaInterface.CloseState();
	
	//	Delete the Script
	delete m_LuaStateArray[nIndex];

	//	Null the Script
	m_LuaStateArray[nIndex] = 0;
}

///////////////////////////////////////////////////////////////////////////////////
//	LUA	SCRIPT FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////
static int LoadLuaLibraries(lua_State *L)
{
	//	Check that this is a string
	if(lua_isstring(L,-1) == 0)
		return 0;

	//	Get the String
	std::string libString = lua_tostring(L,-1);

	//	Open the Proper Library
	if(libString == "all")
		luaL_openlibs(L);
	else if(libString == "io")
		luaopen_io(L);
	else if(libString == "math")
		luaopen_math(L);
	else if(libString == "string")
		luaopen_string(L);
	else if(libString == "debug")
		luaopen_debug(L);
	else if(libString == "os")
		luaopen_os(L);
	else if(libString == "table")
		luaopen_table(L);
	else if(libString == "package")
		luaopen_package(L);

	//	Return nothing
	return 0;
}
static int GetLibraryId(lua_State *L)
{
	//	Itterate and find the index
	//	Check that this is a string
	if(lua_isstring(L,-1) == 0)
		return 0;

	//	Get the String
	std::string libString = lua_tostring(L,-1);

	//	Get a pointer to the script system
	CScriptSystem *system = CScriptSystem::GetInstance();

	//	Get the Hash String
	int nHashString = system->_ScriptHash(libString.c_str());

	int nIndex = -1;
	//	Get the size
	unsigned int nSize = system->GetRegInfoArray()->size();

	//	Itterage and find a matching library name
	for(unsigned int itter = 0; itter < nSize; itter++)
	{
		//	Check for a match
		if((*system->GetRegInfoArray())[itter]->nHashValue == nHashString)
		{
			nIndex = itter;
			break;
		}
	}

	//	Push Back Index
	lua_pushinteger(L,nIndex);

	//	Return the index of a library
	return 1;
}
static int GetLuaFileIndex(lua_State *L)
{
	//	Get the Script System Pointer
	CScriptSystem *system = CScriptSystem::GetInstance();

	//	Get the Script ID
	int nLuaIndex = system->_GetScriptID(L);

	//	Push back the index
	lua_pushnumber(L,nLuaIndex);

	//	Return the index
	return 1;
}

static int LoadGameLibrary(lua_State *L)
{
	//	Check that this is a string
	if(lua_isstring(L,-1) == 1)
	{
		//	Get the String
		std::string libString = lua_tostring(L,-1);

		//	Get a pointer to the script system
		CScriptSystem *system = CScriptSystem::GetInstance();

		//	Get a pointer to the Reg Info Array
		(*system->GetRegInfoArray())[0];

		//	Get the Hash String
		int nHashString = system->_ScriptHash(libString.c_str());

		//	Get the size
		unsigned int nSize = system->GetRegInfoArray()->size();

		//	Check if we want to load all game libraries
		if(nHashString == system->_ScriptHash("all"))
		{
			//	Itterage and find a matching library name
			for(unsigned int itter = 0; itter < nSize; itter++)
			{
				//	Load the library
				(*system->GetRegInfoArray())[itter]->cScriptFunc.Register(system->GetLuaInterface(L));
			}
		}
		else
		{
			//	Itterage and find a matching library name
			for(unsigned int itter = 0; itter < nSize; itter++)
			{
				//	Check for a match
				if((*system->GetRegInfoArray())[itter]->nHashValue == nHashString)
				{
					(*system->GetRegInfoArray())[itter]->cScriptFunc.Register(system->GetLuaInterface(L));
					break;
				}
			}
		}
	}
	else if(lua_isnumber(L,-1) == 1)
	{
		//	Get the index
		int nIndex = (signed)lua_tointeger(L,-1);
		lua_pop(L,1);

		//	Get a pointer to the script system
		CScriptSystem *system = CScriptSystem::GetInstance();

		//	Check that the index is valid
		if(nIndex < 0 || (unsigned)nIndex >= system->GetRegInfoArray()->size())
			return 0;

		//	Call the array information
		(*system->GetRegInfoArray())[nIndex]->cScriptFunc.Register(system->GetLuaInterface(L));
	}

	//	Return Nothing;
	return 0;
}

static int RegLoadScript(lua_State *L)
{
	//	Pulls string to register
	std::string szEntrance = lua_tostring(L,-1);
	lua_pop(L,1);
	std::string szFile = lua_tostring(L,-1);
	lua_pop(L,1);

	//	Load the External file
	CScriptSystem::GetInstance()->LoadExternalScript(szFile.c_str(),szEntrance.c_str());

	//	Return Nothing
	return 0;
}

static int LibLoadScript(lua_State *L)
{
	//	Get the GameLib ID
	int nScriptID = (int)lua_tointeger(L,-1);
	const char *szScript = lua_tostring(L,-2);

	//	Call the load script function
	CScriptSystem::GetInstance()->_LoadScriptCall(nScriptID,szScript);

	//	Pop the Stack
	lua_pop(L,2);

	//	Return Nothing
	return 0;
}

void CScriptSystem::RegisterScriptFunctions(SLua *pLua)
{
	//	Register the libraries with the state
	pLua->RegisterFunction("LoadLuaLib",LoadLuaLibraries);
	pLua->RegisterFunction("LoadGameLib",LoadGameLibrary);
	pLua->RegisterFunction("GetLibID",GetLibraryId);
	pLua->RegisterFunction("GetScriptID",GetLuaFileIndex);
	pLua->RegisterFunction("RegisterFunction",RegLoadScript);
	pLua->RegisterFunction("LibLoadScript",LibLoadScript);
}