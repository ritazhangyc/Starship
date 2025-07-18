#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Bullet.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Beetle.hpp"
#include "Game/Wasp.hpp"
#include "Game/Debris.hpp"
#include "Game/Entity.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"

RandomNumberGenerator g_RNG;

Game::Game() 
{
    m_gameClock = new Clock();

    Vec2 worldCenter(WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f);
    m_gameCamera.SetOrthoView(Vec2(0.0f, 0.0f), Vec2(200.0f, 100.0f));
    m_PlayerShip = new PlayerShip(this, Vec2(WORLD_CENTER_X, WORLD_CENTER_Y), 0.f);
    for (int i = 0; i < 4; i++)
    {
        Vec2 randomPos = GetPositionJustOffScreen(ASTEROID_COSMETIC_RADIUS);
        float randomOrientation = g_RNG.RollRandomFloatInRange(0.0f, 360.0f);
        SpawnAsteroids(randomPos, randomOrientation);
    }
	for (int i = 0; i < 1; i++)
	{
        Vec2 randomPos = GetPositionJustOffScreen(BEETLE_COSMETIC_RADIUS);		
		float randomOrientation = g_RNG.RollRandomFloatInRange(0.0f, 360.0f);
		SpawnBeetles(randomPos, randomOrientation);
	}

    g_theEventSystem->SubscribeEventCallbackFunction("ChangeTimeScale", Game::SetEnteredTimeScale);
}
   

Game::~Game() {
    delete m_PlayerShip;
    m_PlayerShip = nullptr;
}

PlayerShip* Game::GetPlayerShip() const
{
	return m_PlayerShip; 
}

void Game::Update()
{
    AdjustForPauseAndTimeDistortion();
    
    if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
    {
        m_isDebugging = !m_isDebugging;
    }

	if (AreAllHostilesDestroyed())
	{
		SpawnNewWave(); 
	}

	if (g_theInput->WasKeyJustPressed('N'))
	{
		if (!m_PlayerShip->IsAlive() && m_PlayerShip->m_extraLives > 0)
		{
			SoundID respawnSound = g_theAudio->CreateOrGetSound("Data/Audio/PlayerRespawn.wav");
			g_theAudio->StartSound(respawnSound);
			m_PlayerShip->Rebirth();
		}
	}
	if (m_PlayerShip->m_extraLives <= 0 && !m_PlayerShip->IsAlive())
	{
        m_timeUntilAttractMode -= static_cast<float>(m_gameClock->GetDeltaSeconds());
        if (m_timeUntilAttractMode <= 0.f)
        {
            g_theApp->IsInAttractMode = true;
            m_timeUntilAttractMode = 3.0f;
        }
	}

    UpdateEntities();
    CheckBulletsVsEnemy();
    CheckPlayershipVsEnemies();
    DeleteGarbageEntities(); 
}

void Game::UpdateEntities()
{
    float deltaseconds = static_cast<float>(m_gameClock->GetDeltaSeconds());

    if (m_PlayerShip != nullptr)
    {
        m_PlayerShip->Update(deltaseconds);
    }    
    for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
    {
        if (m_Bullets[bulletIndex] != nullptr)
        {
            m_Bullets[bulletIndex]->Update(deltaseconds);
        }
    }
    for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
    {
        if (m_Asteroids[asteroidIndex] != nullptr)
        {
            m_Asteroids[asteroidIndex]->Update(deltaseconds);
        }
    }
    for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
    {
        if (m_Debris[debrisIndex] != nullptr)
        {
            m_Debris[debrisIndex]->Update(deltaseconds);
        }
    }
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		if (m_Beetles[beetleIndex] != nullptr)
		{
			m_Beetles[beetleIndex]->Update(deltaseconds);
		}
	}
	for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		if (m_Wasps[waspIndex] != nullptr)
		{
            m_Wasps[waspIndex]->Update(deltaseconds);
		}
	}

}

void Game::Render()
{
    g_theRenderer->BeginCamera(m_gameCamera);
    RenderEntities();
    if (m_isDebugging == true)
    {
        DebugRender();
    }
    int extraIcon = m_PlayerShip->m_extraLives;
    if (extraIcon > 0)
    {
        RenderUI(extraIcon);
    }
    g_theRenderer->EndCamera(m_gameCamera);
}


void Game::SpawnAsteroids(Vec2 const& position, float OrientationDegrees)
{
    for (int numAsteroids=0; numAsteroids < MAX_ASTEROIDS; ++numAsteroids) {

        Asteroid* newAsteroid = new Asteroid(this, position, OrientationDegrees);
        if (m_Asteroids[numAsteroids] == nullptr)
        {
            m_Asteroids[numAsteroids] = newAsteroid;
            return;
        }
    }
    ERROR_RECOVERABLE("CANNOT SPAWN MORE ASTEROIDS; ALL SLOTS ARE FULL!")
}

void Game::SpawnDebris(Vec2 const& position, Vec2 const& velocity, float OrientationDegrees, float physicsRadius, Rgba8 color)
{
    for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
    {
        Debris* newDebris = new Debris(this, position, velocity, OrientationDegrees, physicsRadius, color);
        if (m_Debris[debrisIndex] == nullptr)
        {
            newDebris->m_color = color;                            
            m_Debris[debrisIndex] = newDebris;
            return;
        }
    }
}

void Game::SpawnDebrisCluster(int numDebris, Vec2 const& position, Vec2 const& velocity, float OrientationDegrees, float physicsRadius, Rgba8 color)
{
    for (int i = 0; i < numDebris; ++i)
    {
		float randomAngle = g_RNG.RollRandomFloatInRange(0, 180);
		Vec2 scatterDirection = Vec2::MakeFromPolarDegrees(randomAngle, 1.0f);

		float scatterScale = g_RNG.RollRandomFloatInRange(0.5f, 2.0f); 
		Vec2 scatterVelocity = scatterDirection * scatterScale * DEBRIS_FIXED_SPEED;
		Vec2 debrisVelocity = velocity + scatterVelocity;
        SpawnDebris(position, debrisVelocity, OrientationDegrees, physicsRadius, color);
    }

}


void Game::SpawnBeetles(Vec2 const& position, float OrientationDegrees)
{
	for (int numBeetles = 0; numBeetles < MAX_BEETLES; ++numBeetles)
    {
		Beetle* newBeetle = new Beetle(this, position, OrientationDegrees);
		if (m_Beetles[numBeetles] == nullptr)
		{
            m_Beetles[numBeetles] = newBeetle;
			return;
		}
	}
}


void Game::SpawnWasps(Vec2 const& position, float OrientationDegrees)
{
	for (int numWasps = 0; numWasps < MAX_WASPS; ++numWasps)
	{
		Wasp* newWasp = new Wasp(this, position, OrientationDegrees);
		if (m_Wasps[numWasps] == nullptr)
		{
			m_Wasps[numWasps] = newWasp;
			return;
		}
	}
}

void Game::SpawnBullets(Vec2 const& position, float OrientationDegrees)
{
    Bullet* newBullet = new Bullet(this, position, OrientationDegrees);
    for (int i = 0; i < MAX_BULLETS; ++i)
    {
        if (m_Bullets[i] == nullptr)
        {
            m_Bullets[i] = newBullet;
            return;
        }
    }
    ERROR_RECOVERABLE("CANNOT SPAWN MORE BULLETS; ALL SLOTS ARE FULL!")
}

void Game::CheckBulletsVsEnemy()
{
    for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
    {
        Bullet* bullet = m_Bullets[bulletIndex];
        if (!bullet || !bullet->IsAlive())
        {
            continue;
        }
        for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
        {
            Asteroid* asteroid = m_Asteroids[asteroidIndex];
            if (asteroid && asteroid->IsAlive())
            {
                CheckBulletVsEnemy(*bullet, *asteroid);
            }
        }
        for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
        {
            Beetle* beetle = m_Beetles[beetleIndex];
            if (beetle && beetle->IsAlive())
            {
                CheckBulletVsEnemy(*bullet, *beetle);
            }
        }
        for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
        {
            Wasp* wasp = m_Wasps[waspIndex];
            if (wasp && wasp->IsAlive())
            {
                CheckBulletVsEnemy(*bullet, *wasp);
            }
        }
    }
}

void Game::CheckBulletVsEnemy(Bullet& bullet, Entity& enemy)
{
    if (DoDiscsOverlap(bullet.GetPosition(), bullet.GetPD(), enemy.GetPosition(), ASTEROID_PHYSICS_RADIUS) == true)
    {
        bullet.Hit();
        enemy.Hit();
    }
}

void Game::CheckPlayershipVsEnemies()
{
    if (m_PlayerShip == nullptr || !m_PlayerShip->IsAlive())
    {
        return;
    }

    for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
    {
        Asteroid* asteroid = m_Asteroids[asteroidIndex];
        if (asteroid == nullptr || !asteroid->IsAlive())
        {
            continue; 
        }

        if (DoDiscsOverlap(m_PlayerShip->GetPosition(), PLAYER_SHIP_PHYSICS_RADIUS, asteroid->GetPosition(), ASTEROID_PHYSICS_RADIUS))
        {
            m_PlayerShip->Hit();
            asteroid->Hit();
        }
    }
    for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
    {
        Beetle* beetle = m_Beetles[beetleIndex];
        if (beetle == nullptr|| !beetle->IsAlive())
        {
            continue;
        }
        if (DoDiscsOverlap(m_PlayerShip->GetPosition(), PLAYER_SHIP_PHYSICS_RADIUS, beetle->GetPosition(), beetle->GetPD()))
        {
            m_PlayerShip->Hit();
            beetle->Hit();
        }
    }
    for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
    {
        Wasp* wasp = m_Wasps[waspIndex];
        if (wasp == nullptr || !wasp->IsAlive())
        {
            continue;
        }
        if (DoDiscsOverlap(m_PlayerShip->GetPosition(), PLAYER_SHIP_PHYSICS_RADIUS, wasp->GetPosition(), wasp->GetPD()))
        {
            m_PlayerShip->Hit();
            wasp->Hit();
        }
    }
}


void Game::AdjustForPauseAndTimeDistortion()
{
	if (g_theInput->WasKeyJustPressed('P'))
	{
		m_gameClock->TogglePause();
	}

	if (g_theInput->WasKeyJustPressed('O'))
	{
		m_gameClock->StepSingleFrame();
	}

	if (g_theInput->WasKeyJustPressed('T'))
	{
		if (m_gameClock->GetTimeScale() == 0.1f)
		{
			m_gameClock->SetTimeScale(1.0f);
		}
		else
		{
			m_gameClock->SetTimeScale(0.1f);
		}
	}
}

void Game::RenderEntities()
{
    if (m_PlayerShip != nullptr && m_PlayerShip->IsAlive())
    {
        m_PlayerShip->Render();
    }

    for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
    {
        if (m_Bullets[bulletIndex] != nullptr) {
            m_Bullets[bulletIndex]->Render();
        }     
    }

    for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
    {
        if (m_Asteroids[asteroidIndex] != nullptr) {
            m_Asteroids[asteroidIndex]->Render();
        }
    }

    for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
    {
        if (m_Debris[debrisIndex] != nullptr) {
            m_Debris[debrisIndex]->Render();
        }
    }
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		if (m_Beetles[beetleIndex] != nullptr) {
			m_Beetles[beetleIndex]->Render();
		}
	}
	for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		if (m_Wasps[waspIndex] != nullptr) {
			m_Wasps[waspIndex]->Render();
		}
	}
}

void Game::RenderUI(int extraIcon)
{
    for (int i = 0; i < extraIcon; i++)
    {
		Vertex_PCU translucentRedFakeShip[15];
		PlayerShip::InitializeVerts(&translucentRedFakeShip[0], Rgba8(255, 0, 0, 127));
		Vec2 iconPosition = Vec2(5, 95);
        iconPosition += Vec2( 5.f * static_cast<float>(i), 0);
		TransformVertexArrayXY3D(NUM_SHIP_VERTS, &translucentRedFakeShip[0], 1.f, 90.f, iconPosition);
		g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, translucentRedFakeShip);
    }
	
}

void Game::DebugRender() const
{
    if (m_PlayerShip != nullptr) 
    {
        m_PlayerShip->DebugRender();
    }
    for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
    {
        Bullet const* bullet = m_Bullets[bulletIndex];
        if (bullet)
        {
            bullet->DebugRender();
            DebugDrawLine(bullet->GetPosition(), m_PlayerShip->GetPosition(), Rgba8(50, 50, 50), 0.2f);
        }
    }
    for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
    {
        Asteroid const* asteroid = m_Asteroids[asteroidIndex];
        if (asteroid)
        {
            asteroid->DebugRender();
            DebugDrawLine(asteroid->GetPosition(), m_PlayerShip->GetPosition(), Rgba8(50, 50, 50), 0.2f);
        }
    }
    for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
    {
        Debris const* debris = m_Debris[debrisIndex];
        if (debris)
        {
            debris->DebugRender();
            DebugDrawLine(debris->GetPosition(), m_PlayerShip->GetPosition(), Rgba8(50, 50, 50), 0.2f);
        }
    }
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle const* beetle = m_Beetles[beetleIndex];
		if (beetle)
		{
            beetle->DebugRender();
			DebugDrawLine(beetle->GetPosition(), m_PlayerShip->GetPosition(), Rgba8(50, 50, 50), 0.2f);
		}
	}
    for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		Wasp const* wasp = m_Wasps[waspIndex];
		if (wasp)
		{
            wasp->DebugRender();
			DebugDrawLine(wasp->GetPosition(), m_PlayerShip->GetPosition(), Rgba8(50, 50, 50), 0.2f);
		}
	}
}

void Game::DeleteGarbageEntities()
{
    for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
    {
        if (m_Bullets[bulletIndex] == nullptr)
        {
            continue;
        }
        
        if (m_Bullets[bulletIndex]->IsGarbage())
        {
            delete m_Bullets[bulletIndex];
            m_Bullets[bulletIndex] = nullptr;
        }
    }
    for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
    {
        if (m_Asteroids[asteroidIndex] == nullptr)
        {
            continue;
        }
        
        if (m_Asteroids[asteroidIndex]->IsGarbage())
        {
            delete m_Asteroids[asteroidIndex];
            m_Asteroids[asteroidIndex] = nullptr;
        }
    }
    for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
    {
        if (m_Debris[debrisIndex] == nullptr)
        {
            continue;
        }
        if (m_Debris[debrisIndex]->IsGarbage())
        {
            delete m_Debris[debrisIndex];
            m_Debris[debrisIndex] = 0;
        }
    }
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		if (m_Beetles[beetleIndex] == nullptr)
		{
			continue;
		}
		if (m_Beetles[beetleIndex]->IsGarbage())
		{
			delete m_Beetles[beetleIndex];
            m_Beetles[beetleIndex] = 0;
		}
	}
	for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		if (m_Wasps[waspIndex] == nullptr)
		{
			continue;
		}
		if (m_Wasps[waspIndex]->IsGarbage())
		{
			delete m_Wasps[waspIndex];
            m_Wasps[waspIndex] = 0;
		}
	}
}


Vec2 Game::GetPositionJustOffScreen(float cradius)
{
	Vec2 randomPos;
	int edge = g_RNG.RollRandomIntInRange(1,4);

    if (edge == 1)
    {   // Top edge
        randomPos.x = g_RNG.RollRandomFloatInRange(0.f, WORLD_SIZE_X);
        randomPos.y = WORLD_SIZE_Y + cradius;
        return Vec2(randomPos);
    }
	if (edge == 2)
    {   // Right edge
		randomPos.x = WORLD_SIZE_X + cradius;
		randomPos.y = g_RNG.RollRandomFloatInRange(0.f, WORLD_SIZE_Y);
        return Vec2(randomPos);
    }
	if (edge == 3)
    {   // Bottom edge
		randomPos.x = g_RNG.RollRandomFloatInRange(0.f, WORLD_SIZE_X);
		randomPos.y = -cradius;
        return Vec2(randomPos);
    }
    else
    {   // Left edge
		randomPos.x = -cradius;
		randomPos.y = g_RNG.RollRandomFloatInRange(0.f, WORLD_SIZE_Y);
        return Vec2(randomPos);
	}
}


bool Game::AreAllHostilesDestroyed() const
{
    for (int i = 0; i < MAX_WASPS; ++i)
    {
        if (m_Wasps[i] && m_Wasps[i]->IsAlive())
        return false;
    }

	for (int i = 0; i < MAX_BEETLES; ++i)
	{
		if (m_Beetles[i] && m_Beetles[i]->IsAlive())
			return false;
	}

    for (int i = 0; i < MAX_ASTEROIDS; ++i)
    {
        if (m_Asteroids[i] && m_Asteroids[i]->IsAlive())
            return false;
    }

	return true; 
}

void Game::SpawnNewWave()
{
    float deltaseconds = static_cast<float>(m_gameClock->GetDeltaSeconds());

    m_waveNumber++;
	if (m_waveNumber >= 5 && m_PlayerShip->IsAlive())
	{
		m_timeUntilAttractMode -= deltaseconds;
		if (m_timeUntilAttractMode <= 0.f)
		{
			g_theApp->IsInAttractMode = true;
			m_timeUntilAttractMode = 3.0f;
		}
	}
    else 
    {
		int numNewBeetles = 1 + m_waveNumber;
		int numNewAsteroids = 4 + m_waveNumber * 2;
		int numNewWasps = 1 + m_waveNumber;

		for (int i = 0; i < numNewBeetles; ++i)
		{
			Vec2 position = GetPositionJustOffScreen(BEETLE_COSMETIC_RADIUS);
			float orientationDegrees = g_RNG.RollRandomFloatInRange(0.f, 360.f);
			SpawnBeetles(position, orientationDegrees);
		}

		for (int i = 0; i < numNewAsteroids; ++i)
		{
			Vec2 position = GetPositionJustOffScreen(ASTEROID_COSMETIC_RADIUS);
			float orientationDegrees = g_RNG.RollRandomFloatInRange(0.f, 360.f);
			SpawnAsteroids(position, orientationDegrees);
		}

		for (int i = 0; i < numNewWasps; ++i)
		{
			Vec2 position = GetPositionJustOffScreen(WASP_COSMETIC_RADIUS);
			float orientationDegrees = g_RNG.RollRandomFloatInRange(0.f, 360.f);
			SpawnWasps(position, orientationDegrees);
		}
    }
	
}

bool Game::SetEnteredTimeScale(EventArgs& args)
{
    float timeScale = args.GetValue("timeScale", 1.0f);
    g_theApp->m_game->m_gameClock->SetTimeScale(timeScale);
    return false;
}