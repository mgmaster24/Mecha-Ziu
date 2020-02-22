------------------------------------------------------------------------
--	File Name	:	SH_Level1.lua											--
--																			--
--	Description	:	Handles the initialization and updating of level 1	--
------------------------------------------------------------------------

LoadLuaLib("all")
LoadGameLib("levelPartition")
LoadGameLib("objectSys")
LoadGameLib("renderSys")
LoadGameLib("gameHud")
LoadGameLib("sobjSys")
LoadGameLib("dialogue")
LoadGameLib("particle")

require "Assets/Scripts/SH_Player"
require "Assets/Scripts/SH_Enemy"
require "Assets/Scripts/SH_Formation"
require "Assets/Scripts/SH_Squad"
require "Assets/Scripts/SH_LModel"
require "Assets/Scripts/SH_Particles"
require "Assets/Scripts/SH_AI"

--	[Global Script Variables] --

--	Wave Time
local fWaveTime = 0;
local fNextWaveTime = 20;
local fBossTime = -1;

--	Enemy Spawn count
local nTotalSpawn = 0;

--	Spawn Type
local nEnemyType = Enemy.GetBlueID();
local nWaveIndex = 1;
local spawnTable = 0;

-- Lights
local earthLight = 0;
local sunLight = 0;
local moonLight = 0;
local levelLight = 0;
-- WarTurtle
local war = 0;

function Init()

	--	Load the partition data
	parID = GetLibID("levelPartition");
	print("Partition ID ".. parID);
	
	LibLoadScript("./Assets/Scripts/SH_Level1.par",parID);
	LoadParData(0,"./Assets/Models/TestModel/SH_Parim.Model","");
	print("Partition Loaded");

	--	Load Models
	LModel.LoadModels("./Assets/Models/TestModel/skyspherenear.Model");

	--	Load Particles
	Particle.LoadParticles()
	
	--	Reset the Object Manager
	ResetObjectManager();
	
	--	Load Player
	Player.LoadPlayerData();
	print("Player Loaded");
	
	--	Load Dialogue stuff
	local dialogueID = 0;
	dialogueID = LoadDialogue("./Assets/Scripts/test.dlg");
	SetDialogue(dialogueID);
	
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
	
	-- Init the AI for level one
	AI.InitAIData(1, 1);
	
	-- Create the Game Factories
	CreateFactory(0,75,75);		--	Enemies
	CreateFactory(1,600,600);		--	Debrie
	CreateFactory(2,10,10);		--	Powerups
	
	--	Create Sun
	local object = CreateObject("sun");
	SetObjPos(object, -40000.0, 2000.0, -60000.0);
	AddToObjectManager(object);
	
	--	Create Earth
	object = CreateObject("earth");
	AddToObjectManager(object);
	
	--	Create Moon
	object = CreateObject("moon");
	AddToObjectManager(object);
	
	--	Load enemies
	Enemy.LoadEnemyData();
	Formation.LoadFormationsLib();
	
	--	Create the Warturtle
	war = CreateWarTurtle(LoadModel("./Assets/Models/TestModel/wtbody.Model"),0,200,0,600);
	
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
				 	
	-- Point light in the level
	levelLight =   {1.0, 0.0, 0.0, 0.0,				-- Ambient
				   1.0, 1.0, 1.0, 1.0,			    -- Diffuse
				   1.0, 0.3, 0.3, 0.3,				-- Specular
				   400.0, 0.0, 0.0,				-- xyz Pos
				   0,								-- Light Type
				   2500,					     	-- Range
				   1.0, 0.0, 0.0};				-- Attenuation
			
		 
	-- Add the light to the render engine
	AddLight(earthLight);
	AddLight(sunLight);
	AddLight(levelLight);
	
	-- Clear win codition
	SetWinCondition(0);
	
	EnableWarturtleAI()
	SetDialogue(LoadDialogue("./Assets/Scripts/BossStart.dlg"));
	DisplayDialogue(1)
	print("Warturtle engaged");

	fWaveTime = 0;
	nWaveIndex = 7;
	fBossTime = -1;
	
	-- Set camera
	SetCameraTarget(Player.playerHandle);
	
end


function Update(fTime)
	
	--	Incriment the wave time past
	if GetWinCondition() == 0 then
	
		if fBossTime == -1 then
			
			-- Initial enemies
			CreateSquadEnemy(GetEnemyModel(0), Enemy.GetBlueID(), -50, 50, 200, 1, 0);
			CreateSquadEnemy(GetEnemyModel(0), Enemy.GetBlueID(), 50, 50, 200, 1, 0);
			CreateSquadEnemy(GetEnemyModel(0), Enemy.GetBlueID(), -75, 50, 200, 1, 0);
			CreateSquadEnemy(GetEnemyModel(0), Enemy.GetBlueID(), 75, 50, 200, 1, 0);
			fBossTime = 0;
			
			return;
		
		end
		
	end

	--	If win conditions are met
	if CheckWarturtleAlive() == 1 then
		
		--	End the Level, Set the Win Condition
		SetWinCondition(1);
		
	end		
	
end
