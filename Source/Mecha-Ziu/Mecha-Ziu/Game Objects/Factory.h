/*****************************************************************************
File Name	:	Factory

Description	:	A factory/warehouse for creating game objects
*****************************************************************************/
#ifndef COLD_GAME_FACTORY_H
#define COLD_GAME_FACTORY_H
#include "../CArray.h"

class CFactory
{
public:
	/*****************************************************************************
	Description	:	A base for the factory type
	*****************************************************************************/
	class tDType
	{
	public:
		/*****************************************************************************
		Description	:	Base Abstract member functions for Objects
		*****************************************************************************/
		virtual void Clear() = 0;
		virtual void* RequestType() = 0;
		virtual void ReturnType(void* pReturn) = 0;
	};

public:
	/*****************************************************************************
	Description	:	Templated factory type stored within factory array
	*****************************************************************************/
	template<class TYPE>
	class tFactoryType	: public tDType
	{
	public:
		CArray<TYPE *>			m_ObjectArray;		//	Array of game objects
		int						m_nNumCreated;		//	The number of objects created
		int						m_nMaxObjects;		//	The maximum number of objects to create

		//	Constructor
		tFactoryType()	:	m_nMaxObjects(0), m_nNumCreated(-1)	{}
		~tFactoryType()	{	Clear();	}

		/*****************************************************************************
		Description	:	Accessors and Mutators
		*****************************************************************************/
		int GetCreated()				{return m_nNumCreated;}
		int GetMax()					{return m_nMaxObjects;}
		void SetCreated(int nCreated)	{m_nNumCreated = nCreated;}
		void SetMax(int nMax)			{m_nMaxObjects = nMax;}

		/*****************************************************************************
		Description	:	Clear all objects in the factory type
		*****************************************************************************/
		void Clear();

		/*****************************************************************************
		Description	:	Create a set number of objects
		*****************************************************************************/
		void CreateType(int nNum);

		/*****************************************************************************
		Description	:	Request Object from the factory type
		*****************************************************************************/
		void* RequestType();

		/*****************************************************************************
		Description	:	Return the type to the factory
		*****************************************************************************/
		void ReturnType(void* pReturn);
	};
private:
	//	Array of Factory Generated Objects
	CArray<tDType *>		m_FactoryTypes;		//	Array of factory object types
private:
	
	//	Private in order to make this a singleton
	CFactory();
	CFactory(const CFactory&);
	CFactory &operator=(const CFactory&);
public:
	
	//	Destructor
	~CFactory();

	/*****************************************************************************
	Description	:	Clear all objects in arrays
	*****************************************************************************/
	static CFactory* GetInstance(); 

	/*****************************************************************************
	Description	:	Clear all objects in arrays
	*****************************************************************************/
	void ClearAll();

	/*****************************************************************************
	Description	:	Request a factory type
	*****************************************************************************/
	template<class TYPE>
	CFactory::tFactoryType<TYPE> *GetFactoryType(int nFactoryType = -1);

	/*****************************************************************************
	Description	:	Request Object from the factory
	*****************************************************************************/
	void *GetObject(int nFactoryType);
	template<class TYPE>
	TYPE *GetObjectType(int nFactoryType);

	/*****************************************************************************
	Description	:	Return the type to the factory
	*****************************************************************************/
	void ReturnObject(void *pObject, int OnObjectID);

	/*****************************************************************************
	Description	:	Create factory type
	*****************************************************************************/
	template<class TYPE>
	int CreateFactoryType(int nCreate = 0, int nMaxObjects = -1);
};

template<class TYPE>
void CFactory::tFactoryType<TYPE>::Clear()
{
	//	Get the number of objects
	int nSize = m_ObjectArray.size();
	for(int i = 0; i < nSize; i++)
	{
		if(m_ObjectArray[i] != 0)
		{
			delete m_ObjectArray[i];
			m_ObjectArray[i] = 0;
		}
	}
	m_ObjectArray.clear();
	m_nNumCreated = 0;
	m_nMaxObjects = -1;
}

template<class TYPE>
int CFactory::CreateFactoryType(int nCreate, int nMaxObjects)
{
	//	Check for a valid index
	int nIndex = -1;

	int nSize = m_FactoryTypes.size();

	//	Check if a empty index exists in the array
	for(int i = 0; i < nSize; i++)
	{
		if(m_FactoryTypes[i] == 0)
			nIndex = i;
	}

	//	Check and set the index to the last index
	if(nIndex == -1)
		nIndex = nSize;

	//	Create a new index
	CFactory::tFactoryType<TYPE> *facType = new CFactory::tFactoryType<TYPE>();

	//	Set the max objects
	facType->m_nMaxObjects = nMaxObjects;

	//	Set and create the objects
	//for(int i = 0; i < nCreate; i++)
	facType->CreateType(nCreate);

	//	Add the factory type
	m_FactoryTypes.AddObject(facType);

	//	Return the Index
	return nIndex;
}

template<class TYPE>
void CFactory::tFactoryType<TYPE>::CreateType(int nNum)
{
	//	Check that th num wont put us beyond the max num
	if(m_nMaxObjects != -1)
	{
		if(m_nNumCreated + nNum > m_nMaxObjects)
			nNum = m_nNumCreated - nNum;
	}

	//	Itterate and create the set number of objects
	for(int i = 0; i < nNum; i++)
		m_ObjectArray.AddObject(new TYPE());
	
	//	Check that the number created is not invalid
	if(m_nNumCreated == -1)
		m_nNumCreated = 0;
	m_nNumCreated += nNum;
}

template<class TYPE>
void *CFactory::tFactoryType<TYPE>::RequestType()
{
	void *type = 0;
	unsigned nSize = m_ObjectArray.size();
	//	Check if an object exists
	if(nSize == 0)
	{
		//	Check that th num wont put us beyond the max num
		if(m_nMaxObjects != -1)
		{
			if(m_nNumCreated + 1 > m_nMaxObjects)
				return 0;
		}

		//	Create a new Object
		m_nNumCreated += 1;
		return (void*)(new TYPE());
	}

	//	Check how many Objects are in the factory
	else
	{
		//	Reduce the number in the factory
		type = m_ObjectArray[0];
		m_ObjectArray.RemoveObject(0);
	}
	return type;
}

template<class TYPE>
void CFactory::tFactoryType<TYPE>::ReturnType(void *pReturn)
{
	m_ObjectArray.AddObject((TYPE*)pReturn);
}

template<class TYPE>
CFactory::tFactoryType<TYPE> *CFactory::GetFactoryType(int nFactoryType)
{
	//	Check if this index is valid
	if(nFactoryType == -1 || nFactoryType >= m_FactoryTypes.size())
		return 0;

	//	Return the factory Type
	return m_FactoryTypes[nFactoryType];
}

template<class TYPE>
TYPE *CFactory::GetObjectType(int nFactoryType)
{
	return (TYPE*)m_FactoryTypes[nFactoryType]->RequestType();
}

#endif