LoadLuaLib("all")
LoadGameLib("levelPartition")
LoadGameLib("objectSys")
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
require "Assets/Scripts/SH_Talbot"
require "Assets/Scripts/SH_AI"

local fTalbotTimer = 10;
local fGameTimer = 30;
local nLevelMode = 2;
local nTotalSpawn = 0;
local nTotalKills = 0;

function Init()

	--Load partition data
	parID = GetLibID("levelPartition");
	--print("Partition ID ".. parID);
	
	LibLoadScript("./Assets/Scripts/SH_Level3.par",parID);
	LoadParData(0,"./Assets/Models/TestModel/talbotParim.Model","");
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
	
	-- Init the AI for level three startup
	AI.InitAIData(3, -1);
	
	-- Create the Game Factories
	CreateFactory(0,20,20);		--	Enemies
	CreateFactory(1,600,600);		--	Debrie
	CreateFactory(2,5,5);		--	Powerups
	
	--	Create Sun
	local object = CreateObject("sun");
	SetObjPos(object, 10000.0, -20000.0, 60000.0);
	AddToObjectManager(object);
	
	--Load enemies
	Enemy.LoadEnemyData();
	Formation.LoadFormationsLib();

	--Setup Player
	Player.LoadPlayerData();
	
	-- Point light on earth
	earthLight = {0.1, 0.0, 0.0, 0.0,  				-- Ambient
				   1.0, 1.0, 1.0, 1.0,				-- Diffuse
				   1.0, 0.25, 0.25, 0.25,				-- Specular
				   -20000, 1000.0, -2000.0,			-- xyz Pos
				   0,									-- Light Type
				   30000,								-- Range
				   1.0, 0.0, 0.0}					    -- Attenuation
				   
				 --1.0, -0.01, -0.25};					-- Direction				

	-- Directional light on sun
	sunLight =   {1.0, 0.025, 0.025, 0.025,				-- Ambient
				   1.0, 1.0, 1.0, 0.9,			    -- Diffuse
				   1.0, 0.1, 0.1, 0.1,				-- Specular
				   -12000.0, 0.0, -9000.0,		-- xyz Pos
				   1,								-- Light Type
				   5000000,					     	-- Range
				   1.0, 0.0, 0.0,  				-- Attenuation
				   1.0, 0.0, 1.0};					-- Direction
				 	
	-- Directional light on sun
	levelLight =   {1.0, 0.0, 0.0, 0.0,				-- Ambient
				   1.0, 1.0, 1.0, 1.0,			    -- Diffuse
				   1.0, 0.3, 0.3, 0.3,				-- Specular
				   400.0, 0.0, 0.0,				-- xyz Pos
				   0,								-- Light Type
				   2500,					     	-- Range
				   1.0, 0.0, 0.0};				-- Attenuation
								-- Direction
				 	
				 
	-- Add the light to the render engine
	AddLight(earthLight);
	AddLight(sunLight);
	AddLight(levelLight);
	
	SetWinCondition(0);
	SetCameraTarget(Player.playerHandle);
	
	-- Create the Talbot
	Talbot.Create(LoadModel("./Assets/Models/TestModel/talbot.model"),0,0,500);
	SetObjSpeed(Talbot.talbotHandle, -1);
	
	-- Set the talbot as the level boss
	SetLevelBoss(Talbot.talbotHandle);
	
	-- Spawn Talbots Squad
	CreateSquadEnemy(GetEnemyModel(0), 1, 200, 0, 200, -1, 0);
	CreateSquadEnemy(GetEnemyModel(0), 1, 200, 0, -200, -1, 0);
	CreateSquadEnemy(GetEnemyModel(0), 1, -200, 0, 200, -1, 0);
	CreateSquadEnemy(GetEnemyModel(0), 1, -200, 0, -200, -1, 0);
	nTotalSpawn = 4;
	
	-- Get the Level Difficulty
	local nLevelDifficulty = GetDifficulty();
	
	-- Check Medium Difficulty
	if nLevelDifficulty == 1 then
		CreateSquadEnemy(GetEnemyModel(0), 2, 200, 0, 0, -1, 0);
		CreateSquadEnemy(GetEnemyModel(0), 2, -200, 0, 0, -1, 0);
		nTotalSpawn = 6;
	end
	
	-- Check for Hard
	if nLevelDifficulty == 2 then
		CreateSquadEnemy(GetEnemyModel(0), 2, 200, 0, 0, -1, 0);
		CreateSquadEnemy(GetEnemyModel(0), 2, -200, 0, 0, -1, 0);
		CreateSquadEnemy(GetEnemyModel(0), 1, 300, 0, 0, -1, 0);
		CreateSquadEnemy(GetEnemyModel(0), 1, -300, 0, 0, -1, 0);
		nTotalSpawn = 8;
	end
	
	-- Get the Current Number of Kills
	nTotalKills = GetKills();
	
	-- Unlock the player controls
	LockPlayerControls(0);
		
end


function Update(fDeltaTime)

	-- Time is just the begining
	fGameTimer = fGameTimer - fDeltaTime;
	
	-- Check the Level Mode
	if nLevelMode == 2 then
		
		-- Check the timer
		if fGameTimer <= 0 or (GetKills() - nTotalKills) >= nTotalSpawn then
		
			-- Set the new level mode and timer
			nLevelMode = 1
			fGameTimer = 30;
			
			-- Get the Level Difficulty
			local nLevelDiff = GetDifficulty();
			
			-- Check for easy
			if nLevelDiff == 0 then
				SetObjSpeed(Talbot.talbotHandle, 20);
			elseif nLevelDiff == 1 then
				SetObjSpeed(Talbot.talbotHandle, 25);
			else
				SetObjSpeed(Talbot.talbotHandle, 30);
			end

			-- Set AI for Talbot
			AI.InitAIData(3, GetDifficulty());
			
		end
		
	elseif nLevelMode == 1 then
	
		-- Update the Talbot
		Talbot.UpdateTalbot(fDeltaTime);
		
		--	Check that the talbot is still live
		if GetHP(Talbot.talbotHandle) <= 0 then
			
			-- Lock the controls
			LockPlayerControls(1);
			
			-- Check if the timer is up
			if fTalbotTimer < 0 then
				-- Set the Win Condition
				SetWinCondition(1);
			end
			
			-- Decriment the talbot timer
			fTalbotTimer = fTalbotTimer - fDeltaTime;
		
		-- Check if easy mode
		elseif GetDifficulty() == 0 then
					
			-- Check the Powerup Spawn Timer
			if fGameTimer <= 0 then	
			
				-- Reset the Time
				fGameTimer = 30;
				
				-- Create a power-up
				local pPowerUp = CreateObject(1,"powup");
				local x = 0; local y = 0; local z = 0;
				
				-- Check that the powerup is valid
				if pPowerUp == 0 then
					return;
				end
				
				-- Get and Set the Possition
				x, y, z = GetObjPos(Talbot.talbotHandle)
				SetObjPos(pPowerUp, x, y, z);
				
				-- Add the powerup to the object manager
				AddToObjectManager(pPowerUp);
			end
		end
	
	end

end

-- Shutdown the level and clean up data
function Shutdown()

end
