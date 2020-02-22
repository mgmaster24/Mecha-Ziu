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

--	Wave Time
local fWaveTime = 0;
local fNextWaveTime = 20;

--local fTimer = 0;

--	Enemy Spawns
local nTotalSpawn = 0;

--	Spawn Type
local nEnemyType = Enemy.GetBlueID();
local nWaveIndex = 1;
local spawnTable = 0;
local earthLight = 0;
local sunLight = 0;
local moonLight = 0;
local levelLight = 0;
local war = 0;

function Init()

	--	Load the partition data
	parID = GetLibID("levelPartition");
	print("Partition ID ".. parID);
	
	LibLoadScript("./Assets/Scripts/SH_Level1.par",parID);
	LoadParData(0,"./Assets/Models/TestModel/SH_Parim.model","");
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
	
	--Load Dialogue stuff
	local dialogueID = 0;
	dialogueID = LoadDialogue("./Assets/Scripts/survival.dlg");
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
	
	-- Create the Game Factories
	CreateFactory(0,75,75);		--	Enemies
	CreateFactory(1,600,600);	--	Debris
	CreateFactory(2,10,10);		--	Powerups
	
	--	Create Sun
	local object = CreateObject("sun");
	SetObjPos(object, -40000.0, 2000.0, -60000.0);
	AddToObjectManager(object);
	
	--Create Earth
	object = CreateObject("earth");
	AddToObjectManager(object);
	
	--Create Moon
	object = CreateObject("moon");
	AddToObjectManager(object);
	
	--Load enemies
	Enemy.LoadEnemyData();
	Formation.LoadFormationsLib();
	
	--	Create the Warturtle
	war = CreateWarTurtle(LoadModel("./Assets/Models/TestModel/wtbody.Model"),0,400,0,600);
	--SetObjPos(warTurtle, 200, 0, 600);
	
	-- Set the spawn table
	spawnTable = {5, 10, 20, 30, 40};
	--spawnTable = {1, 1, 1, 1, 1};
	
	
	-- Set the total number of enemies
	SetLevelEnemies(0);
	
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
	
	--Initial enemies
	--CreateEnemy(GetEnemyModel(0), Enemy.GetBlueID(), -100, 50, -200,);
	--CreateEnemy(GetEnemyModel(0), Enemy.GetBlueID(), 100, 50, -200,);
	
	nTotalSpawn = 0;
	
end


function Update(fTime)

	--	Incriment the wave time past
	fWaveTime = fWaveTime + fTime;
	--fTimer = fTimer * fTime;

	if fWaveTime > fNextWaveTime or GetKills() == nTotalSpawn then
			
		if nWaveIndex == 6 then
			nWaveIndex = 5
		end
		
		print("Num Spawn".. nTotalSpawn);
		print("Current Kills".. GetKills());
		 
		--	Check that the enemy type is valid
		if nEnemyType > Enemy.GetWhiteID() then
		
			nEnemyType = 1;
			
		end
			
		-- Spawn a squad of this type
		local x,y,z = GetObjPos(war)
		local actualSpawn = Squad.CreateSquad(spawnTable[nWaveIndex], -1, nEnemyType, x, y, z);
		
		-- Set data for next spawn
		if	actualSpawn == spawnTable[nWaveIndex] then
		
			nEnemyType = nEnemyType + 1;
			nTotalSpawn = nTotalSpawn + spawnTable[nWaveIndex];
		
		else
			
			nEnemyType = nEnemyType + 1;
			nTotalSpawn = nTotalSpawn + actualSpawn;
			
		end
		
		--	Get the next wave time
		fNextWaveTime = 15 + (1 * spawnTable[nWaveIndex]);
		
		-- Incriment the spawn table
		nWaveIndex = nWaveIndex + 1;
		
		--	Reset the spawn time
		fWaveTime = 0;
		
		print("Next wave in " ..fNextWaveTime);
	
	end

end
