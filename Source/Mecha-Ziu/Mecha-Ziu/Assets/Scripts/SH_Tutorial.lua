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

--	Global Script Variables --

--	Enemy Spawns
local nTotalSpawn = 0;

--	Spawn Type
local nEnemyType = Enemy.GetBlueID();
local nWaveIndex = 1;
local earthLight = 0;
local sunLight = 0;
local moonLight = 0;

local war = 0;

function Init()

	--	Load the partition data
	parID = GetLibID("levelPartition");
	print("Partition ID ".. parID);
	
	LibLoadScript("./Assets/Scripts/SH_Level1.par",parID);
	LoadParData(0,"./Assets/Models/TestModel/parimTutorial.Model","");
	print("Partition Loaded");

	--Load Models
	LModel.LoadModels("./Assets/Models/TestModel/skybox.Model");

	--Load Particles
	Particle.LoadParticles()
	
	--Reset the Object Manager
	ResetObjectManager();
	
	--Load Player
	Player.LoadPlayerData();
	print("Player Loaded");
	
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
	
	-- Create the Game Factories
	CreateFactory(0,75,75);		--	Enemies
	CreateFactory(1,600,600);	--	Debris
	CreateFactory(2,10,10);		--	Powerups
	
	--Load enemies
	Enemy.LoadEnemyData();
	Formation.LoadFormationsLib();
	
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
				   200.0, 0.0, 0.0,				-- xyz Pos
				   0,								-- Light Type
				   2000,					     	-- Range
				   1.0, 0.0, 0.0};				-- Attenuation
			
		 
	-- Add the light to the render engine
	AddLight(earthLight);
	AddLight(sunLight);
	AddLight(levelLight);
               
	SetWinCondition(0);
	
	SetCameraTarget(Player.playerHandle);
	
	
end


function Update(fTime)
	
	--EC
	if GetKills() == 1 and nTotalSpawn == 0 then
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetYellowID(), 0, 0, 200, 1, 0)
		nTotalSpawn = 1
	end
	
	--EC Charged
	if GetKills() == 2 and nTotalSpawn == 1 then
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetBlueID(), 0, 0, 400, 1, 0)
		nTotalSpawn = 2
	end
	
	--Head Turrets
	if GetKills() == 3 and nTotalSpawn == 2 then
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetWhiteID(), 20, 0, 300, 1, 0)
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetWhiteID(), 0, 0, 300, 1, 0)
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetWhiteID(), -20, 0, 300, 1, 0)
		nTotalSpawn = 5
	end
	
	--Missiles
	if GetKills() == 6 and nTotalSpawn == 5 then
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetRedID(), 20, 0, 100, 1, 0)
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetRedID(), 0, 0, 300, 1, 0)
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetRedID(), -20, 0, 100, 1, 0)
		nTotalSpawn = 8
	end
	
	--Melee
	if GetKills() == 9 and nTotalSpawn == 8 then
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetGreenID(), 20, 0, 200, 1, 0)
		nTotalSpawn = 9
	end
	
	--Practice
	if GetKills() == 10 and nTotalSpawn == 9 then
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetBlueID(), 0, 0, 250, 1, 0)
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetGreenID(), 20, 20, 250, 1, 0)
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetGreenID(), -20, 20, 250, 1, 0)
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetYellowID(), 40, 40, 250, 1, 0)
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetYellowID(), -40, 40, 250, 1, 0)
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetRedID(), 60, 60, 250, 1, 0)
		CreateSquadEnemy(GetEnemyModel(0), Enemy.GetRedID(), -60, 60, 250, 1, 0)
		nTotalSpawn = 16
	end
	
	if GetKills() == 17 then
		SetWinCondition(1)
	end

end
