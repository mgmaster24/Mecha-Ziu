LoadLuaLib("all")
LoadGameLib("levelPartition")
LoadGameLib("objectSys")
------------------------------------------------------------------------
--	File Name	:	SH_Level2.lua											--
--																			--
--	Description	:	Handles the initialization and updating of level 2	--
------------------------------------------------------------------------

LoadGameLib("renderSys")
LoadGameLib("gameHud")
LoadGameLib("sobjSys")
LoadGameLib("dialogue")
LoadGameLib("particle")

require "Assets/Scripts/SH_Enemy"
require "Assets/Scripts/SH_Player"
require "Assets/Scripts/SH_Formation"
require "Assets/Scripts/SH_Squad"
require "Assets/Scripts/SH_LModel"
require "Assets/Scripts/SH_Particles"
require "Assets/Scripts/SH_AI"

--	[Global Script Variables] --

-- Level time
local fDeathTime = 0;

-- WarTurtle
local warTurtle = 0;

-- Enemy Spawns
local spawnTable = 0;
local spawnTimeTable = 0;
local spawnIndex = 1;
local spawnIndexTotal = 7;
local spawnOffsets = 0;

function Init()

	--Load partition data
	parID = GetLibID("levelPartition");
	--print("Partition ID ".. parID);
	
	LibLoadScript("./Assets/Scripts/SH_Level2.par",parID);
	LoadParData(0,"./Assets/Models/TestModel/SH_Parim.Model","");
	--print("Partition Loaded");

	--Load Models
	LModel.LoadModels("./Assets/Models/TestModel/skyspherenear.Model");
	
	--Load Particles
	Particle.LoadParticles()
	
	--Load Dialogue
	local dialogueID = 0;
	dialogueID = LoadDialogue("./Assets/Scripts/test.dlg");
	SetDialogue(dialogueID);
	
	--Reset the Object Manager
	ResetObjectManager();
	
	--	Load out of bounds message
	local outOfBounds = 0;
	outOfBounds = {2, 
					"\n Get back in the battle Raido.			   \n",
				    "\n											   \n"};
	LoadParMessage(outOfBounds, "./Assets/Audio/Dialogue/DLGAdmiralGetBack.ogg", 0);
	LoadParMessage(outOfBounds, "./Assets/Audio/Dialogue/DLGAdmiralGetBack2.ogg", 1);
	
	outOfBounds = {2, 
				"\n You can't leave just yet. There is still a    \n", 
				"\n war going on.	 						      \n"};
	LoadParMessage(outOfBounds, "./Assets/Audio/Dialogue/DLGAdmiralYouCant.ogg", 2);
	
	outOfBounds = {2, 
				"\n You can't leave now. There is still a    \n", 
				"\n war going on.							 \n"};
	LoadParMessage(outOfBounds, "./Assets/Audio/Dialogue/DLGAdmiralYouCant2.ogg", 3);
	LoadParMessage(outOfBounds, "./Assets/Audio/Dialogue/DLGAdmiralYouCant3.ogg", 4);
	
	-- Get the Level Difficulty
	local nLevelDiff = GetDifficulty();
	--print("Difficulty : " .. nLevelDiff);
	
	-- Init the AI for level two
	AI.InitAIData(2, nLevelDiff);
	
	-- Load Enemy Data
	Enemy.LoadEnemyData();
	
	-- Create the Game Factories
	CreateFactory(0,20,20);		--	Enemies
	CreateFactory(1,80,80);		--	Debrie
	CreateFactory(2,5,5);			--	Powerups

	--	Create Moon
	local object = CreateObject("moon");
	SetObjPos(object, -3000.0, 2000.0, 10000.0);
	AddToObjectManager(object);

	--Setup Player
	Player.LoadPlayerData();
	--print("Player Loaded");
	
	--	Create the Warturtle
	warTurtle = CreateWarTurtle(LoadModel("./Assets/Models/TestModel/wt2body.Model"),1,0,-50,350);
	
	
	-- Point light on earth
	earthLight = {0.1, 0.0, 0.0, 0.0,  				-- Ambient
				   1.0, 0.55, 0.55, 0.55,				-- Diffuse
				   0.1, 0.00, 0.00, 0.00,				-- Specular
				   -12000, 1000.0, -2000.0,			-- xyz Pos
				   0,									-- Light Type
				   3000000,								-- Range
				   0.01, 0.0, 0.0};				    -- Attenuation
				 --1.0, -0.01, -0.25};					-- Direction				

	-- Directional light on sun
	sunLight =   {1.0, 0.1, 0.1, 0.1,				-- Ambient
				   1.0, 0.7, 0.7, 0.6,			    -- Diffuse
				   1.0, 0.005, 0.005, 0.005,		-- Specular
				   -12000.0, 0.0, -9000.0,		-- xyz Pos
				   1,								-- Light Type
				   5000000,					     	-- Range
				   0.0, 0.0, 0.0,  				-- Attenuation
				   1.0, 0.0, 1.0};					-- Direction
				 	
				 
	-- Add the light to the render engine
	AddLight(earthLight);
	AddLight(sunLight);
	
	-- Clear win condition
	SetWinCondition(0);
	
	-- Set camera
	SetCameraTarget(Player.playerHandle);
	
	-- Create the Spawn Table
	if nLevelDiff == 0 then
		spawnTable		= {2,3,3,4,4,4,5};
		spawnTimeTable	= {15,25,35,45,55,65,75};
	elseif nLevelDiff == 1 then
		spawnTable		= {4,6,6,8,8,8,10};
		spawnTimeTable	= {5,15,30,40,50,60,75};
	elseif nLevelDiff == 2 then
		spawnTable		= {6,6,6,8,8,10,10};
		spawnTimeTable	= {5,15,20,30,40,55,75};
	end
	
	-- X ad Y offsets of the spawn
	spawnOffsets	= {
						{-50,20},{50,20}, {-50,-15}, {50,-15},
						{-50,40},{50,40}, {-15,60}, {15,60},
						{-50,-30},{50,-30}
					   };
	
	-- Unlock the player controls
	LockPlayerControls(0);
	
end


function Update(fTime)
	
	-- Check for locked player controls
	if CheckLockPlayer() == 1 then
		return;
	end
	
	--Increment death timer
	fDeathTime = fDeathTime + fTime;
	
	-- Check the spawn index
	if spawnIndex <= spawnIndexTotal then
	
		-- Check the spawn timer
		if spawnTimeTable[spawnIndex] < fDeathTime then
		
			-- Get the Warturtle possition
			local x = 0; local y = 0; local z = 0;
			x, y, z = GetObjPos(warTurtle)
			
			-- Itterate and spawn
			for nItter = 1, spawnTable[spawnIndex], 1 do
				CreateSquadEnemy(GetEnemyModel(0), Enemy.GetGreyID(), x + spawnOffsets[nItter][1], y + spawnOffsets[nItter][2], z - 25, -1, 0);	
			end
			
			-- Incriment the Spawn Index
			spawnIndex = spawnIndex + 1;
		
		end
		
	end

	--Kill player if time runs out
	if fDeathTime > 91.0 then
	
		SetHP(Player.playerHandle, 0.0);
		 
	end

	--Check win condition
	if CheckWarturtleAlive(1) == 1 then
		
		--You win
		SetWinCondition(1);
	
	end

end
