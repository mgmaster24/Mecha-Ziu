LoadLuaLib("all");
LoadGameLib("objectSys");

module("Squad", package.seeall);

-- Function for creating a squad
-- Only Valid when enemies are loaded
function CreateSquad(nSquadSize, nSquadID, nSquadStr, fX,fY,fZ)

	--	The Number of Squad Members actually created
	local nNumActualCreated = 0;

	-- Check for a valid squad size
	if nSquadSize < 0 then
		return nNumActualCreated;
	end
	
	-- Get the enemy Model
	local nModelID = GetEnemyModel(0);
	local nSquadReSize = nSquadSize;

	local nTeamItter = 0;
	local nNoLeader = 0;
	local nFinalPos = 0;

	-- Itterate through all the teams and create the squads
	while nSquadReSize > 0 do

		-- Get the Current Squad Size
		local nNewSquadSize = nSquadReSize;
		local nActualSquadID = 0;

		-- Check that we are in bounds
		if nNewSquadSize > 8 then
			nNewSquadSize = 8;
		end

		--print("Squad "..nTeamItter);
		--print("Squad Size "..nNewSquadSize);
		
		nTeamItter = nTeamItter + 1;

		-- Check if a leader should be created
		if nNewSquadSize > 2 then
		
			-- Create a new squad leader
			nActualSquadID = CreateSquadEnemy(nModelID, nSquadStr, fX + nFinalPos,fY,fZ, nSquadID, 1);
			
			--	Check that the enemy was actually created
			if nActualSquadID == -2 then
				break;
			end
			
			--	Incriment the number of created enemies
			nNumActualCreated = nNumActualCreated + 1;
			
			--	Adjust the squad size counter
			nNewSquadSize = nNewSquadSize - 1;
			nSquadReSize = nSquadReSize - 1;
			nNoLeader = 0;
			
			--print("Create Leader");
		elseif nNewSquadSize <= 2 then
		
			-- Set an invalid squad id
			nSquadID = -1;
			nNoLeader = 10; -- If not leader, wait towards the back
			--print("No Leader");
			
		end
		
		--print("Squad Size "..nNewSquadSize);
		
		-- Divide the Squad across the x
		local nX = -1 * nNewSquadSize * (nTeamItter * 2);

		-- Itterate and create the squad
		for nItter = 1, nNewSquadSize, 1 do

			--print("Create"..nItter);
			
			-- Find the X possition
			nFinalPos = nX + nItter;
			nFinalPos = fX + nFinalPos;

			--	Check that the enemy type is valid
			if nSquadStr > 5 then
				nSquadStr = 1;
			end
		
			-- Create the squad enemy
			nActualSquadID = CreateSquadEnemy(nModelID, nSquadStr, nFinalPos, fY, fZ + nNoLeader, nActualSquadID, 0);
			
			--	Check that the enemy was actually created
			if nActualSquadID == -2 then
				break;
			end
			
			--	Incriment the number of created enemies
			nNumActualCreated = nNumActualCreated + 1;

			-- Set data for next spawn
			nSquadStr = nSquadStr + 1;
		end
		
		--	Check that the enemy was actually created
		if nActualSquadID == -2 then
			break;
		end

		-- Negate the team size
		nSquadReSize = nSquadReSize - nNewSquadSize;
		nSquadID = nSquadID + 1;
	end
	
	--	Return the actual number of enemies spawned
	return nNumActualCreated;

end