/**************************************************************************************************
Name:			Bullet.h
Description:	This class represents the bullets that will be used throughout the entire game.
**************************************************************************************************/
#ifndef BULLET_H_
#define BULLET_H_

#include <d3dx9.h>
#include "../GameObject.h"
#include "../Enemies/Enemy.h"

enum BULLET_TYPES { EC_REGULAR, EC_CHARGED, MISSILE, HEAD_TURRET, ENEMY_BULLET, ENEMY_MISSILE, MINE, THREE_BARREL, CC_CHARGED = -2 };

class CBullet : public CGameObject
{
protected:

	float m_fMaxLifeTime;
	float m_fLifeTime;
	int m_nBulletType;
	int m_nDamage;

	/********************************************************************************
	Description:	Resets this bullet to defualt parameters.
	********************************************************************************/
	virtual void Reset();

public:

	CBullet();
	~CBullet(void);

	/********************************************************************************
	Description:	Accessors and Mutators
	********************************************************************************/
	void SetLifeTime(float fLifeTime)	{ m_fLifeTime = fLifeTime;		}
	void SetBulletType(int nBulletType) { m_nBulletType = nBulletType;	}
	void SetDamage(int nDamage)			{ m_nDamage = nDamage;			}

	float GetLifeTime()					{ return m_fLifeTime;			}
	int GetBulletType()					{ return m_nBulletType;			}
	int GetDamage()						{ return m_nDamage;				}

	/********************************************************************************
	Description:	Updates the bullets' position based on the time passed from 
					the last update, returns true if the bullet is alive.
	********************************************************************************/
	virtual bool Update(float fDeltaTime);
	
	/********************************************************************************
	Description:	Renders the bullet.
	********************************************************************************/
	virtual void Render();

	/********************************************************************************
	Description:	Checks for collisions against game objects.
	********************************************************************************/
	virtual bool CheckCollisions(CGameObject* pObject);

	/********************************************************************************
	Description:	Handles collision reactions.
	********************************************************************************/
	virtual void HandleCollision(CGameObject* pObject);
};

#endif