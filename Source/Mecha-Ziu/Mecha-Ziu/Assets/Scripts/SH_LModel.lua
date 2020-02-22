-- A interface for loading the default model data
LoadGameLib("renderSys");

module("LModel", package.seeall);

--	Load all standard models into memory
function LoadModels( szSkyBoxModel )

	--	Set the Skybox model
	SetSkyBoxModel(LoadModel(szSkyBoxModel));

	--	Set the player model
	SetPlayerModel(LoadModel("./Assets/Models/TestModel/MechaZiu.model"));

	--	Load the enemy model
	SetEnemyModel(0, LoadModel("./Assets/Models/TestModel/Acerlen.model"));

	--	Load the bullets
	--	Energy Shot
	SetBulletModel(LoadModel("./Assets/Models/TestModel/BulletQuad.Model"), 0);

	--	Charge Shot
	SetBulletModel(LoadModel("./Assets/Models/TestModel/charged_cylinder.Model"), 1);

	--	Bullet (Head Turret)
	SetBulletModel(LoadModel("./Assets/Models/TestModel/turret_bullet.Model"), 2);

	--	Missile
	SetBulletModel(LoadModel("./Assets/Models/TestModel/bullet.Model"), 3);
	
	--	Mine
	SetBulletModel(LoadModel("./Assets/Models/TestModel/spacemine.Model"), 4);

	--	Set the power-up model
	--	Health
	SetPowerUpModel(LoadModel("./Assets/Models/TestModel/healthPowerUp.Model"), 0);

	--	Armor
	SetPowerUpModel(LoadModel("./Assets/Models/TestModel/shieldPowerUp.Model"), 1);

	--	Missiles
	SetPowerUpModel(LoadModel("./Assets/Models/TestModel/missilePowerUp.Model"), 2);
end