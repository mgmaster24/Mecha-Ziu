/*********************************************************************************
File Name	:	LuaHeader

Description	:	Contains definitions for the lua header files
*********************************************************************************/
#ifndef COLD_LUA_HEADER_H
#define COLD_LUA_HEADER_H
extern "C" 
{
#include "lua/src/lua.h"
#include "lua/src/lualib.h"
#include "lua/src/lauxlib.h"
}

#ifndef _LUA_LIB
#define _LUA_LIB
#pragma comment(lib, "lua5.1.3.lib")
#endif

#endif