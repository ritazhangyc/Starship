#pragma once
#include "Game/Gamecommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Camera.hpp"

class PlayerShip;
class Bullet;
class Asteroid;
class Debris;
class Beetle;
class Wasp;
class Entity;
class Clock;

class Game 
{
public:

	PlayerShip*		m_PlayerShip = nullptr; 
	Asteroid*		m_Asteroids[MAX_ASTEROIDS] = {}; 
	Bullet*			m_Bullets[MAX_BULLETS] = {}; 
	Beetle*			m_Beetles[MAX_BEETLES] = {};
	Wasp*			m_Wasps[MAX_WASPS] = {};
	Debris*			m_Debris[MAX_DEBRIS] = {};

	PlayerShip* GetPlayerShip() const;

public:
	Game();
	~Game();

	void Update();
	void Render();
	void UpdateEntities();
	
	void SpawnAsteroids(Vec2 const& position, float OrientationDegrees);
	void SpawnBullets(Vec2 const&position, float OrientationDegrees);
	void SpawnDebris(Vec2 const& position, Vec2 const& velocity, float OrientationDegrees, float physicsRadius, Rgba8 color);
	void SpawnDebrisCluster(int numDebris, Vec2 const& position, Vec2 const& velocity, float OrientationDegrees, float physicsRadius, Rgba8 color);
	void SpawnBeetles(Vec2 const& position, float OrientationDegrees);
	void SpawnWasps(Vec2 const& position, float OrientationDegrees);

	void CheckBulletsVsEnemy();
	void CheckBulletVsEnemy(Bullet& bullet, Entity& enemy);
	void CheckPlayershipVsEnemies();

	bool AreAllHostilesDestroyed() const;
	void SpawnNewWave();

	static bool SetEnteredTimeScale(EventArgs& args);

	Clock* m_gameClock;
private:
	void AdjustForPauseAndTimeDistortion();
	void RenderEntities();
	void RenderUI(int extraIcon);
	void DebugRender() const;
	void DeleteGarbageEntities();
	Vec2 GetPositionJustOffScreen(float cradius);

	bool m_isDebugging = false;
	int m_waveNumber = 0;

	float m_timeUntilAttractMode = 3.0f;

private:
	Camera m_gameCamera;	

	enum GameAction
	{
		MOVE_FORWARD,
		MOVE_BACKWARD,
		SPEED_UP,
		TURN_LEFT,
		TURN_RIGHT,
		FIRE_BULLET,
	};
};




