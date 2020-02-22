-- SH_Player - Player pack data
module("Player", package.seeall);

--	Load in the animation data
LoadGameLib("animation");

playerHandle = 0;

-- Load the object Library
LoadGameLib("objectSys");

--	Load the enemy pack data
function LoadPlayerData()
	
	--	Create and set the player data
	playerHandle = CreatePlayer();
	SetPlayerData(-1, 0, 25, -50, 35.0, 9001, 6000);
	SetAITarget(playerHandle);
	SetMeleeData(playerHandle, -1, 4.0);
	AddToObjectManager(playerHandle);
	
	--	Load in the player animation timings
	AddObjectAnimIndex(playerHandle, 0, 40);		--	Idle
	AddObjectAnimIndex(playerHandle, 41, 60);		--	Forward
	AddObjectAnimIndex(playerHandle, 61, 80);		--	Retreat
	
	
end
