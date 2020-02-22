#include "SLua.h"

SLua::SLua(void)	:	m_LuaState(0), m_nNumArguments(0), m_nNumReturned(0)
{
}

SLua::SLua(const char *szFile, const char szEntrance)	:	m_LuaState(0), m_nNumArguments(0), m_nNumReturned(0)
{
	//	Open the File
	OpenFile(szFile);
}

SLua::~SLua(void)
{
	//	Close the Lua State
	CloseState();
}

void SLua::OpenState(void)
{
	//	If the Lua State Exists, close it
	if(m_LuaState != 0)
		lua_close(m_LuaState);

	//	Init the Lua State
	m_LuaState = lua_open();
}

bool SLua::OpenFile(const char *szFile)
{
	//	Check that the pointer is valid
	if(szFile == 0)
		return false;

	//	Check the Lua State
	if(m_LuaState == 0)
		OpenState();

	//	Set the file we are using
	return luaL_loadfile(m_LuaState,szFile) == 0;
}

void SLua::SetEntrance(const char *szEntrance)
{
	m_szEntrance = szEntrance;
}

void SLua::CloseFile()	
{ 
	if(m_LuaState != 0)
	{
		lua_close(m_LuaState);
		m_LuaState = 0;
	}
}

void SLua::RegisterFunction(const char *szFuncName, void *function)
{
	//	Check that the Function is not Null
	if(szFuncName == 0 || function == 0)
		return;

	//	Register the function with lua
	lua_register(m_LuaState,szFuncName,(lua_CFunction)function);
}

void SLua::LockFunction(const char *szName)
{
	//	Check that the pointer is not null
	if(szName == 0)
		return;

	//	Lock the Variable/Function Name
	lua_getglobal(m_LuaState, szName);
}

void SLua::SetFunctionData(const char *szName, int numReturns, int numArguments)
{
	//	Set the Local Variables and Prepare for a function to be called
	m_nNumArguments = numArguments;
	m_nNumReturned = numReturns;
	
	//	Set the Funtion
	LockFunction(szName);
}

bool SLua::CallFunction(const char *szName, int numReturns, int numArguments)
{
	//	Check for valid string
	if(szName == 0)
		return false;

	//	Lock the Variable/Function Name
	lua_getglobal(m_LuaState, szName);

	//	Call the function
	return lua_pcall(m_LuaState,numArguments,numReturns,0) == 0;
}

bool SLua::CallFunction()
{
	//	Call Lua and return false is an error has occured
	return lua_pcall(m_LuaState,m_nNumArguments,m_nNumReturned,0) == 0;
}

void SLua::LockVariable(const char *szName)
{
	//	Check that the pointer is not null
	if(szName == 0)
		return;
	//	Lock the Variable/Function Name
	lua_getglobal(m_LuaState, szName);
}

void SLua::PushVariable(int var)
{
	//	Push the value onto the stack
	lua_pushnumber(m_LuaState,var);
}
void SLua::PushVariable(unsigned int var)
{
	//	Push the value onto the stack
	lua_pushnumber(m_LuaState,var);
}
void SLua::PushVariable(double var)
{
	//	Push the value onto the stack
	lua_pushnumber(m_LuaState,var);
}
void SLua::PushVariable(const char *szVar)
{
	//	Push the value onto the stack
	lua_pushstring(m_LuaState,szVar);
}
bool SLua::UseScript()
{
	//	Call the entrance function
	return CallFunction(m_szEntrance.c_str(),0,0);
}

std::string SLua::PopString(int var)
{
	//	Check for a valid string
	if(luaL_checkstring(m_LuaState,var) == 0)
		return std::string("");

	//	Return a string from the stack
	std::string string = lua_tostring(m_LuaState,var);
	lua_pop(m_LuaState,-var);
	return string;
}
int SLua::PopInt(int var)
{
	//	Pop a Number off the stack
	int nVal = (int)lua_tonumber(m_LuaState,var);
	PopStack(-var);
	return nVal;
}
double SLua::PopDouble(int var)
{
	//	Return a double fromt he lua stack
	double dVal = lua_tonumber(m_LuaState,var);
	PopStack(-var);
	return dVal;
}

void SLua::PopStack(int var)
{
	//	Pop and object off the lua stack
	lua_pop(m_LuaState,var);
}

int SLua::GetTopOfStack(void)
{
	//	Return the Top of the lua stack
	return lua_gettop(m_LuaState);
}

void SLua::CloseState(void)
{
	//	If the Lua State Exists, close it
	CloseFile();
}