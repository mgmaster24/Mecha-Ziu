LoadLuaLib("all");
LoadGameLib("aiSys");

module("AI", package.seeall);

local function Level1(difficulty)

	-- Set the AI Values
	if difficulty == 0 then
		SetAIValue("MaxAttackDist", 5000.0);
		SetAIValue("AngryDist", 3000.0);
		SetAIValue("AngryTime", 7.0);
		SetAIValue("IdleTime", 10.0);
		SetAIValue("EnemyCutOff", 200.0);
		SetAIValue("Difficulty", 5.0);
		SetAIValue("EnemyFar", 2000.0);
		SetAIValue("EnemyNear", 1000.0);
		SetAIValue("MaxLeaders", 4.0);
		SetAIValue("PatrolDistance", 4000.0);
		SetAIValue("MeleeOffset", 6.0);
		SetAIValue("RangeOffset", 15.0);
		SetAIValue("MaxYAdjust", 15.0);
		SetAIValue("MaxYDist", 2.0);
		
	elseif difficulty == 1 then
		SetAIValue("MaxAttackDist", 6000.0);
		SetAIValue("AngryDist", 7000.0);
		SetAIValue("AngryTime", 10.0);
		SetAIValue("IdleTime", 5.0);
		SetAIValue("EnemyCutOff", 200.0);
		SetAIValue("Difficulty", 3.0);
		SetAIValue("EnemyFar", 1500.0);
		SetAIValue("EnemyNear", 800.0);
		SetAIValue("MaxLeaders", 10.0);
		SetAIValue("PatrolDistance", 8000.0);
		SetAIValue("MeleeOffset", 3.0);
		SetAIValue("RangeOffset", 15.0);
		SetAIValue("MaxYAdjust", 15.0);
		SetAIValue("MaxYDist", 1.5);
		
	elseif difficulty == 2 then
		SetAIValue("MaxAttackDist", 7000.0);
		SetAIValue("AngryDist", 8000.0);
		SetAIValue("AngryTime", 20.0);
		SetAIValue("IdleTime", 3.0);
		SetAIValue("EnemyCutOff", 200.0);
		SetAIValue("Difficulty", 1.0);
		SetAIValue("EnemyFar", 1600.0);
		SetAIValue("EnemyNear", 800.0);
		SetAIValue("MaxLeaders", 16.0);
		SetAIValue("PatrolDistance", 10000.0);
		SetAIValue("MeleeOffset", 1.5);
		SetAIValue("RangeOffset", 18.0);
		SetAIValue("MaxYAdjust", 15.0);
		SetAIValue("MaxYDist", 1.0);
	end
	
end

local function Level2(difficulty)

	-- Set the AI Values
	if difficulty == 0 then
		SetAIValue("MaxAttackDist", 6000.0);
		SetAIValue("AngryDist", 7000.0);
		SetAIValue("AngryTime", 7.0);
		SetAIValue("IdleTime", 1.0);
		SetAIValue("EnemyCutOff", 200.0);
		SetAIValue("Difficulty", 5.0);
		SetAIValue("EnemyFar", 2000.0);
		SetAIValue("EnemyNear", 1400.0);
		SetAIValue("MaxLeaders", 16.0);
		SetAIValue("PatrolDistance", 10000.0);
		SetAIValue("MeleeOffset", 3.0);
		SetAIValue("RangeOffset", 18.0);
		SetAIValue("MaxYAdjust", 15.0);
		SetAIValue("MaxYDist", 1.0);
		
	elseif difficulty == 1 then
		SetAIValue("MaxAttackDist", 6000.0);
		SetAIValue("AngryDist", 7000.0);
		SetAIValue("AngryTime", 7.0);
		SetAIValue("IdleTime", 1.0);
		SetAIValue("EnemyCutOff", 200.0);
		SetAIValue("Difficulty", 3.0);
		SetAIValue("EnemyFar", 2000.0);
		SetAIValue("EnemyNear", 1400.0);
		SetAIValue("MaxLeaders", 16.0);
		SetAIValue("PatrolDistance", 10000.0);
		SetAIValue("MeleeOffset", 3.0);
		SetAIValue("RangeOffset", 18.0);
		SetAIValue("MaxYAdjust", 15.0);
		SetAIValue("MaxYDist", 1.0);
		
	elseif difficulty == 2 then
		SetAIValue("MaxAttackDist", 6000.0);
		SetAIValue("AngryDist", 7000.0);
		SetAIValue("AngryTime", 7.0);
		SetAIValue("IdleTime", 1.0);
		SetAIValue("EnemyCutOff", 200.0);
		SetAIValue("Difficulty", 1.5);
		SetAIValue("EnemyFar", 2000.0);
		SetAIValue("EnemyNear", 1400.0);
		SetAIValue("MaxLeaders", 16.0);
		SetAIValue("PatrolDistance", 10000.0);
		SetAIValue("MeleeOffset", 3.0);
		SetAIValue("RangeOffset", 18.0);
		SetAIValue("MaxYAdjust", 15.0);
		SetAIValue("MaxYDist", 1.0);
	end

end

local function Level3(difficulty)

	-- Set the AI Values
	if difficulty == -1 then
	
		SetAIValue("MaxAttackDist", 6000.0);
		SetAIValue("AngryDist", 7000.0);
		SetAIValue("AngryTime", 10.0);
		SetAIValue("IdleTime", 1.0);
		SetAIValue("EnemyCutOff", 200.0);
		SetAIValue("Difficulty", 1.5);
		SetAIValue("EnemyFar", 1500.0);
		SetAIValue("EnemyNear", 800.0);
		SetAIValue("MaxLeaders", 2.0);
		SetAIValue("PatrolDistance", 8000.0);
		SetAIValue("MeleeOffset", 3.0);
		SetAIValue("RangeOffset", 15.0);
		SetAIValue("MaxYAdjust", 15.0);
		SetAIValue("MaxYDist", 1.5);
	
	elseif difficulty == 0 then
		SetAIValue("MaxAttackDist", 2000.0);
		SetAIValue("AngryDist", 1400.0);
		SetAIValue("AngryTime", 7.0);
		SetAIValue("IdleTime", 5.0);
		SetAIValue("EnemyCutOff", 400.0);
		SetAIValue("Difficulty", 3.0);
		SetAIValue("EnemyFar", 2000.0);
		SetAIValue("EnemyNear", 1000.0);
		SetAIValue("PatrolDistance", 10000.0);
		SetAIValue("MeleeOffset", 6.0);
		SetAIValue("RangeOffset", 20.0);
		SetAIValue("MaxYAdjust", 15.0);
		SetAIValue("MaxYDist", 2.0);
		
	elseif difficulty == 1 then
		SetAIValue("MaxAttackDist", 2400.0);
		SetAIValue("AngryDist", 1800.0);
		SetAIValue("AngryTime", 10.0);
		SetAIValue("IdleTime", 3.0);
		SetAIValue("EnemyCutOff", 200.0);
		SetAIValue("Difficulty", 1.5);
		SetAIValue("EnemyFar", 1500.0);
		SetAIValue("EnemyNear", 800.0);
		SetAIValue("PatrolDistance", 10000.0);
		SetAIValue("MeleeOffset", 3.0);
		SetAIValue("RangeOffset", 15.0);
		SetAIValue("MaxYAdjust", 15.0);
		SetAIValue("MaxYDist", 1.5);
		
	elseif difficulty == 2 then
		SetAIValue("MaxAttackDist", 3000.0);
		SetAIValue("AngryDist", 2500.0);
		SetAIValue("AngryTime", 20.0);
		SetAIValue("IdleTime", 1.0);
		SetAIValue("EnemyCutOff", 200.0);
		SetAIValue("Difficulty", 0.1);
		SetAIValue("EnemyFar", 1500.0);
		SetAIValue("EnemyNear", 800.0);
		SetAIValue("PatrolDistance", 10000.0);
		SetAIValue("MeleeOffset", 1.0);
		SetAIValue("RangeOffset", 10.0);
		SetAIValue("MaxYAdjust", 15.0);
		SetAIValue("MaxYDist", 1.0);
	end

end

local function Survival(difficulty)

	-- Set the AI Values
	-- Insane mode
	
	SetAIValue("MaxAttackDist", 7000.0);
	SetAIValue("AngryDist", 9000.0);
	SetAIValue("AngryTime", 50.0);
	SetAIValue("IdleTime", 0.5);
	SetAIValue("EnemyCutOff", 200.0);
	SetAIValue("Difficulty", 1.3);
	SetAIValue("EnemyFar", 2000.0);
	SetAIValue("EnemyNear", 1400.0);
	SetAIValue("MaxLeaders", 16.0);
	SetAIValue("PatrolDistance", 10000.0);
	SetAIValue("MeleeOffset", 1.0);
	SetAIValue("RangeOffset", 19.0);
	SetAIValue("MaxYAdjust", 15.0);
	SetAIValue("MaxYDist", 0.8);

end

-- Init the AI data
function InitAIData(level, difficulty)

	-- Check the level
	if level == 1 then	
		Level1(difficulty)
	elseif level == 2 then	
		Level2(difficulty)
	elseif level == 3 then	
		Level3(difficulty)
	elseif level == 4 then
		Survival(difficulty)
	end
	
end