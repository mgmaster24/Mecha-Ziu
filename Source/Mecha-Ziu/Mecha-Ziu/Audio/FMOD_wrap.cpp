#include "FMOD_wrap.h"
#include "../Camera.h"
#include "../Game.h"
bool CFMOD::Init(int nMaxChannels, FMOD_INITFLAGS uiInitFlags, void* pExtraDriverData)
{
	if(nMaxChannels < 0 || uiInitFlags < 0)
		return false;

	// used to catch FMOD error codes
	FMOD_RESULT fmResult;
	// create the system object
	if((fmResult = FMOD::System_Create(&m_pSystem)) != FMOD_OK)
		FMOD_ERROR(fmResult);
	if((fmResult = m_pSystem->set3DSettings(1.0f,1.0f,1.0f)) != FMOD_OK)
		FMOD_ERROR(fmResult);
	// initialize the system
	if((fmResult = m_pSystem->init(nMaxChannels, uiInitFlags, pExtraDriverData)) != FMOD_OK)
		FMOD_ERROR(fmResult);
	// create the sound groups
	if((fmResult = m_pSystem->createSoundGroup("Tracks",&m_pTrackGroup)) != FMOD_OK)
		FMOD_ERROR(fmResult);
	if((fmResult = m_pSystem->createSoundGroup("SoundFX", &m_pSFXGroup)) != FMOD_OK)
		FMOD_ERROR(fmResult);
	if((fmResult = m_pSystem->createSoundGroup("Dialogues", &m_pDialogueGroup)) != FMOD_OK)
		FMOD_ERROR(fmResult);

	m_fReleaseTimer = 0.0f;

	return true;
}
bool CFMOD::Shutdown(void)
{
	if(UnloadAll())
	{
		m_pDialogueGroup->release();
		m_pDialogueGroup = NULL;
		m_pSFXGroup->release();
		m_pSFXGroup = NULL;
		m_pTrackGroup->release();
		m_pTrackGroup = NULL;
		
		return true;
	}		
	else 
		return false;
}	
bool CFMOD::Update(float fDelta)
{
	if(m_pSystem == NULL)
		return false;

	// used to catch FMOD error codes
	FMOD_RESULT fmResult;
	
	// set the listener to be the camera
	FMOD_VECTOR vLisPos, vLisForward, vLisUp, vLisVel;
	// the listener's position
	vLisPos.x = CCamera::GetInstance()->GetViewMatrix()._41;
	vLisPos.y = CCamera::GetInstance()->GetViewMatrix()._42; 
	vLisPos.z = -CCamera::GetInstance()->GetViewMatrix()._43;
	// the listener's forward vector
	vLisForward.x = CCamera::GetInstance()->GetViewMatrix()._31;
	vLisForward.y = CCamera::GetInstance()->GetViewMatrix()._32;
	vLisForward.z = CCamera::GetInstance()->GetViewMatrix()._33;
	// the listener's up vector
	vLisUp.x = CCamera::GetInstance()->GetViewMatrix()._21;
	vLisUp.y = CCamera::GetInstance()->GetViewMatrix()._22;
	vLisUp.z = CCamera::GetInstance()->GetViewMatrix()._23;
	// our camera's velocity is based on he player's velocity
	vLisVel.x = CCamera::GetInstance()->GetVelocity().x;
	vLisVel.y = CCamera::GetInstance()->GetVelocity().y;
	vLisVel.z = CCamera::GetInstance()->GetVelocity().z;
	// set the 3d attrubutes for the listener
	if(fabs(vLisVel.x) < 0.00001f)
		vLisVel.x = 0.0f;
	if(fabs(vLisVel.y) < 0.00001f)
		vLisVel.y = 0.0f;
	if(fabs(vLisVel.z) < 0.00001f)
		vLisVel.z = 0.0f;
	if((fmResult = m_pSystem->set3DListenerAttributes(0, &vLisPos, &vLisVel, 
		&vLisForward, &vLisUp)) != FMOD_OK)
		FMOD_ERROR(fmResult);
	
	// update the system
	if((fmResult = m_pSystem->update()) != FMOD_OK)
		FMOD_ERROR(fmResult);
	m_fReleaseTimer += fDelta;
	// release any unused channels
	if(m_fReleaseTimer > FMOD_RELEASE)
	{
		ReleaseChannels();
		m_fReleaseTimer = 0.0f;
	}
	return true;
}
int CFMOD::LoadSound(const char* szFileName, bool bIsStreaming, FMOD_MODE uiMode)
{
	// make sure the system exists
	if(m_pSystem == NULL || szFileName == NULL)
		return -1;

	// string for group qualifier
	char szGroupType[4];
	ZeroMemory(szGroupType, 4);
	// index to be returned and count 
	int nIndex = -1, nSoundCount = 0, nSoundType = -1;
	// used to catch FMOD errors;
	FMOD_RESULT fmResult;
	string szFile = szFileName; 
	size_t Offset = szFile.find_last_of("/");
	string szName = szFile.substr(Offset + 1);

	// vector iterators for sound groups
	vector<tSoundInfo>::iterator trackIter = m_Tracks.begin();
	vector<tSoundInfo>::iterator sfxIter = m_SoundFX.begin();
	vector<tSoundInfo>::iterator dlgIter = m_DialogueFX.begin();

	// copy the group type
	for(int i = 0; i < 3; i++)
	{
		if(szName.c_str()[i] != '\0')
			szGroupType[i] = szName.c_str()[i];
	}
	szGroupType[3] = '\0';
	//strcpy_s(szGroupType, 3, szName.c_str());

	// define the type of sound this is
	if(_stricmp(szGroupType, "TRK") == 0)
		nSoundType = TRACK;
	else if(_stricmp(szGroupType, "SFX") == 0)
		nSoundType = SOUNDEFFECT;
	else if(_stricmp(szGroupType, "DLG") == 0)
		nSoundType = DIALOGUE;
	// checks for if sounds exist in the list if it does we will not create it again
	string szFName;
	nIndex = -1;
	switch(nSoundType)
	{
	case TRACK:
		{
			// run through the loaded tracks
			while(trackIter != m_Tracks.end())
			{
				szFile = (*trackIter).szFileName;
				Offset = szFile.find_last_of("/");
				szFName = szFile.substr(Offset + 1);
				// if these are equal the sound is already loaded
				if(_stricmp(szFName.c_str(), szName.c_str()) == 0)
				{
					(*trackIter).uiRefCount++;
					nIndex = nSoundCount;
					break;
				}
				trackIter++;
				nSoundCount++;
			}
			if(nIndex != -1) 
				return nIndex;
		}
		break;
	case SOUNDEFFECT:
		{
			
			// run through the loaded soundFX
			while(sfxIter != m_SoundFX.end())
			{
				szFile = (*sfxIter).szFileName;
				Offset = szFile.find_last_of("/");
				szFName = szFile.substr(Offset + 1);
				// if these are equal the sound is already loaded
				if(_stricmp(szFName.c_str(), szName.c_str()) == 0)
				{
					(*sfxIter).uiRefCount++;
					nIndex = nSoundCount;
					break;
				}
				sfxIter++;
				nSoundCount++;
			}
			if(nIndex != -1) 
				return nIndex;
		}
		break;
	case DIALOGUE:
		{
			
			// run through the loaded dialoques
			while(dlgIter != m_DialogueFX.end())
			{
				szFile = (*dlgIter).szFileName;
				Offset = szFile.find_last_of("/");
				szFName = szFile.substr(Offset + 1);
				// if these are equal the sound is already loaded
				if(_stricmp(szFName.c_str(), szName.c_str()) == 0)
				{
					(*dlgIter).uiRefCount++;
					nIndex = nSoundCount;
					break;
				}
				dlgIter++;
				nSoundCount++;
			}
			if(nIndex != -1) 
				return nIndex;
		}
	};
	// sound has not been loaded before, a new sound will 
	// be made from this file
	if(nIndex == -1)
	{
		// the new sound to be loaded
		tSoundInfo newSound;
		// the file name of the sound
		strcpy_s(newSound.szFileName, FMOD_MAX_PATH, szFileName);
		// the type of sound this is
		newSound.uiType = nSoundType;
		// the reference count
		newSound.uiRefCount = 1;
		// the format of the sound
		newSound.uiSoundFormat |= uiMode;
		// is the sound streaming or not
		// if so create a streaming(loads at run time) sound
		if(bIsStreaming)
			if((fmResult = m_pSystem->createStream(newSound.szFileName, 
				newSound.uiSoundFormat, 0, &newSound.pSound)) != FMOD_OK)
				int i = 0;
				//FMOD_ERROR(fmResult);
		// if not create a default(loads into memory when loaded)sound
		if(!bIsStreaming)
			if((fmResult = m_pSystem->createSound(newSound.szFileName, 
				newSound.uiSoundFormat, 0, &newSound.pSound)) != FMOD_OK)
				int j = 0;	
				//FMOD_ERROR(fmResult);
		if(newSound.uiSoundFormat == FMOD_3D)
			newSound.pSound->set3DMinMaxDistance(2.0f, 10000.0f);

		// set the group and add the sound to the list
		if(newSound.uiType == TRACK)
		{
			newSound.pSound->setSoundGroup(m_pTrackGroup);
			newSound.pGroup = m_pTrackGroup;
			m_Tracks.push_back(newSound);
			return (int)m_Tracks.size() - 1;
		}
		else if(newSound.uiType == SOUNDEFFECT)
		{
			newSound.pSound->setSoundGroup(m_pSFXGroup);
			newSound.pGroup = m_pSFXGroup;
			m_SoundFX.push_back(newSound);
			return (int)m_SoundFX.size() - 1;
		}
		else if(newSound.uiType == DIALOGUE)
		{
			newSound.pSound->setSoundGroup(m_pDialogueGroup);
			newSound.pGroup = m_pDialogueGroup;
			m_DialogueFX.push_back(newSound);
			return (int)m_DialogueFX.size() - 1;
		}	
	}
	// will be negative one here, all other methods have failed
	return nIndex;
}
bool CFMOD::UnloadSound(int nSoundID, unsigned uiSoundType)
{
	if(m_pSystem == NULL)
		return false;
	
	FMOD_RESULT fmResult;
	bool bIsReleased = false;
	list<FMOD::Channel*>::iterator lIter;
	switch(uiSoundType)
	{
	case TRACK:
		{
			if(m_Tracks[nSoundID].uiRefCount == 0 || 
				(fmResult = m_Tracks[nSoundID].pSound->release()) == FMOD_OK)
			{
				ZeroMemory(m_Tracks[nSoundID].szFileName, FMOD_MAX_PATH);
				m_Tracks[nSoundID].pSound = NULL;
				m_Tracks[nSoundID].uiRefCount = 0;
				m_Tracks[nSoundID].pGroup = NULL;
				m_Tracks[nSoundID].uiSoundFormat = 0;
				m_Tracks[nSoundID].uiType = 0;
				lIter = m_Tracks[nSoundID].pChannels.begin();
				while(lIter != m_Tracks[nSoundID].pChannels.end())
					lIter = m_Tracks[nSoundID].pChannels.erase(lIter);
				m_Tracks[nSoundID].pChannels.clear();
			}
		}
		break;
	case SOUNDEFFECT:
		{
			if(m_SoundFX[nSoundID].uiRefCount == 0 || 
				(fmResult = m_SoundFX[nSoundID].pSound->release()) == FMOD_OK)
			{
				ZeroMemory(m_SoundFX[nSoundID].szFileName, FMOD_MAX_PATH);
				m_SoundFX[nSoundID].pSound = NULL;
				m_SoundFX[nSoundID].uiRefCount = 0;
				m_SoundFX[nSoundID].pGroup = NULL;
				m_SoundFX[nSoundID].uiSoundFormat = 0;
				m_SoundFX[nSoundID].uiType = 0;
				lIter = m_SoundFX[nSoundID].pChannels.begin();
				while(lIter != m_SoundFX[nSoundID].pChannels.end())
					lIter = m_SoundFX[nSoundID].pChannels.erase(lIter);
				m_SoundFX[nSoundID].pChannels.clear();
			}
		}
		break;
	case DIALOGUE:
		{
			if(m_DialogueFX[nSoundID].uiRefCount == 0 || 
				(fmResult = m_DialogueFX[nSoundID].pSound->release()) == FMOD_OK)
			{
				ZeroMemory(m_DialogueFX[nSoundID].szFileName, FMOD_MAX_PATH);
				m_DialogueFX[nSoundID].pSound = NULL;
				m_DialogueFX[nSoundID].uiRefCount = 0;
				m_DialogueFX[nSoundID].pGroup = NULL;
				m_DialogueFX[nSoundID].uiSoundFormat = 0;
				m_DialogueFX[nSoundID].uiType = 0;
				lIter = m_DialogueFX[nSoundID].pChannels.begin();
				while(lIter != m_DialogueFX[nSoundID].pChannels.end())
					lIter = m_DialogueFX[nSoundID].pChannels.erase(lIter);
				m_DialogueFX[nSoundID].pChannels.clear();
			}
		}
		break;
	};
	return true;
}
bool CFMOD::UnloadAll(void)
{
	unsigned uiSoundIndex = 0;
	// un load all tracks
	for(uiSoundIndex = 0; uiSoundIndex < m_Tracks.size(); ++uiSoundIndex)
		if(!UnloadSound(uiSoundIndex, TRACK))
			return false;
	m_Tracks.clear();
	// reset the index and unload the sound effects
	for(uiSoundIndex = 0; uiSoundIndex < m_SoundFX.size(); ++uiSoundIndex)
		if(!UnloadSound(uiSoundIndex, SOUNDEFFECT))
			return false;
	m_SoundFX.clear();
	// reset the index and unload the sound effects
	for(uiSoundIndex = 0; uiSoundIndex < m_DialogueFX.size(); ++uiSoundIndex)
		if(!UnloadSound(uiSoundIndex, DIALOGUE))
			return false;
	m_DialogueFX.clear();

	return true;
	
}
bool CFMOD::SetVolume(unsigned uiSoundType, float fVolume)
{
	if(m_pSystem == NULL)
		return false;

	switch(uiSoundType)
	{
	case TRACK:
		{
			m_pTrackGroup->setVolume(fVolume);
		}
		break;
	case SOUNDEFFECT:
		{
			m_pSFXGroup->setVolume(fVolume);
		}
		break;
	case DIALOGUE:
		{
			m_pDialogueGroup->setVolume(fVolume);
		}
		break;
	}
	return true;
}
	
bool CFMOD::SetPan(unsigned uiSoundType, int nSoundID, float fPan)
{
	if(m_pSystem == NULL)
		return false;

	list<FMOD::Channel*>::iterator lIter;
	FMOD_RESULT fmResult;
	bool bIsPlaying = false;
	switch(uiSoundType)
	{
	case TRACK:
		{
			lIter = m_Tracks[nSoundID].pChannels.begin();
			while(lIter != m_Tracks[nSoundID].pChannels.end())
			{
				if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
					(*lIter)->setPan(fPan);
				lIter++;
			}
		}
		break;
	case SOUNDEFFECT:
		{
			lIter = m_SoundFX[nSoundID].pChannels.begin();
			while(lIter != m_SoundFX[nSoundID].pChannels.end())
			{
				if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
					(*lIter)->setPan(fPan);
				lIter++;
			}
		}
		break;
	case DIALOGUE:
		{
			lIter = m_DialogueFX[nSoundID].pChannels.begin();
			while(lIter != m_DialogueFX[nSoundID].pChannels.end())
			{
				if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
					(*lIter)->setPan(fPan);
				lIter++;
			}
		}
		break;
	};
	return true;

}	
bool CFMOD::SetFrequency(unsigned uiSoundType, int nSoundID, float fFrequency)
{
	if(m_pSystem == NULL)
		return false;

	list<FMOD::Channel*>::iterator lIter;
	FMOD_RESULT fmResult;
	bool bIsPlaying = false;
	switch(uiSoundType)
	{
	case TRACK:
		{
			lIter = m_Tracks[nSoundID].pChannels.begin();
			while(lIter != m_Tracks[nSoundID].pChannels.end())
			{
				if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
				{
					float fCurrFreq = 0.0f;
					(*lIter)->getFrequency(&fCurrFreq);
					fFrequency += fCurrFreq;
					(*lIter)->setFrequency(fFrequency);
				}
				lIter++;
			}
		}
		break;
	case SOUNDEFFECT:
		{
			lIter = m_SoundFX[nSoundID].pChannels.begin();
			while(lIter != m_SoundFX[nSoundID].pChannels.end())
			{
				if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
				{
					float fCurrFreq = 0.0f;
					(*lIter)->getFrequency(&fCurrFreq);
					fFrequency += fCurrFreq;
					(*lIter)->setFrequency(fFrequency);
				}
				lIter++;
			}
		}
		break;
	case DIALOGUE:
		{
			lIter = m_DialogueFX[nSoundID].pChannels.begin();
			while(lIter != m_DialogueFX[nSoundID].pChannels.end())
			{
				if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
				{
					float fCurrFreq = 0.0f;
					(*lIter)->getFrequency(&fCurrFreq);
					fFrequency += fCurrFreq;
					(*lIter)->setFrequency(fFrequency);
				}
				lIter++;
			}
		}
		break;
	};
	return true;

}

bool CFMOD::CheckTrackPosition(int nSoundID, unsigned int unPosition)
{
	if(m_pSystem == NULL || !(m_Tracks[nSoundID].pChannels.size()))
		return false;

	unsigned int unCurrentPos; 
	list<FMOD::Channel*>::iterator lIter = m_Tracks[nSoundID].pChannels.begin();
	
	if((*lIter)->getPosition(&unCurrentPos, FMOD_TIMEUNIT_MS) != FMOD_OK)
		return false;

	if(unCurrentPos > unPosition)
		return true;

	return false;
}

unsigned int CFMOD::GetTrackLength(int nSoundID, unsigned uiSoundType)
{
	if(m_pSystem == NULL)
		return false;

	FMOD_RESULT fmResult;
	unsigned int unLength;
	switch(uiSoundType)
	{
	case TRACK:
		{
			if((fmResult = m_Tracks[nSoundID].pSound->getLength(&unLength, FMOD_TIMEUNIT_MS)) != FMOD_OK)
				return 0;
		}
		break;
	case SOUNDEFFECT:
		{
			if((fmResult = m_SoundFX[nSoundID].pSound->getLength(&unLength, FMOD_TIMEUNIT_MS)) != FMOD_OK)
				return 0;
		}
		break;
	case DIALOGUE:
		{
			if((fmResult = m_DialogueFX[nSoundID].pSound->getLength(&unLength, FMOD_TIMEUNIT_MS)) != FMOD_OK)
				return 0;
		}
		break;
	}
	return unLength;
}

bool CFMOD::IsSoundPlaying(int nSoundID, unsigned uiSoundType)
{
	if(m_pSystem == NULL)
		return false;

	list<FMOD::Channel*>::iterator lIter;
	FMOD_RESULT fmResult;
	bool bIsPlaying = false;
	switch(uiSoundType)
	{
	case TRACK:
		{
			lIter = m_Tracks[nSoundID].pChannels.begin();
			while(lIter != m_Tracks[nSoundID].pChannels.end())
			{
				if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
					break;
				lIter++;
			}
		}
		break;
	case SOUNDEFFECT:
		{
			lIter = m_SoundFX[nSoundID].pChannels.begin();
			while(lIter != m_SoundFX[nSoundID].pChannels.end())
			{
				if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
					break;
				lIter++;
			}
		}
		break;
	case DIALOGUE:
		{
			lIter = m_DialogueFX[nSoundID].pChannels.begin();
			while(lIter != m_DialogueFX[nSoundID].pChannels.end())
			{
				if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
					break;
				lIter++;
			}
		}
		break;
	};
	return bIsPlaying;
}
void CFMOD::Play(int nSoundID, unsigned uiSoundType)
{
	if(nSoundID == -1)
		return;
	switch(uiSoundType)
	{
	case TRACK:
		{
			PlayTrack(nSoundID);
		}
		break;
	case SOUNDEFFECT:
		{
			PlaySoundEffect(nSoundID);
		}
		break;
	case DIALOGUE:
		{
			PlayDialogue(nSoundID);
		}
		break;
	};
}
void CFMOD::Play3D(int nSoundID, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vVel)
{
	// there is no sound type here.  Sound effects should be the only thing to be played in 3D
	if(m_SoundFX[nSoundID].uiSoundFormat != FMOD_3D)
		return;

	FMOD::Channel* pChannel;
	FMOD_RESULT	fmResult;
	FMOD_VECTOR fvPos, fvVel;
	// convert the necessary D3DXVECTORs to FMOD_VECTORs
	fvPos.x = vPos.x;	fvVel.x = vVel.x;
	fvPos.y = vPos.y;	fvVel.y = vVel.y;
	fvPos.z = vPos.z;	fvVel.z = vVel.z;


	if((fmResult = m_pSystem->playSound(FMOD_CHANNEL_FREE, 
		m_SoundFX[nSoundID].pSound, false, &pChannel)) != FMOD_OK)
		FMOD_ERROR(fmResult);

	if((fmResult = pChannel->set3DAttributes(&fvPos, &fvVel)) != FMOD_OK)
		FMOD_ERROR(fmResult);
	m_SoundFX[nSoundID].pChannels.push_back(pChannel);
}

void CFMOD::PlayTrack(int nTrackID)
{
	FMOD::Channel* pChannel;
	FMOD_RESULT	fmResult;

	if((fmResult = m_pSystem->playSound(FMOD_CHANNEL_FREE, 
		m_Tracks[nTrackID].pSound, false, &pChannel)) != FMOD_OK)
		FMOD_ERROR(fmResult);
	m_Tracks[nTrackID].pChannels.push_back(pChannel);

}
void CFMOD::PlaySoundEffect(int nEffectID)
{
	FMOD::Channel* pChannel;
	FMOD_RESULT	fmResult;

	if((fmResult = m_pSystem->playSound(FMOD_CHANNEL_FREE, 
		m_SoundFX[nEffectID].pSound, false, &pChannel)) != FMOD_OK)
		FMOD_ERROR(fmResult);
	//if(m_SoundFX[nEffectID].uiSoundFormat == (unsigned)FMOD_3D);
	m_SoundFX[nEffectID].pChannels.push_back(pChannel);
}	
void CFMOD::PlayDialogue(int nDialogueID)
{
	FMOD::Channel* pChannel;
	FMOD_RESULT	fmResult;

	if((fmResult = m_pSystem->playSound(FMOD_CHANNEL_FREE, 
		m_DialogueFX[nDialogueID].pSound, false, &pChannel)) != FMOD_OK)
		FMOD_ERROR(fmResult);

	m_DialogueFX[nDialogueID].pChannels.push_back(pChannel);
}
void CFMOD::StopSound(int nSoundID, unsigned uiSoundType)
{
	if(nSoundID == -1)
		return;
	switch(uiSoundType)
	{
	case TRACK:
		{
			StopTrack(nSoundID);
		}
		break;
	case SOUNDEFFECT:
		{
			StopSoundEffect(nSoundID);
		}
		break;
	case DIALOGUE:
		{
			StopDialogue(nSoundID);
		}
		break;
	};
}
void CFMOD::StopTrack(int nTrackID)
{
	FMOD_RESULT	fmResult;
	bool bIsPlaying = false;
	list<FMOD::Channel*>::iterator lIter = m_Tracks[nTrackID].pChannels.begin();
	while(lIter != m_Tracks[nTrackID].pChannels.end())
	{
		if(( fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
			(*lIter)->stop();
		lIter++;
	}
}
void CFMOD::StopSoundEffect(int nEffectID)
{
	FMOD_RESULT	fmResult;
	bool bIsPlaying = false;
	list<FMOD::Channel*>::iterator lIter = m_SoundFX[nEffectID].pChannels.begin();
	while(lIter != m_SoundFX[nEffectID].pChannels.end())
	{
		if(( fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
			(*lIter)->stop();
		lIter++;
	}
}
void CFMOD::StopDialogue(int nDialogueID)
{
	FMOD_RESULT	fmResult;
	bool bIsPlaying = false;
	list<FMOD::Channel*>::iterator lIter = m_DialogueFX[nDialogueID].pChannels.begin();
	while(lIter != m_DialogueFX[nDialogueID].pChannels.end())
	{
		if(( fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
			(*lIter)->stop();
		lIter++;
	}
}
void CFMOD::ResetSound(int nSoundID, unsigned uiSoundType)
{
	if(nSoundID == -1)
		return;
	switch(uiSoundType)
	{
	case TRACK:
		{
			ResetTrack(nSoundID);
		}
		break;
	case SOUNDEFFECT:
		{
			ResetSoundEffect(nSoundID);
		}
		break;
	case DIALOGUE:
		{
			ResetDialogue(nSoundID);
		}
		break;
	};
}
void CFMOD::ResetTrack(int nTrackID)
{
	FMOD_RESULT fmResult;
	bool bIsPlaying = false;

	list<FMOD::Channel*>::iterator lIter = m_Tracks[nTrackID].pChannels.begin();

	while(lIter != m_Tracks[nTrackID].pChannels.end())
	{
		if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
			(*lIter)->setPosition(0, FMOD_TIMEUNIT_MS);
		lIter++;
	}

}
void CFMOD::ResetSoundEffect(int nEffectID)
{
	FMOD_RESULT fmResult;
	bool bIsPlaying = false;

	list<FMOD::Channel*>::iterator lIter = m_SoundFX[nEffectID].pChannels.begin();

	while(lIter != m_SoundFX[nEffectID].pChannels.end())
	{
		if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
			(*lIter)->setPosition(0, FMOD_TIMEUNIT_MS);
		lIter++;
	}
}
void CFMOD::ResetDialogue(int nDialogueID)
{
	FMOD_RESULT fmResult;
	bool bIsPlaying = false;

	list<FMOD::Channel*>::iterator lIter = m_DialogueFX[nDialogueID].pChannels.begin();

	while(lIter != m_DialogueFX[nDialogueID].pChannels.end())
	{
		if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_OK)
			(*lIter)->setPosition(0, FMOD_TIMEUNIT_MS);
		lIter++;
	}
}		
void CFMOD::ReleaseChannels(void)
{
	// used to catch FMOD errors
	FMOD_RESULT  fmResult;
	// filled out by channel isPlaying call
	bool bIsPlaying = false;
	list<FMOD::Channel*>::iterator lIter;
	vector<tSoundInfo>::iterator vIter = m_Tracks.begin();
	while(vIter != m_Tracks.end())
	{
		lIter = (*vIter).pChannels.begin();
		while(lIter != (*vIter).pChannels.end())
		{
			if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_ERR_INVALID_HANDLE)
				lIter = (*vIter).pChannels.erase(lIter);
			else
				++lIter;
		}
		++vIter;
	}
	vIter = m_SoundFX.begin();
	while(vIter != m_SoundFX.end())
	{
		lIter = (*vIter).pChannels.begin();
		while(lIter != (*vIter).pChannels.end())
		{
			if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_ERR_INVALID_HANDLE)
				lIter = (*vIter).pChannels.erase(lIter);
			else
				++lIter;
		}
		++vIter;
	}
	vIter = m_DialogueFX.begin();
	while(vIter != m_DialogueFX.end())
	{
		lIter = (*vIter).pChannels.begin();
		while(lIter != (*vIter).pChannels.end())
		{
			if((fmResult = (*lIter)->isPlaying(&bIsPlaying)) == FMOD_ERR_INVALID_HANDLE)
				lIter = (*vIter).pChannels.erase(lIter);
			else
				++lIter;
		}
		++vIter;
	}
}