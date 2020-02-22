/*********************************************************************************
Name:			Scene Object
Description:	Class that governs animated objects in cut scene.
				Also has control of particle systen positioning.
*********************************************************************************/

#ifndef SCENE_OBJ_H_
#define SCENE_OBJ_H_

#include "Animation.h"
#include "DirectInput.h"
#include "ParticleSystem.h"
#include "Sphere.h"
enum OBJECT_TYPE { PLAYER, REG_ENEMY, WAR_TURTLE, TALBOT };
enum ANIM_TYPE { FORWARD_ANIM, BACKWARD_ANIM, LEFT_ANIM, RIGHT_ANIM };
typedef struct _tAnimTrigger
{
	bool	 bLoop;
	unsigned uiTriggerFrame;
	unsigned uiStartIndex;
	unsigned uiEndIndex;
	unsigned uiIndex;
}tAnimTrigger;
typedef struct _tParticleTrigger
{
	unsigned uiTriggerFrame;
	unsigned uiParticleType;
	D3DXMATRIX maRenderAt;
}tParticleTrigger;
class CSceneObject
{
private:
	// The object in the scene, if the interpolator is null the object is not animated.
	bool						m_bIsSelected;
	bool						m_bHasInterpolator;
	bool						m_bStartInterp;
	OBJECT_TYPE					m_Type;
	unsigned int				m_uiModelIndex;
	CSphere						m_Sphere;
	vector<tAnimTrigger>		m_AnimTriggers;
	vector<tParticleTrigger>	m_ParticleTriggers;
	CInterpolator*				m_pInterpolator;
	
public:
	D3DXMATRIX					m_maWorld;
	
	CSceneObject(void);
	virtual ~CSceneObject(void);

	// accessors
	void SetModelIndex(unsigned uiIndex) { m_uiModelIndex = uiIndex; }
	void SetWorld(D3DXMATRIX& maWorld) { m_maWorld = maWorld; }
	void SetSphere(CSphere& sphere) { m_Sphere = sphere; }
	void SetType(OBJECT_TYPE type) { m_Type = type; }
	bool IsSelected(void) { return m_bIsSelected; }
	bool HasInterpolator(void)  { return m_bHasInterpolator; }
	bool Select(void) { m_bIsSelected = !m_bIsSelected; return m_bIsSelected;}
	void CreateInterpolator(unsigned uiModelIndex);
	void AddAnimTrigger(tAnimTrigger& trig) { m_AnimTriggers.push_back(trig); }
	void AddParticleTrigger(tParticleTrigger& trig) { m_ParticleTriggers.push_back(trig); }
	// mutators
	unsigned GetModelIndex(void) { return m_uiModelIndex; }
	D3DXMATRIX GetWorldMatrix(void) { return m_maWorld; }
	CSphere	GetSphere(void) { return m_Sphere; }
	OBJECT_TYPE GetType(void) { return m_Type; }
	CInterpolator* GetInterpolator(void) { return m_pInterpolator; }
	vector<tAnimTrigger>& GetAnimTriggers(void) { return m_AnimTriggers; }
	/********************************************************************************
	Description:	Updates the position, orientation, animation and whatever other 
					information is necessary to properly display the object.
	********************************************************************************/
	bool Update(float fDeltaTime); 

	/********************************************************************************
	Description:	Renders the object.
	********************************************************************************/
	void Render(); 

	void CheckMovementInput(float fDeltaTime);
};

#endif 