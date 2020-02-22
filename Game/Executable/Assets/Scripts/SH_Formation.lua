-- SH_Formations - Formations available to the game
module("Formation", package.seeall);

-- Load the AI Library
LoadGameLib("aiSys");

--	Load the formation data
function LoadFormationsLib()
	
	--	Balanced Formations
	--	Point Formation
	local formationTable = 
	{
		{ -10, 0, 0, 8 }, { 10, 0, 0, -8 }, { 25, 8, 0, 0 }, { 25, -8, 0, 0 },
	};
	BuildFormation(formationTable, 10, 10);
	
	--	Axe Formation
	formationTable = 
	{
		{ 50, 0, 1.5, 8 }, { 50, 0, 1.5, -8 }, { 50, 8, 1.5, 0 }, { 50, -8, 1.5, 0 },
		{ 50, 13, 0, 0 }, { 50, -13, 0, 0 }, { 50, 0, 1.5, -13 }, { 50, 0, 1.5, 13 },
	};
	BuildFormation(formationTable, 15, 15);
	
	--	Wedge Formation
	formationTable = 
	{
		{ 15, 13, 0, 0 }, { 15, -13, 0, 0 }, { -50, 0, 0, 8 }, { 15, 0, 0, -8 },
		{ 15, 8, 0, 8 }, { 15, -8, 0, 8 },
	};
	BuildFormation(formationTable, 50, 15);
	
	--	Target Strike
	formationTable = 
	{
		{ -5, 12, 0, 0 }, { -5,-12, 0, 0 }, { -5, 0, 0, 12 }, { -5, 0, 0,-12 },
		{ -5, 12, 0, 12 }, { -5,-12, 0, 12 }, { -5, 12, 0, 12 }, { -5,-12, 0, 12 },
		{ 5, 7, 1.5, 0 }, { 5,-7, 1.5, 0 }, { 5, 0, 1.5, 7 }, { 5, 0, 0,-7 },
		{ 5, 7,-1.5, 7 }, { 5,-7,-1.5, 7 }, { 5, 7,-1.5, 7 }, { 5,-7,-1.5, 7 },
	};
	BuildFormation(formationTable, 666, 14);

end

--	Build the formation using the formation table
function BuildFormation(formTable, formValue, formRadius)
	
	--	Create the formation
	local formID = CreateFormation(formValue, formRadius);
	
	--	Load the formation table data
	for nIndex, value in pairs(formTable) do
		AddFormationTable(formTable[nIndex], formID);
	end
	
end