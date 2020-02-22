/******************************************************************************
File Name	:	SEnemyPak

Description	:	Stores loaded enemy data to use to create enemies
******************************************************************************/
#ifndef SCRIPT_ENEMY_PACKAGE_H
#define SCRIPT_ENEMY_PACKAGE_H
#include "../../CArray.h"

class SEnemyPak
{
public:
	/******************************************************************************
	Description	:	Stores enemy data
	******************************************************************************/
	struct _tPack
	{
		int nHealth;
		float fSpeed;
		float fMeleeDPS;
		float fMeleeROF;
		float fRangeDPS;
		float fRangeROF;
		char  nEnemyType;

		//	Constructor
		_tPack()	:	fSpeed(0), fMeleeDPS(0), fMeleeROF(0),
			fRangeDPS(0), fRangeROF(0), nHealth(0), nEnemyType(-1)	{}
	};

private:
	CArray<_tPack> m_EnemyPacks;
public:

	/******************************************************************************
	Description	:	Returns a stored enemy pack
	******************************************************************************/
	const _tPack *GetEnemyPack(unsigned int nIndex)	
	{	
		if(nIndex < 0 || nIndex >= m_EnemyPacks.size())
			return 0;
		return &m_EnemyPacks[nIndex];	
	}

	/******************************************************************************
	Description	:	Add an enemy pack
	******************************************************************************/
	void AddEnemyPack(_tPack &pack)	{	m_EnemyPacks.AddObject(pack);	}

	/******************************************************************************
	Description	:	Clear an enemy pack
	******************************************************************************/
	void ClearEnemyPack(void)		{	m_EnemyPacks.clear();	}
};

#endif