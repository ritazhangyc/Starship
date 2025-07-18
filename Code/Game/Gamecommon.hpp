#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"


//------------------------------------------------------------------------------------------------
// Forward class declarations, and global pointer extern declarations, so everyone everywhere
//	within Game code can see and use our global engine systems.
class App;
class InputSystem;
class Renderer;
class Window;

extern App*				g_theApp;
extern InputSystem*		g_theInput;
extern Renderer*		g_theRenderer;
extern AudioSystem*		g_theAudio;
extern Window*			g_theWindow;



void DebugDrawRing( Vec2 const& center, float radius, float thickness, Rgba8 const& color );
void DebugDrawLine( Vec2 const& start, Vec2 const& end, Rgba8 color, float thickness );


constexpr int		NUM_LINE_TRIS = 2;
constexpr int		NUM_LINE_VERTS = 3 * NUM_LINE_TRIS;

constexpr int		NUM_SHIP_TRIS = 5;
constexpr int		NUM_SHIP_VERTS = 3 * NUM_SHIP_TRIS;

constexpr int		NUM_STARTING_ASTEROIDS = 6;
constexpr int		MAX_ASTEROIDS = 50;
constexpr int		MAX_BULLETS = 200;
constexpr float		WORLD_SIZE_X = 200.f;
constexpr float		WORLD_SIZE_Y = 100.f;
constexpr float		WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float		WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;
constexpr float		ASTEROID_SPEED = 10.f;
constexpr float		ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float		ASTEROID_COSMETIC_RADIUS = 2.0f;
constexpr float		BULLET_LIFETIME_SECONOS = 2.0f;
constexpr float		BULLET_SPEED = 50.f;
constexpr float		BULLET_PHYSTCS_RADIUS = 0.5f;
constexpr float		BULLET_COSMETIC_RADIUS = 2.0f;
constexpr float		PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float		TURN_SPEED_DEGREES_PER_SECOND = 300.f;
constexpr float		PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float		PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;

constexpr float		BEETLE_SPEED = 10.f;
constexpr float		BEETLE_COSMETIC_RADIUS = 2.0f;
constexpr float		BEETLE_PHYSICS_RADIUS = 1.5f;
constexpr int		MAX_BEETLES = 5;

constexpr float		WASP_ACCELERATION = 30.f;
constexpr float		WASP_COSMETIC_RADIUS = 2.0f;
constexpr float		WASP_PHYSICS_RADIUS = 1.0f;
constexpr int		MAX_WASPS = 5;

constexpr float		DEBRIS_LIFETIME_SECONDS = 2.f;
constexpr int		MAX_DEBRIS = 100;
constexpr int		NUM_DEBRIS_TRIS = 8;
constexpr float		DEBRIS_FIXED_SPEED = 10.f;

constexpr float		MAX_WAVES = 5;

