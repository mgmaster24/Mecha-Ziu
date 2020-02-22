LoadLuaLib("all");
LoadGameLib("objectSys");
LoadGameLib("renderSys");
LoadGameLib("aiSys");

function POC()

	parID = GetLibID("levelPartition");
	print("Partition ID ".. parID);
	
	LibLoadScript("./Assets/Scripts/SH_PocPartition.par",parID);
	print("Partition Loaded");
	
	--	Set the player atributes
	SetPlayerData(-1,0,5,-20,0,9001,6000);
	
	--	Build and load the formation data
	BuildFormations();
	
	--	Load the enemy data
	table = 
	{
		{4000,5,0.2,8,3,4},
		{8000,5,0.75,1500,.05,2000},
	};
	
	--	Load all the enemy pairs
	for index, value in pairs(table) do
		LoadEnemyPack(table[index]);
	end
	
	--	Load the enemy model
	local enemyModelID = LoadModel("./Assets/Models/TestModel/yuna.model");
	
	-- Set the Stored Model ID for enemy one
	SetEnemyModel(0, enemyModelID);
	
	-- Create a squad
	CreateSquadEnemy(enemyModelID,10,10,10,12,0,1);
	CreateSquadEnemy(enemyModelID,12,11,10,12,0,0);
	CreateSquadEnemy(enemyModelID,14,12,10,12,0,0);
	CreateSquadEnemy(enemyModelID,16,13,10,12,0,0);

end

function BuildFormations()

	--	Create a new Formation
	local nFormationID = CreateFormation(0);
	
	--	Setup the formation information
	local formTable = 
	{
		{ -10, 0, 0, 5 },
		{ 10, 0, 0, -5 },
		{ 25, 5, 0, 0 },
		{ 25, -5, 0, 0 },
	};
	
	--	Load the formation table
	for index, value in pairs(formTable) do
		AddFormationTable(formTable[index],nFormationID);
	end

end