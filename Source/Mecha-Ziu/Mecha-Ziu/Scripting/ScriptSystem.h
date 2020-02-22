/**********************************************************************************
File Name	:	ScriptSystem

Description	:	Contains definitions and interface for using external scripts
					within the game, and linking c*c++ code to the server
**********************************************************************************/
#ifndef COLD_SCRIPT_SYSTEM_H
#define COLD_SCRIPT_SYSTEM_H
#include "SLua.h"
#include "../CArray.h"
#include "CScriptFuncPtr.h"

class CScriptSystem
{
public:
	/**********************************************************************************
	Description	:	Contains information regarding a script file in use
	**********************************************************************************/
	typedef struct _tSLuaInterface
	{
		std::string szFileName;		//	The Name of the File Being Loaded
		int nHashValue;				//	Hash value for quick hunting scripts
		SLua LuaInterface;			//	Interface into the lua system
	}tSLuaInterface;
private:
	/**********************************************************************************
	Description	:	Contains information regarting the registered systems
	**********************************************************************************/
	typedef struct _tScriptSysRegInfo
	{
		int nHashValue;				//	Hash value for quick hunting scripts
		std::string szSysName;		//	The lua of the Interface system
		CScriptFunc cScriptFunc;	//	Pointer to the Class Interface
	}tScriptReg;
private:
	CArray<tSLuaInterface*> m_LuaStateArray;	//	Array of Lua State Scripts
	CArray<tScriptReg*>	m_ScriptRegInfo;		//	Aray of script registration info

	//	Constructor
	CScriptSystem(void);
	CScriptSystem(const CScriptSystem&);
	CScriptSystem &operator=(const CScriptSystem&);
public:
	//	Destructor
	~CScriptSystem(void);

	/**********************************************************************************
	Description	:	Return the Only Instance of this class
	**********************************************************************************/
	static CScriptSystem *GetInstance();

	/**********************************************************************************
	Description	:	Return The system registration array
	**********************************************************************************/
	CArray<_tScriptSysRegInfo*> *GetRegInfoArray()	{return &m_ScriptRegInfo;}

	/**********************************************************************************
	Description	:	Return Index Lua State
	**********************************************************************************/
	SLua *GetLuaIndex(int nIndex);
	SLua *GetLuaInterface(lua_State *L);

	/**********************************************************************************
	Description	:	Call the Selected Script
	**********************************************************************************/
	bool RunScript(SLua *lua, bool bCallEntrance = false, bool bCallScript = true);
	bool RunScript(int nScriptIndex, bool bCallEntrance = false, bool bCallScript = true);

	/**********************************************************************************
	Description	:	Init the Script System
	**********************************************************************************/
	bool Init(const char *szProcFile, const char *szProcFunction, bool bCallEntrance = true);

	/**********************************************************************************
	Description	:	Shutdown the Script System
	**********************************************************************************/
	void Shutdown(void);

	/**********************************************************************************
	Description	:	Register a Compatable Class with the lua system
	**********************************************************************************/
	template<typename TYPE>
	int RegisterClass(TYPE *Object, const char *szClassTag);

	template<typename TYPE>
	int _Register_Class(TYPE *Object, const char *szClassTag);

	/**********************************************************************************
	Description	:	Clear all scripts in the system
	**********************************************************************************/
	void ClearScripts();

	/**********************************************************************************
	Description	:	Remove the script from the system
	**********************************************************************************/
	void RemoveScript(int nIndex);

	/**********************************************************************************
	Description	:	Load a script external to the scripting system
	**********************************************************************************/
	int LoadExternalScript(const char *szScriptName, const char *szEntrance, 
		bool bCallEntrance = false, bool bCallScript = false);

public:
	/**********************************************************************************
	Description	:	Register all scripting system functions
	**********************************************************************************/
	void RegisterScriptFunctions(SLua *pLua);

	/**********************************************************************************
	Description	:	Register all scripting system functions
	**********************************************************************************/
	void LoadScript(const char *szScript) {}

	/**********************************************************************************
	Description	:	Return a script Id from a value
	**********************************************************************************/
	int _GetScriptID(int nHash);
	int _GetScriptID(lua_State *L);
	int _GetScriptID(SLua *lua);

	/**********************************************************************************
	Description	:	Get the Hash value for a script name
	***********************************************************************************/
	int _ScriptHash(const char *szScript);

	/**********************************************************************************
	Description	:	Call the load script function on the selected library
	**********************************************************************************/
	void _LoadScriptCall(int nIndex, const char *szScript);
};

template<typename TYPE>
int CScriptSystem::_Register_Class(TYPE *Object, const char *szClassTag)
{
	return RegisterClass<TYPE>(Object,szClassTag);
}

template<typename TYPE>
int CScriptSystem::RegisterClass(TYPE *Object, const char *szClassTag)
{
	//	The class registered index to return
	int nLuaStateID = -1;

	//	Check fora  valid string
	if(szClassTag == 0)
		return nLuaStateID;

	//	Get the Size of the Registration array
	unsigned int nRegSize = m_ScriptRegInfo.size();

	//	Itterate and check if this object already exists
	for(unsigned int regItter = 0; regItter < nRegSize; regItter++)
	{
		//	Compare the strings
		if(strcmp(szClassTag, m_ScriptRegInfo[regItter]->szSysName.c_str()) == 0)
		{
			//	Rebind the object
			if(Object != 0)
			{	m_ScriptRegInfo[regItter]->cScriptFunc.Bind<TYPE>(Object);	}

			//	return the index
			return regItter;
		}
	}
	
	//	Set the current script index
	nLuaStateID = (signed)m_ScriptRegInfo.size();

	//	Create a new script reg
	_tScriptSysRegInfo *newScriptReg = new _tScriptSysRegInfo();

	//	Register this class
	newScriptReg->cScriptFunc.Bind<TYPE>(Object);

	//	Set the lua string
	newScriptReg->szSysName = szClassTag;

	//	Set the Hash Function
	newScriptReg->nHashValue = _ScriptHash(szClassTag);

	//	Add the system to the list
	m_ScriptRegInfo.AddObject(newScriptReg);

	//	Return the class regiser Index
	return nLuaStateID;
}

#endif