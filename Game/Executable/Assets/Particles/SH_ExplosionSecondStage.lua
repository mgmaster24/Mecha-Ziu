--Second part of the explosion
fLifeTime = 2.0;
fStartSizeX = 1.0;
fStartSizeY = 1.0;
fEndSizeX = 3.0;
fEndSizeY = 3.0;
nParticleAmount = 40;
nStartColorAl = 255;
nStartColorR = 150;
nStartColorG = 100;
nStartColorB = 30;
nEndColorAl = 0;
nEndColorR = 150;
nEndColorG = 20;
nEndColorB = 20;
fEmitterPosX = 0.0;
fEmitterPosY = 1.5;
fEmitterPosZ = -3.5;
bOn = 1;
bLoop = 0;
--If it will pause, this will be 1
bPause = 0;

--If there are multiple emitters this will be 1
bMultEmitters = 0;

--Default Particle
bRandX = 1;
fMinX = -3.0;
fMaxX = 3.0;
vVelocityX = 0.0;
bRandY = 1;
fMinY = -3.0;
fMaxY = 3.0;
vVelocityY = 0.0;
vVelocityZ = 0.0;

bRandom = 0;
fDecr = 0.0;
szPart = "./Assets/Particles/explosion.tga";