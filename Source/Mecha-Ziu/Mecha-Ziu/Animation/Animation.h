#ifndef _C_ANIM_H
#define _C_ANIM_H

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <vector>
#include "../Constants.h"
using std::vector;

/*******************************************************************
*	Name: _tQuatForm
*
*	Description:  Quaternion based transform 				 
*******************************************************************/
typedef struct _tQuatForm
{
	D3DXVECTOR3		vecPos;
	D3DXVECTOR3		vecScale;
	D3DXQUATERNION	quatRot;
}QuatForm;
/********************************************************************
*	Name: _tTransNode
*
*	Description: Node representing the transform of a mesh and the 
*				 index of its parent transform.
********************************************************************/
typedef struct _tTransNode
{
	char			szName[32];
	// unsigned		uiParentIndex;
	//char			szParentName[32];
	//D3DXMATRIX		maToRoot;
	QuatForm		quatTransform;
	
	_tTransNode(void){}
}TransformNode;
/**********************************************************************
*	Name: _tKeyFrame
*
*	Description: A single key frame of the animation. Has a 
*				 collection of transforms nodes to represent 
*				 the hierarchy of transforms within the model
**********************************************************************/
typedef struct _tKeyFrame
{
	unsigned int			nNumTransNodes;
	float					fKeyTime;
	float					fTweenTime;
	TransformNode*			pTransNodes;
	_tKeyFrame& operator =(const _tKeyFrame& ref)
	{
		if(&ref != this)
		{
			nNumTransNodes = ref.nNumTransNodes;
			fKeyTime = ref.fKeyTime;
			fTweenTime = ref.fTweenTime;
		}
		return *this;
	}
	
	_tKeyFrame(void) : nNumTransNodes(0), fKeyTime(0.0f), fTweenTime(0.0f), pTransNodes(NULL){}
	~_tKeyFrame(void){ delete [] pTransNodes; }
}KeyFrame;
/**********************************************************************
*	Name: tAnimation
*
*	Description: struct that handles all in game animations
**********************************************************************/
typedef struct _tAnimation
{
	// Key frames may need to be stores as and array of key frame pointers
	unsigned			nNumFrames;
	float				fAnimDuration;
	KeyFrame*			pKeyFrames;	

	_tAnimation(void) : nNumFrames(0), fAnimDuration(0.0f), pKeyFrames(NULL){}
	~_tAnimation(void) { delete [] pKeyFrames; }
}Animation;

/**********************************************************************
*	Name: CInterpolator
*
*	Description: Class that handles the interpolation of transforms
*				 betwenn key frames.
**********************************************************************/
class CInterpolator
{	
public:
	enum PRIORITY { IMMEDIATE = 0, HIGH, NORMAL, LOW, SKIP };
private:
	
	// the frames and data for the animation
	typedef struct _tAnimData
	{
		bool				bLooping;
		bool				bTrigger;
		bool				bBlendOut;
		unsigned int		unPriority;
		unsigned int		unID;
		vector<KeyFrame*>	keyframes;
	}tAnimData;
	
	// values of animations currently playing
	typedef struct _tPlayingAnim
	{
		unsigned int	unIndex;
		float			fAnimTime;
		KeyFrame		currFrame;
		float			fBlendWeight;
		bool			bActive;
		CInterpolator*	pInterp;

		_tPlayingAnim(); 

		void Update(float fDeltaTime);

		/**********************************************************************
		*	Description: Interpolates collection of transforms
		**********************************************************************/
		void Interpolate(unsigned uiCurrIndex, unsigned uiNextIndex, float fLamda);

	}tPlayingAnim;

	// state the interpolator is in
	const Animation*		m_pAnimation;
	D3DXMATRIX*				m_pmaTransforms;
	// all the animations that this object contains
	vector<tAnimData>		m_Animations;
	tPlayingAnim			m_PlayingAnims[NUM_PLAYABLE_ANIMS];
	// the nodes to blend
	KeyFrame				m_ResultFrame;
	
	unsigned			m_uiCurr;			// the current animation index
	float				m_fTargetWeight;	// target blend weight
	bool				m_bAnimationAdded;	// check for first playing animation
	unsigned			m_uiNumActive;		// number of animations currently active
	unsigned			m_uiNumTriggers;	// number of animations triggers currently pressed
	bool				m_bSnap;			// Determines whether or not to snap immediatly out of an animation instead of blending

	// function to detect if any new triggers were activated since the last frame and add them
	// the list of playing animations
	void AddNewAnims();

	// checks if an animation is already playing
	bool IsAnimPlaying(unsigned int unIndex);

	// adds an animation to the list of playing animations
	void PlayAnimation(unsigned int unIndex);

public:

	explicit CInterpolator(Animation* m_pAnimation);
	~CInterpolator(void);
	
	// mutators
	void ToggleAnimation(unsigned int unIndex, bool bActive, bool bBlendOut = true);
	void SetCurrIndex(unsigned uiCurr) { m_uiCurr = uiCurr; }
	/**********************************************************************
	*	Description: Adds an animation to the list of available animations
	*				 
	**********************************************************************/
	void AddAnimation(unsigned int unStart, unsigned int unEnd, unsigned int unID, bool bLooping, unsigned int unPriority = 0);

	void AddCutSceneAnim(unsigned int unStart, unsigned int unEnd);
	// accessors
	KeyFrame GetResultFrame(void) { return m_ResultFrame; }
	/**********************************************************************
	*	Description: Converts all transforms for the current
	*				 key frame into world space and returns them.
	**********************************************************************/
	D3DXMATRIX*	GetTransforms(void);
	/**********************************************************************
	*	Description: Gets the transform by name.
	**********************************************************************/
	D3DXMATRIX* GetTransformByName(const char* szName);
	/**********************************************************************
	*	Description: Updates 
	*				 
	**********************************************************************/
	void Update(float fElapsed);

	/**********************************************************************
	*	Description: Blends all playing animations into one frame
	*				 for drawing
	**********************************************************************/
	void Blend();
};
#endif