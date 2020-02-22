LoadLuaLib("all")
LoadGameLib("levelPartition")
LoadGameLib("objectSys")
LoadGameLib("renderSys")
LoadGameLib("gameHud")
LoadGameLib("sobjSys")

require "Assets/Scripts/SH_Enemy"
require "Assets/Scripts/SH_Formation"
require "Assets/Scripts/SH_Squad"
require "Assets/Scripts/SH_LModel"

--	Global Script Variables
--	Wave Time
local fWaveTime = 0;
local fNextWaveTime = 20;

--	Enemy Spawns
local nTotalSpawn = 0;

--	Spawn Type
local nEnemyType = Enemy.GetBlueID();
local nWaveIndex = 1;
local spawnTable = 0;
local earthLight = 0;
local sunLight = 0;
local moonLight = 0;

local staticEarth = 0;
local staticCloud = 0;
local staticMoon = 0;
local staticSun = 0;


function Init()

	--	Load the partition data
	parID = GetLibID("levelPartition");
	print("Partition ID ".. parID);
	
	LibLoadScript("./Assets/Scripts/SH_Level1.par",parID);
	LoadParData(0,"./Assets/Models/TestModel/SH_Parim.Model","");
	print("Partition Loaded");

	--Load Models
	LModel.LoadModels("./Assets/Models/TestModel/skybox.Model");
	
	--Load out of bounds message
	--local outOfBounds = 0;
	--outOfBounds = {1, "WARNING: YOU ARE LEAVING THE BATTLEIELD"};
	--LoadParMessage("WARNING: YOU ARE LEAVING THE BATTLEIELD", "./Assets/Audio/DLGWodenTest.wav");
	
	--Create Earth
	--staticEarth = CreateStaticObj("./Assets/Models/TestModel/Earth.Model");
	--SObjScale(staticEarth, 65.0, 65.0, 65.0);
	--SObjRotY(staticEarth, 45.0);
	--SetSObjPos(staticEarth, 0.0, -15.0, -5000.0);
	
	--Create Cloud model
	--staticCloud = CreateStaticObj("./Assets/Models/TestModel/StaticSphere.Model", "./Assets/Textures/clouds.png");
	--SObjScale(staticCloud, 30.0, 30.0, 30.0);
	--SetSObjPos(staticCloud, 0.0, -15.0, -5000.0);
	
	--Create Moon
	--staticMoon = CreateStaticObj("./Assets/Models/TestModel/Earth.Model", "./Assets/Textures/Moon.png");
	--SObjScale(staticMoon, 7.0, 7.0, 7.0);
	--SetSObjPos(staticMoon, -60.0, 0.0, 1500.0);
	
	--Create Sun
	--staticSun = CreateStaticObj("./Assets/Models/TestModel/Earth.Model", "./Assets/Textures/Sun.png");
	--SetSObjPos(staticSun, -9000.0, 4000.0, -14000.0);
	--SObjScale(staticSun, 55.0, 55.0, 55.0);
	
	--Load enemies
	Enemy.LoadEnemyData();
	Formation.LoadFormationsLib();
	
	--Setup Player
	SetPlayerData(-1, 0, 25, -50, 30.0, 9001, 6000);
	
	--	Create the Warturtle
	CreateWarTurtle(LoadModel("./Assets/Models/TestModel/Earth.Model"),0,0,0,600);
	
	-- Set the spawn table
	spawnTable = {2, 3, 4};
	
	-- Set the total number of enemies
	SetLevelEnemies(9);
	
	-- Point light on earth
	earthLight = {0.1, 0.0, 0.0, 0.0,  				-- Ambient
				   1.0, 0.8, 0.8, 0.8,					-- Diffuse
				   0.1, 0.0000, 0.0000, 0.0000,		-- Specular
				   -12000, 1000.0, -2000.0,			-- xyz Pos
				   0,									-- Light Type
				   5000000,								-- Range
				   0.01, 0.0, 0.0}					   -- Attenuation
				   
				 --1.0, -0.01, -0.25};					-- Direction				

	-- Directional light on sun
	sunLight =   {1.0, 0.1, 0.1, 0.1,				-- Ambient
				   1.0, 0.7, 0.7, 0.6,			    -- Diffuse
				   1.0, 0.04, 0.04, 0.04,			-- Specular
				   -12000.0, 0.0, -9000.0,		-- xyz Pos
				   1,								-- Light Type
				   3000000,					     	-- Range
				   0.01, 0.0, 0.0,  				-- Attenuation
				   1.0, -1.0, 1.0};				-- Direction
				 	
				 
	-- Add the light to the render engine
	AddLight(earthLight);
	AddLight(sunLight);
               
	
	SetWinCondition(0);
	
end


function Update(fTime)

	--	Incriment the wave time past
	fWaveTime = fWaveTime + fTime;

	if fWaveTime > fNextWaveTime or GetKills() == nTotalSpawn then
		
		--	Check that we have not reached the last wave
		if nWaveIndex == 4 then
		
			if GetKills() == nTotalSpawn then
				
				EnableWarturtleAI()
			
				fWaveTime = 0;
				nWaveIndex = 5;
				
			end
			
			return;
			
		elseif nWaveIndex == 5 then
		
			if GetKills() == nTotalSpawn and CheckWarturtleAlive() == 1 then
				
				--	End the Level, Set the Win Condition
				SetWinCondition(1);
				
			end
			
			return;
		end
		
		print("Num Spawn".. nTotalSpawn);
		print("Current Kills".. GetKills());
		
		--	Check that the enemy type is valid
		if nEnemyType > Enemy.GetWhiteID() then
			nEnemyType = 1;
		end
			
		-- Spawn a squad of this type
		Squad.CreateSquad(spawnTable[nWaveIndex], -1, nEnemyType, -20, 15, 400);
		
		-- Set data for next spawn
		nEnemyType = nEnemyType + 1;
		nTotalSpawn = nTotalSpawn + spawnTable[nWaveIndex];
		
		--	Get the next wave time
		fNextWaveTime = 15 + (5 * spawnTable[nWaveIndex]);
		
		-- Incriment the spawn table
		nWaveIndex = nWaveIndex + 1;
		
		--	Reset the spawn time
		fWaveTime = 0;
		
		print("Next wave in " ..fNextWaveTime);
	
	end

end
