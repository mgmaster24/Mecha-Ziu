#include "Factory.h"

CFactory::CFactory()
{
}

CFactory::~CFactory()
{
	ClearAll();
}

CFactory* CFactory::GetInstance()
{
	static CFactory instance; 
	return &instance;
}

void CFactory::ClearAll()
{
	//	Get the number of arrays
	int nSize = m_FactoryTypes.size();
	for(int i = 0; i < nSize; i++)
	{
		if(m_FactoryTypes[i] != 0)
		{
			m_FactoryTypes[i]->Clear();
			delete m_FactoryTypes[i];
			m_FactoryTypes[i] = 0;
		}
	}
	m_FactoryTypes.clear();
}

void CFactory::ReturnObject(void *pObject, int OnObjectID)
{
	if(OnObjectID == -1)
		return;

	m_FactoryTypes[OnObjectID]->ReturnType(pObject);
	pObject = 0;
}

void *CFactory::GetObject(int nFactoryType)
{
	//	Check if this index is valid
	if(nFactoryType == -1 || nFactoryType >= (signed)m_FactoryTypes.size())
		return 0;

	return (void*)m_FactoryTypes[nFactoryType]->RequestType();
}