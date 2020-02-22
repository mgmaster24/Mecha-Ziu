#include "Animation.h"

#if _DEBUG
#include "renderengine.h"
#endif

CInterpolator::_tPlayingAnim::_tPlayingAnim() : bActive(false), unIndex(UINT_MAX), fAnimTime(0.0f) , fBlendWeight(0.0f), pInterp(NULL)
{
}

CInterpolator::CInterpolator(Animation* animation) : m_fTargetWeight(1.0f), m_bAnimationAdded(false), m_uiNumActive(0), m_uiNumTriggers(0)
{
	m_pAnimation = animation; 
	m_ResultFrame = m_pAnimation->pKeyFrames[0]; 
	m_pmaTransforms = new D3DXMATRIX[m_ResultFrame.nNumTransNodes];
	m_ResultFrame.pTransNodes = new TransformNode[m_ResultFrame.nNumTransNodes];

	for(unsigned int i = 0; i < m_ResultFrame.nNumTransNodes; ++i)
		m_ResultFrame.pTransNodes[i] = m_pAnimation->pKeyFrames[0].pTransNodes[i]; 

	for(unsigned int i = 0; i < NUM_PLAYABLE_ANIMS; ++i)
	{
		m_PlayingAnims[i].pInterp = this;
		m_PlayingAnims[i].currFrame.pTransNodes = new TransformNode[m_ResultFrame.nNumTransNodes];
	}
}

void CInterpolator::AddAnimation(unsigned int unStart, unsigned int unEnd, unsigned int unID, bool bLooping)
{
	for(unsigned int i = 0; i < m_Animations.size(); ++i)
		if(m_Animations[i].unID == unID)
			return;

	tAnimData animation; 
	animation.bLooping = bLooping; 
	animation.bTrigger = false;
	animation.unID = unID;

	for(unsigned int i = unStart; i < unEnd; ++i)
		animation.keyframes.push_back(&(m_pAnimation->pKeyFrames[i]));

	m_Animations.push_back(animation); 
}

void CInterpolator::ToggleAnimation(unsigned uiIndex, bool bActive)
{ 
	m_Animations[uiIndex].bTrigger = bActive;
}

CInterpolator::~CInterpolator(void)
{
	if(m_pmaTransforms != NULL)
		delete [] m_pmaTransforms;
}
D3DXMATRIX* CInterpolator::GetTransforms(void)
{
	float fTemp[3];
	D3DXMATRIX	 maTrans, maRot, maScale, maResult;

	for(unsigned int i = 0; i < m_ResultFrame.nNumTransNodes; ++i)
	{
		// create rotation matrix based on the quaternion value of the current interpolated key frame
		D3DXMatrixRotationQuaternion(&maRot, &m_ResultFrame.pTransNodes[i].quatTransform.quatRot);
		
		// create translation matrix based on the position value of the current interpolated key frame
		ZeroMemory(&fTemp, sizeof(float)*3); 
		fTemp[0] = m_ResultFrame.pTransNodes[i].quatTransform.vecPos.x;
		fTemp[1] = m_ResultFrame.pTransNodes[i].quatTransform.vecPos.y;
		fTemp[2] = m_ResultFrame.pTransNodes[i].quatTransform.vecPos.z;
		D3DXMatrixTranslation(&maTrans, fTemp[0], fTemp[1], fTemp[2]);

		// create scale matrix based on the scale value of the current interpolated key frame
		ZeroMemory(&fTemp, sizeof(float)*3); 
		fTemp[0] = m_ResultFrame.pTransNodes[i].quatTransform.vecScale.x;
		fTemp[1] = m_ResultFrame.pTransNodes[i].quatTransform.vecScale.y;
		fTemp[2] = m_ResultFrame.pTransNodes[i].quatTransform.vecScale.z;
		D3DXMatrixScaling(&maScale, fTemp[0], fTemp[1], fTemp[2]);
		
		// create the matrix. Matrix equals rotation * translation * scale.	
		// rotation * translation
		D3DXMatrixMultiply(&m_pmaTransforms[i], &maRot, &maTrans); 	
		// result * scale
		D3DXMatrixMultiply(&m_pmaTransforms[i], &m_pmaTransforms[i], &maScale);
	}
	// still might need to set up the hierarchy based on transform name and the parent name
	return m_pmaTransforms;
}
D3DXMATRIX* CInterpolator::GetTransformByName(const char* szName)
{
	for(unsigned j = 0; j < m_pAnimation->pKeyFrames[0].nNumTransNodes; ++j)
	{
		if(_stricmp(m_pAnimation->pKeyFrames[0].pTransNodes[j].szName, szName) == 0)
		{
			return &(m_pmaTransforms[j]);
		}
	}
	return NULL;
}
void CInterpolator::_tPlayingAnim::Update(float fDeltaTime)
{
	// weight step
	static const float fStep = 3.0f;

	// don't do anything unless you're active
	if(!bActive)
		return;

	//update animtime
	fAnimTime += fDeltaTime;

	// TODO: This is actually a little broken, the weights don't always add up to one
	//adjust the weight
	if(pInterp->m_Animations[unIndex].bTrigger)
	{
		if(fBlendWeight > pInterp->m_fTargetWeight)
		{
			fBlendWeight -= fDeltaTime * fStep;
			
			if(fBlendWeight < pInterp->m_fTargetWeight)
				fBlendWeight = pInterp->m_fTargetWeight;
		}
		else if(fBlendWeight < pInterp->m_fTargetWeight)
		{
			fBlendWeight += fDeltaTime * fStep;

			if(fBlendWeight > pInterp->m_fTargetWeight)
				fBlendWeight = pInterp->m_fTargetWeight;
		}
	}
	else
	{
		fBlendWeight -= fDeltaTime * fStep;

		if(fBlendWeight < 0.0f)
		{
			fBlendWeight = 0.0f;
			pInterp->m_uiNumActive--;
			bActive = false;
		}
	}

	// set fTime
	float fTime = fAnimTime + pInterp->m_Animations[unIndex].keyframes[0]->fKeyTime;

	// find the current Index and next Index 
	unsigned		uiCurrIndex = 0, uiNextIndex = 0, uiLastIndex = (unsigned)pInterp->m_Animations[unIndex].keyframes.size() - 1;
	bool			bLooping = pInterp->m_Animations[unIndex].bLooping;

	for(unsigned int i =0; i < pInterp->m_Animations[unIndex].keyframes.size(); ++i)
	{
		if(fTime > pInterp->m_Animations[unIndex].keyframes[i]->fKeyTime)
		{
			uiCurrIndex = i;

			if(uiCurrIndex == uiLastIndex)
				uiNextIndex = 0;
			else
				uiNextIndex = uiCurrIndex + 1;
		}
		
		if(bLooping && fTime > (pInterp->m_Animations[unIndex].keyframes[uiLastIndex]->fKeyTime) + 
			(pInterp->m_Animations[unIndex].keyframes[uiLastIndex]->fTweenTime))
		{
			fAnimTime = 0;
		}
	}

	// you've reached the end of the animation, get out if you're not supposed to loop
	if(!bLooping && uiCurrIndex == uiLastIndex)
		return;

	// get lambda
	float fCurrentFrameTime = pInterp->m_Animations[unIndex].keyframes[uiCurrIndex]->fKeyTime;
	float fNextFrameTime = pInterp->m_Animations[unIndex].keyframes[uiCurrIndex]->fKeyTime + pInterp->m_Animations[unIndex].keyframes[uiCurrIndex]->fTweenTime;
	float fTotalFrameDuration = fNextFrameTime - fCurrentFrameTime;
	float fElapsedFrameTime = fTime - fCurrentFrameTime;
	float fLamda = fElapsedFrameTime / fTotalFrameDuration;

	//Interpolate
	Interpolate(uiCurrIndex, uiNextIndex, fLamda);
}
void CInterpolator::Update(float fElapsed)
{
	AddNewAnims();

	// update target weight
	unsigned int unAnimCounter = 0;
	for(unsigned int i = 0; i < m_Animations.size(); ++i)
		if(m_Animations[i].bTrigger == true)
			unAnimCounter++;
	m_fTargetWeight = 1.0f / unAnimCounter;

	// store the number of triggered anims
	m_uiNumTriggers = unAnimCounter; 

	// update currently playing animations
	for(unsigned int i = 0; i < NUM_PLAYABLE_ANIMS; ++i)
		m_PlayingAnims[i].Update(fElapsed);

	Blend();
}
void CInterpolator::GoToFrame(unsigned uiFrame)
{
	m_ResultFrame = m_pAnimation->pKeyFrames[uiFrame];
	for(unsigned i = 0; i < m_pAnimation->pKeyFrames[uiFrame].nNumTransNodes; ++i)
	{
		m_ResultFrame.pTransNodes[i] = m_pAnimation->pKeyFrames[uiFrame].pTransNodes[i];
	}
}
void CInterpolator::Blend()
{
	// TODO: Make this more efficient
	for(unsigned int i = 0; i < m_ResultFrame.nNumTransNodes; ++i)
	{
		m_ResultFrame.pTransNodes[i].quatTransform.quatRot = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f); 
		m_ResultFrame.pTransNodes[i].quatTransform.vecPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_ResultFrame.pTransNodes[i].quatTransform.vecScale = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	}

#if _DEBUG
	float fDebug = 0.0f; 
	float fDebug2 = 0.0f;
	unsigned int unDebug = 0;
#endif

	for(unsigned int i = 0; i < m_ResultFrame.nNumTransNodes; ++i)
	{
		for(unsigned int j = 0; j < NUM_PLAYABLE_ANIMS; ++j)
		{
			if(m_PlayingAnims[j].bActive)
			{
#if _DEBUG
				unDebug++;
				fDebug += m_PlayingAnims[j].fBlendWeight;
				fDebug2 = m_PlayingAnims[j].fBlendWeight;
#endif 
				m_ResultFrame.pTransNodes[i].quatTransform.quatRot += m_PlayingAnims[j].currFrame.pTransNodes[i].quatTransform.quatRot * m_PlayingAnims[j].fBlendWeight;
				m_ResultFrame.pTransNodes[i].quatTransform.vecPos += m_PlayingAnims[j].currFrame.pTransNodes[i].quatTransform.vecPos * m_PlayingAnims[j].fBlendWeight;
			}
		}		
	}

#if _DEBUG
	char buffer[256]; 
	sprintf_s(buffer, 256, "Total Weights: %f\nLast Weight: %f\nTotalAnims: %i\nTarget Weight: %f", fDebug / 20, fDebug2, unDebug / 20, m_fTargetWeight);
	CRenderEngine::GetInstance()->RenderText(buffer, 10, 100, 0xffffffff); 
#endif
}

void CInterpolator::AddNewAnims()
{
	bool bInsert;
	for(unsigned int i=0; i < m_Animations.size(); ++i)
	{
		bInsert = true;

		// if the trigger is true
		if(m_Animations[i].bTrigger)
		{
			// check if you're already playing
			for(unsigned int j=0; j < NUM_PLAYABLE_ANIMS; ++j)
			{
				if(m_PlayingAnims[j].bActive)
				{
					if(m_Animations[i].unID == m_Animations[ m_PlayingAnims[j].unIndex ].unID)
					{
						bInsert = false;
						break;
					}
				}
			}
			
			// if you're not already playing
			if(bInsert)
			{
				// find an empty spot in the playing array
				for(unsigned int j=0; j < NUM_PLAYABLE_ANIMS; ++j)
				{
					if(m_PlayingAnims[j].bActive == false)
					{
						// start playing
						m_PlayingAnims[j].unIndex = i;
						m_PlayingAnims[j].fAnimTime = 0.0f; 
						m_PlayingAnims[j].bActive = true;
						m_PlayingAnims[j].currFrame = *(m_Animations[i].keyframes[0]);
						m_uiNumActive++;

						if(m_bAnimationAdded)
							m_PlayingAnims[j].fBlendWeight = 0.0f; 
						else
						{
							m_bAnimationAdded = true;
							m_PlayingAnims[j].fBlendWeight = 1.00f;
						}
						break;
					}
				}
			}
		}	
	}
}

void CInterpolator::_tPlayingAnim::Interpolate(unsigned uiCurrIndex, unsigned uiNextIndex, float fLamda)
{
	// temporary quaternion values for interpolation
	D3DXQUATERNION  quatKey1, quatKey2;
	// temporary vector values for interpolation
	D3DXVECTOR3		vecScale1, vecScale2, vecPos1, vecPos2;
	// find the interpolated value between this key frame and the next
	for(unsigned int i = 0; i < currFrame.nNumTransNodes; ++i)
	{
		// quaternion assigment to two consecutive key frames
		quatKey1 = pInterp->m_Animations[unIndex].keyframes[uiCurrIndex]->pTransNodes[i].quatTransform.quatRot;
		quatKey2 = pInterp->m_Animations[unIndex].keyframes[uiNextIndex]->pTransNodes[i].quatTransform.quatRot;

		// scale vector assignment to two consecutive key frames
		vecScale1 = pInterp->m_Animations[unIndex].keyframes[uiCurrIndex]->pTransNodes[i].quatTransform.vecScale;
		vecScale2 = pInterp->m_Animations[unIndex].keyframes[uiNextIndex]->pTransNodes[i].quatTransform.vecScale;

		// position vector assignment to two consecutive key frames
		vecPos1 = pInterp->m_Animations[unIndex].keyframes[uiCurrIndex]->pTransNodes[i].quatTransform.vecPos;
		vecPos2 = pInterp->m_Animations[unIndex].keyframes[uiNextIndex]->pTransNodes[i].quatTransform.vecPos;

		// Spherical interpolation of quaternions between key frames
		D3DXQuaternionSlerp(&(currFrame.pTransNodes[i].quatTransform.quatRot), &quatKey1, &quatKey2, fLamda);
		// Linear interpolation of vector for scale and position between key frames 
		D3DXVec3Lerp(&(currFrame.pTransNodes[i].quatTransform.vecScale), &vecScale1, &vecScale2, fLamda);
		D3DXVec3Lerp(&(currFrame.pTransNodes[i].quatTransform.vecPos), &vecPos1, &vecPos2, fLamda);
	}
}