-- SH_Particles - Particle Pak Data
module("Particle", package.seeall);

--	Load in the particle data
LoadGameLib("particle");

-- Load the object Library
LoadGameLib("objectSys");

--	Global start value for partice init
local parInit = -1;

function LoadParticles()

	--	Load the particle Files
	parInit = LoadParticle("./Assets/Particles/SH_Bullet.lua");
	LoadParticle("./Assets/Particles/SH_EnemyBullet.lua");
	LoadParticle("./Assets/Particles/SH_Turret.lua");
	LoadParticle("./Assets/Particles/SH_Mine.lua");
	LoadParticle("./Assets/Particles/SH_MineExplosion.lua");
	LoadParticle("./Assets/Particles/SH_ChargeTurtle.lua");
	LoadParticle("./Assets/Particles/SH_EndChargeTurtle.lua");
	LoadParticle("./Assets/Particles/SH_TurtleNoShieldHit.lua");
	LoadParticle("./Assets/Particles/SH_TurtleShieldHit.lua");
	LoadParticle("./Assets/Particles/SH_TurtleHit.lua");
	LoadParticle("./Assets/Particles/SH_TurtleExplosion.lua");
	LoadParticle("./Assets/Particles/SH_TurtleCannonExplosion.lua");
	LoadParticle("./Assets/Particles/SH_Teleport.lua");
	LoadParticle("./Assets/Particles/SH_Explosion.lua");
	LoadParticle("./Assets/Particles/SH_Hit.lua");
	LoadParticle("./Assets/Particles/SH_PlayerThruster.lua");
	LoadParticle("./Assets/Particles/SH_Charge.lua");
	LoadParticle("./Assets/Particles/SH_EndCharge.lua");
	LoadParticle("./Assets/Particles/SH_PlayerHit.lua");
	LoadParticle("./Assets/Particles/SH_Health.lua");
	LoadParticle("./Assets/Particles/SH_Armor.lua");
	LoadParticle("./Assets/Particles/SH_Missiles.lua");
	LoadParticle("./Assets/Particles/SH_MissileSmoke.lua");
	LoadParticle("./Assets/Particles/SH_Pickup.lua");
	LoadParticle("./Assets/Particles/SH_Firewall_Down.lua");
	LoadParticle("./Assets/Particles/SH_Firewall_Up.lua");
	LoadParticle("./Assets/Particles/SH_Firewall_Left.lua");
	LoadParticle("./Assets/Particles/SH_Firewall_Right.lua");
	LoadParticle("./Assets/Particles/SH_Hit_Talbot.lua");
	LoadParticle("./Assets/Particles/SH_MicroExplosion.lua");

end

function AddParticle(gameObject, particleIndex)

	ApplyParticle(gameObject, particleIndex);

end

-- Return Particle ID Types
function ParBullet()				return parInit;			end
function ParEnemyBullet()			return parInit + 1;		end
function ParTurret()				return parInit + 2;		end
function ParMine()				return parInit + 3;		end
function ParSheild()				return parInit + 4;		end
function ParPlayerThruster()			return parInit + 5;		end
function ParCharge()				return parInit + 6;		end
function ParPlayerHit()				return parInit + 7;		end
