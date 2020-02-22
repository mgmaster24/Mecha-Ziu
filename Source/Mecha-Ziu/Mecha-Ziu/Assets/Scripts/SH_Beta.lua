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

local staticEarth = 0;
local staticCloud = 0;
local staticMoon = 0;
local staticSun = 0;
local war = 0;

function Init()

	--	Load the partition data
	parID = GetLibID("levelPartition");
	print("Partition ID ".. parID);
	
	LibLoadScript("./Assets/Scripts/SH_Level1.par",parID);
	LoadParData(0,"./Assets/Models/TestModel/SH_Parim.Model","");
	print("Partition Loaded");

	--Load Models
	LModel.LoadModels("./Assets/Models/TestModel/skybox.Model");

	--Load Particles
	Particle.LoadParticles()
	
	--Load Player
	Player.LoadPlayerData();
	print("Player Loaded");
	
	--Load Dialogue stuff
	local dialogueID = 0;
	dialogueID = LoadDialogue("./Assets/Scripts/test.dlg");
	SetDialogue(dialogueID);
	
	--Load out of bounds message
	local outOfBounds = 0;
	outOfBounds = {6, 
					"\n SYSTEM MESSAGE							   \n",
					"\n	WARNING: YOU ARE LEAVING THE BATTLFIELD \n",
					"\n	INCOMMING PROJECTILES DETECTED          \n",
					"\n	ESTIMATED SURVIVAL PERCENTAGE: 0.0001%  \n",
					"\n	BE ADVISED, RETURN TO THE BATTLEFIELD   \n",
				    "\n	OR YOU WILL BE DESTROYED				   \n"};
					
	LoadParMessage(outOfBounds, "./Assets/Audio/DLGWodenTest.wav");
	
	--Create Earth
	--staticEarth = CreateObject("earth");
	--SObjScale(staticEarth, 65.0, 65.0, 65.0);
	--SObjRotY(staticEarth, 45.0);
	--SetSObjPos(staticEarth, 0.0, -15.0, -5000.0);
	
	--Create Moon
	--staticMoon = CreateObject("moon");
	--SObjScale(staticMoon, 7.0, 7.0, 7.0);
	--SetSObjPos(staticMoon, -60.0, 0.0, 1500.0);
	
	--Create Sun
	--staticSun = CreateObject("sun");
	--SetSObjPos(staticSun, -9000.0, 4000.0, -14000.0);
	--SObjScale(staticSun, 55.0, 55.0, 55.0);
	
	--Load enemies
	Enemy.LoadEnemyData();
	Formation.LoadFormationsLib();
	
	--	Create the Warturtle
	war = CreateWarTurtle(LoadModel("./Assets/Models/TestModel/wtbody.Model"),0,400,0,600);
	--SetObjPos(warTurtle, 200, 0, 600);
	
	-- Set the spawn table
	spawnTable = {4, 6, 8, 10};
	--spawnTable = {1, 1, 1, 1, 1};
	
	
	-- Set the total number of enemies
	SetLevelEnemies(30);
	
	-- Point light on earth
	earthLight = {0.1, 0.0, 0.0, 0.0,  				-- Ambient
				   1.0, 0.75, 0.75, 0.75,				-- Diffuse
				   0.1, 0.00, 0.00, 0.00,				-- Specular
				   -12000, 1000.0, -2000.0,			-- xyz Pos
				   0,									-- Light Type
				   3000000,								-- Range
				   0.01, 0.0, 0.0}					    -- Attenuation
				   
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
               
	SetWinCondition(0);
	
	SetCameraTarget(Player.playerHandle);
	
	--Initial enemies
	CreateSquadEnemy(GetEnemyModel(0), Enemy.GetBlueID(), -50, 50, 200, 1, 0);
	CreateSquadEnemy(GetEnemyModel(0), Enemy.GetBlueID(), 50, 50, 200, 1, 0);
	--CreateEnemy(GetEnemyModel(0), Enemy.GetBlueID(), -100, 50, -200,);
	--CreateEnemy(GetEnemyModel(0), Enemy.GetBlueID(), 100, 50, -200,);
	
	nTotalSpawn = 2;
	
end


function Update(fTime)

	--	Incriment the wave time past
	fWaveTime = fWaveTime + fTime;
	--fTimer = fTimer * fTime;

	if fWaveTime > fNextWaveTime or GetKills() == nTotalSpawn then
		
		--	Check that we have not reached the last wave
		if nWaveIndex == 5 then
		
			if GetKills() == nTotalSpawn then
				
				EnableWarturtleAI()
				SetDialogue(LoadDialogue("./Assets/Scripts/BossStart.dlg"));
				DisplayDialogue(1)
				print("Warturtle engaged");
			
				fWaveTime = 0;
				nWaveIndex = 6;
				
			end
			
			return;
			
		elseif nWaveIndex == 6 then
		
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
		local x,y,z = GetObjPos(war)
		Squad.CreateSquad(spawnTable[nWaveIndex], -1, nEnemyType, x, y, z);
		
		-- Set data for next spawn
		nEnemyType = nEnemyType + 1;
		nTotalSpawn = nTotalSpawn + spawnTable[nWaveIndex];
		
		--	Get the next wave time
		fNextWaveTime = 10 + (1 * spawnTable[nWaveIndex]);
		
		-- Incriment the spawn table
		nWaveIndex = nWaveIndex + 1;
		
		--	Reset the spawn time
		fWaveTime = 0;
		
		print("Next wave in " ..fNextWaveTime);
	
	end

end
