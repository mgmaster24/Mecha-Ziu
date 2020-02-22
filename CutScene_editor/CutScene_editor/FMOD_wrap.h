/*********************************************************************
*	File Name: FMOD_wrap.h
*
*	Desciption: Wraps up the functionality of the FMOD sound system.
*********************************************************************/
#ifndef _C_FMOD_H_
#define _C_FMOD_H_

// windows
#include <windows.h>
// DX
#include "d3dx9.h"
// needed includes for FMOD API
#include "./fmod.hpp"
// storage medium includes
#include <vector>
using std::vector;
#include <list>
using std::list;
#include <string>
using std::string;


// FMOD wrapper global constants ///////////////////////////////////////////////////
// Max pathname for the sound
const unsigned FMOD_MAX_PATH = 128;
// Flag for looping a sound
const unsigned FMOD_LOOP = (FMOD_LOOP_NORMAL | FMOD_DEFAULT);
// time to release unused channels
const float FMOD_RELEASE = 3.0;
// macro for generating error codes;
#define FMOD_ERROR(errorcode) { const char* szError = "CFMOD Error: ";\
								char		szBuff[128];\
								sprintf_s(szBuff, 128, szError);\
								sprintf_s(szBuff + sizeof(szError), 128 - sizeof(szError), "%i",(unsigned int)errorcode);\
							  }
////////////////////////////////////////////////////////////////////////////////////

class CFMOD
{
	typedef FMOD::SoundGroup*	GroupInst;
	typedef FMOD::Sound*		SoundInst;
	
	typedef struct _tSoundInformation
	{
		// File name for this song, szGroupType will be part of file name
		char					szFileName[FMOD_MAX_PATH];
		// The number of time the sound has been loaded, should only be 1.
		unsigned				uiRefCount;
		// The type of sound this is.
		unsigned				uiType;
		// The format of this sound
		FMOD_MODE				uiSoundFormat;
		// The pointer to the FMOD sound object
		SoundInst				pSound;
		// The pointer to the group instance this sound belongs too
		GroupInst				pGroup;
		// list of channel used for this sound
		list<FMOD::Channel*>	pChannels;
		// cTor
		_tSoundInformation(void)
		{
			szFileName[0] = '\0';
			uiRefCount = 0;
			uiType = 0;
			uiSoundFormat = 0;
			pSound = NULL;
			pGroup = NULL;
			pChannels.clear();
		}
		// dTor
		~_tSoundInformation(void)
		{
			szFileName[0] = '\0';
			uiRefCount = 0;
			uiType = 0;
			uiSoundFormat = 0;
			pSound = NULL;
			pGroup = NULL;
		}
	}tSoundInfo;
	
	// the single pointer to the FMOD sound system
	FMOD::System*		m_pSystem;
	// FMOD sound group intstance for all background music tracks
	GroupInst			m_pTrackGroup;
	// FMOD sound group instance for all sound effects
	GroupInst			m_pSFXGroup;
	// FMOD sound group instance for all dialogue tracks
	GroupInst			m_pDialogueGroup;
	// vector of music tracks 
	vector<tSoundInfo>	m_Tracks;
	// vector of sound effects;
	vector<tSoundInfo>	m_SoundFX;
	// vector of dialogues
	vector<tSoundInfo>	m_DialogueFX;
	// the timer for releasing unused channels
	float				m_fReleaseTimer; 

	CFMOD(void) : m_pSystem(NULL), m_pTrackGroup(NULL), m_pSFXGroup(NULL), m_pDialogueGroup(NULL){}
	CFMOD(const CFMOD& ref){}
	CFMOD& operator =(const CFMOD& ref){}
	~CFMOD(void){ m_pSystem = NULL; }
	
	/*********************************************************************
	*	Name: UnLoadAll
	*
	*	Description: UnLoads All sounds
	**********************************************************************/
	bool UnloadAll(void);
	/*********************************************************************
	*	Name: PlayTrack
	*	
	*	Desciption: Plays the designated track
	*********************************************************************/
	void PlayTrack(int nTrackID);
	/*********************************************************************
	*	Name: PlaySoundEffect
	*	
	*	Desciption: Plays the designated sound effect
	*********************************************************************/
	void PlaySoundEffect(int nEffectID);
	/*********************************************************************
	*	Name: PlaySoundEffect3D
	*	
	*	Desciption: Plays the designated sound effect
	*********************************************************************/
	void PlaySoundEffect3D(int nEffectID);
	/*********************************************************************
	*	Name: PlayDialogue
	*	
	*	Desciption: Play the designated dialogue
	*********************************************************************/
	void PlayDialogue(int nDialogueID);

	/*********************************************************************
	*	Name: StopTrack
	*	
	*	Desciption: Stops the designated track
	*********************************************************************/
	void StopTrack(int nTrackID);
	/*********************************************************************
	*	Name: StopSoundEffect
	*	
	*	Desciption: Stops the designated sound effect
	*********************************************************************/
	void StopSoundEffect(int nEffectID);
	/*********************************************************************
	*	Name: StopDialogue
	*	
	*	Desciption: Stops the designated dialogue
	*********************************************************************/
	void StopDialogue(int nDialogueID);
	/*********************************************************************
	*	Name: ResetTrack
	*	
	*	Desciption: Resets the designated track
	*********************************************************************/
	void ResetTrack(int nTrackID);
	/*********************************************************************
	*	Name: ResetSoundEffect
	*	
	*	Desciption: Resets the designated sound effect
	*********************************************************************/
	void ResetSoundEffect(int nEffectID);
	/*********************************************************************
	*	Name: ResetDialogue
	*	
	*	Desciption: Resets the designated dialogue
	*********************************************************************/
	void ResetDialogue(int nDialogueID);
	
public:
	
	// the enumerator for type of sound being played or accessed
	enum { TRACK = 0, SOUNDEFFECT, DIALOGUE };

	// Accessors 
	vector<tSoundInfo>& GetTracks(void)		{ return m_Tracks; }
	vector<tSoundInfo>& GetSFX(void)		{ return m_SoundFX; }
	vector<tSoundInfo>& GetDialogues(void)	{ return m_DialogueFX; }
	
	/*********************************************************************
	*	Name: GetInstance
	*
	*	Desciption: Returns a single static instance of the FMOD wrapper
	*********************************************************************/
	static CFMOD* GetInstance(void){ static CFMOD instance; return &instance; }
	/*********************************************************************
	*	Name: Init
	*
	*	Desciption: Initializes the FMOD sound system
	*********************************************************************/
	bool Init(int nMaxChannels, FMOD_INITFLAGS uiFlags, void* pExtraDriverData);
	/*********************************************************************
	*	Name: ShutDown
	*
	*	Desciption: Cleans up and releases all FMOD system objects 
	*********************************************************************/
	bool Shutdown(void);
	/*********************************************************************
	*	Name: Update
	*
	*	Desciption: Updates the FMOD sound system 
	*********************************************************************/
	bool Update(float fDelta);
	/*********************************************************************
	*	Name: SetVolume
	*
	*	Desciption: Sets the volume of the track group
	*********************************************************************/
	bool SetVolume(unsigned uiSoundType, float fVolume);
	/*********************************************************************
	*	Name: SetPan
	*
	*	Desciption: Sets the pan for nSoundID's sound
	*********************************************************************/
	bool SetPan(unsigned uiSoundType, int nSoundID, float fPan);
	/*********************************************************************
	*	Name: SetFrequency
	*
	*	Desciption: Sets the frequency for nSoundID's sound
	*********************************************************************/
	bool SetFrequency(unsigned uiSoundType, int nSoundID, float fFrequency);
	/*********************************************************************
	*	Name: LoadSound
	*
	*	Desciption: Loads a new sound and puts it in the appropriate vector
	*********************************************************************/
	int LoadSound(const char* szFileName, bool bIsStreaming, FMOD_MODE uiMode);
	/*********************************************************************
	*	Name: UnloadSound
	*
	*	Desciption: Unloads a sound and takes it out of the necessary vector
	*********************************************************************/
	bool UnloadSound(int nSoundID, unsigned uiSoundType);
	/*********************************************************************
	*	Name: IsSoundPlaying
	*	
	*	Desciption: Tells you if the current sound is playing
	*********************************************************************/
	bool IsSoundPlaying(int nSoundID, unsigned uiSoundType);	
	/*********************************************************************
	*	Name: CheckTrackPosition
	*
	*	Description: Returns true if the track passed the specified position
	**********************************************************************/
	bool CheckTrackPosition(int nSoundID, unsigned int unPosition);
	/*********************************************************************
	*	Name: GetTrackLength
	*
	*	Description: Returns the length of the specified track in milliseconds
	**********************************************************************/
	unsigned int GetTrackLength(int nSoundID, unsigned uiSoundType);
	/*********************************************************************
	*	Name: Play
	*	
	*	Desciption: Plays the desired sound, base on type and ID
	*********************************************************************/
	void Play(int nSoundID, unsigned uiSoundType);
	/*********************************************************************
	*	Name: Play3D
	*	
	*	Desciption: Plays the desired sound, base on type and ID
	*********************************************************************/
	void Play3D(int nSoundID, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vVel);
	/*********************************************************************
	*	Name: StopSound
	*	
	*	Desciption: Stop the desired sound, base on type and ID
	*********************************************************************/
	void StopSound(int nSoundID, unsigned uiSoundType);
	/*********************************************************************
	*	Name: ResetSound
	*	
	*	Desciption: Reset the desired sound, base on type and ID
	*********************************************************************/
	void ResetSound(int nSoundID, unsigned uiSoundType);

protected:

	/*********************************************************************
	*	Name: ReleaseChannels
	*	
	*	Desciption: Releases channels handles held by non playing sounds
	*********************************************************************/
	void ReleaseChannels(void);

};
#endif