-- SH_Formations - Enemies pack data
module("Enemy", package.seeall);

-- Load the AI Library
LoadGameLib("objectSys");

--	Load the enemy pack data
function LoadEnemyData()
	
	--	Clear the previous enemy data
	ClearEnemyPack();
	
	--	The local enemy data table
	local EnemyDataTable = 
	{
		--{HP, Accel, Range RoF, Range Atk,
		--Melee RoF, Melee Atk, Enemy type}
		
		{8000,  23, 0.3, 150, 0.5, 2000, GetGreenID()},
		{12000, 20, 0.3, 150, 0.5, 2500, GetRedID()},
		{12000, 20, 1.0, 0,   0.5, 3000, GetBlueID()},
		{10000, 28, 0.3, 250, 0.5, 2000, GetYellowID()},
		{10000, 28, 0.3, 500, 0.5, 0,    GetGreyID()},
		{10000, 28, 0.4, 400, 0.5, 2500, GetWhiteID()},
		{1000,  40, 0.3, 10,  0.1, 100,  GetDroneID()},
	};
	
	--	Load all the enemy pairs
	for nIndex, value in pairs(EnemyDataTable) do
		LoadEnemyPack(EnemyDataTable[nIndex]);
	end
	
end

--	Return the color Enemy ID
function GetGreenID()	return 0;	end
function GetRedID()		return 1;	end
function GetBlueID()	return 2;	end
function GetYellowID()	return 3;	end
function GetGreyID()	return 4;	end
function GetWhiteID()	return 5;	end
function GetDroneID()	return 6;	end