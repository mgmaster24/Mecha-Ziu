/*************************************************************************************
File Name	:	CScriptFuncPtr.h

Description	:	Provides an abstract interface for calling script functions within
				a class
*************************************************************************************/
#ifndef COLD_FUNCTION_PTR_RENDER_H
#define COLD_FUNCTION_PTR_RENDER_H

class CScriptFuncPtr_Base
{
public:
	virtual void Register(SLua *pLua) = 0;
	virtual void LoadScript(const char *szScript) = 0;
};

template<class TYPE>
class CScriptFuncPtr : public CScriptFuncPtr_Base
{
public:
	TYPE *genFunc;

public:
	CScriptFuncPtr(TYPE *funcObj)	: genFunc(funcObj) {}
	CScriptFuncPtr()	: genFunc(0) {}
	~CScriptFuncPtr()	{}

	void set(TYPE *funcObj)
	{	genFunc = funcObj;	}
	virtual void Register(SLua *pLua)
	{	genFunc->RegisterScriptFunctions(pLua);	}
	virtual void LoadScript(const char *szScript)
	{	genFunc->LoadScript(szScript);	}
};

template<class TYPE>
CScriptFuncPtr_Base *GetFunctionObject(TYPE *object)
{	
	//	Check that the pointer is valid
	if(object == 0)
		return 0;

	//	Create and return the new object
	CScriptFuncPtr<TYPE> *pObject = new CScriptFuncPtr<TYPE>;
	pObject->set(object);

	//	Return the object
	return pObject;
};

class CScriptFunc
{
private:
	CScriptFuncPtr_Base *m_FuncBase;
public:
	//	Constructor
	CScriptFunc() : m_FuncBase(0) {}
	//	Destructor
	~CScriptFunc()
	{	
		//	Check that the pointer exists
		if(m_FuncBase != 0)
		{
			delete m_FuncBase;
			m_FuncBase = 0;
		}
	}

	//	Set the Function
	template<typename TYPE>
	void Bind(TYPE *object)
	{
		//	Check that the pointer is valid
		if(object == 0)
			return;

		//	Check that the function does not exist
		if(m_FuncBase != 0)
		{	delete m_FuncBase;
			m_FuncBase = 0;	}

		//	Create and return the new object
		m_FuncBase = new CScriptFuncPtr<TYPE>(object);
	}

	void Register(SLua *pLua)
	{	m_FuncBase->Register(pLua);	}
	void LoadScript(const char *szScript)
	{	m_FuncBase->LoadScript(szScript);	}
};

#endif