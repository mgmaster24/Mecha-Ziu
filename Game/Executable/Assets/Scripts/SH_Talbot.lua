LoadLuaLib("all");
LoadGameLib("objectSys");
LoadGameLib("animation");
LoadGameLib("levelPartition")

module("Talbot", package.seeall);

-- Handle To the Talbot
talbotHandle = 0;

-- Create the Talbot
function Create(modelID, x, y, z)

	-- Create the talbot
	talbotHandle = CreateObject("talbot");

	-- Check that talbot is valid
	if talbotHandle == 0 then
		return 0;
	end

	-- Set the possition
	SetObjPos(talbotHandle,x,y,z);

	-- Set the Model ID
	SetObjModelID(talbotHandle, modelID);

	-- Add the talbot to the object manager
	AddToObjectManager(talbotHandle);
	
	-- Init the Interpolator
	InitInterpolator(talbotHandle);
	
	-- Init the Melee Data
	SetMeleeData(talbotHandle,modelID,-1);
	
	-- Get the Level Difficulty
	local nLevelDiff = GetDifficulty();
	local nTalbotHealth = 200000 * (0.5 + (0.5 * nLevelDiff));
	
	-- Set the Talbot HP
	SetHP(talbotHandle, nTalbotHealth);
	
	--	Set the animation indexes
	AddObjectAnimIndex(talbotHandle,1,10,0,0);		-- Forward
	AddObjectAnimIndex(talbotHandle,15,25,1,0);		-- Retreat
	AddObjectAnimIndex(talbotHandle,30,40,2,0);		-- Rise
	AddObjectAnimIndex(talbotHandle,45,55,3,0);		-- Fall
	AddObjectAnimIndex(talbotHandle,60,70,4,0);		-- Left
	AddObjectAnimIndex(talbotHandle,75,85,5,0);		-- Right
	AddObjectAnimIndex(talbotHandle,90,110,6,1);	-- Idle
	AddObjectAnimIndex(talbotHandle,110,116,7,0);	-- Combat State
	AddObjectAnimIndex(talbotHandle,116,130,8,0);	-- Combo 1
	AddObjectAnimIndex(talbotHandle,130,145,9,0);	-- Combo 2
	AddObjectAnimIndex(talbotHandle,145,160,10,0);	-- Combo 3
	AddObjectAnimIndex(talbotHandle,160,175,11,0);	-- Combo 4
	
	-- Update the interpolator and create transform data
	UpdateInterpolator(talbotHandle,0.01);
	
	-- Retrieve and set transforms
	SetControlTransforms(talbotHandle,"gunarmshape",0);
	SetSphereOffsets(talbotHandle,"gunarm",0);
	SetSphereOffsets(talbotHandle,"head",1);
	SetSphereOffsets(talbotHandle,"head",2);

	-- Return successfull
	return 1;
end

-- Update for the talbot
function UpdateTalbot(deltaTime)

	-- Check if the talbot is in bounds
	if CheckInBounds(talbotHandle) == 2 then
		OutOfBounds(talbotHandle);
	end
	
end

-- Destroy the Talbot and clean up any data
function Destroy()

end