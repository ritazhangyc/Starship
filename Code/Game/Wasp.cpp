#include "Game/Beetle.hpp"
#include "Game/Gamecommon.hpp"
#include "Game/Game.hpp"
#include "Game/Wasp.hpp"
#include "Game/Playership.hpp"
#include "Game/Entity.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

extern RandomNumberGenerator g_RNG;

Wasp::Wasp(Game* owner, Vec2 const& position, float OrientationDegrees)
    : Entity(owner, position, OrientationDegrees, Rgba8(250, 128, 114, 255))
{
    m_health = 2;
    m_physicsRadius = WASP_PHYSICS_RADIUS;
    m_cosmeticRadius = WASP_COSMETIC_RADIUS;
    m_velocity = Vec2(0, 0);
    InitializeLocalVerts();
}

Wasp::~Wasp()
{
}

void Wasp::Update(float deltaseconds)
{
	Vec2 playerPosition = m_game->GetPlayerShip()->GetPosition();
    Vec2 FacingDirection = playerPosition - m_position;

	float targetDegrees = FacingDirection.GetOrientationDegrees();
	m_OrientationDegrees = targetDegrees;

    Vec2 Acceleration = FacingDirection.GetNormalized() * WASP_ACCELERATION;
    m_velocity += Acceleration * deltaseconds;
    
    if (m_velocity.GetLength() >= 30.f)
    {
        m_velocity.ClampLength(30.f);
    }

	if (m_position.x < -m_cosmeticRadius)
	{
		m_position.x += WORLD_SIZE_X + 2 * m_cosmeticRadius;
	}
	else if (m_position.x > WORLD_SIZE_X + m_cosmeticRadius)
	{
		m_position.x -= WORLD_SIZE_X + 2 * m_cosmeticRadius;
	}

	if (m_position.y < -m_cosmeticRadius)
	{
		m_position.y += WORLD_SIZE_Y + 2 * m_cosmeticRadius;
	}
	else if (m_position.y > WORLD_SIZE_Y + m_cosmeticRadius)
	{
		m_position.y -= WORLD_SIZE_Y + 2 * m_cosmeticRadius;
	}
    m_position += m_velocity * deltaseconds;
}

void Wasp::Render() const
{
    Vertex_PCU worldSpaceVerts[NUM_WASP_VERTS];
    for (int vertIndex = 0; vertIndex < NUM_WASP_VERTS; ++vertIndex)
    {
        worldSpaceVerts[vertIndex] = m_localVerts[vertIndex];
    }
    TransformVertexArrayXY3D(NUM_WASP_VERTS, &worldSpaceVerts[0]
        , 1.f, m_OrientationDegrees, m_position);

    g_theRenderer->DrawVertexArray(NUM_WASP_VERTS, &worldSpaceVerts[0]);
}

void Wasp::DebugRender() const
{
    float lineThickness = 0.2f;
    DebugDrawRing(m_position, m_physicsRadius, lineThickness, Rgba8(0, 255, 255));
    DebugDrawRing(m_position, m_cosmeticRadius, lineThickness, Rgba8(255, 0, 255));
    DebugDrawLine(m_position, m_position + m_velocity, Rgba8(255, 255, 0), lineThickness);
    Vec2 forwardVector = Vec2::MakeFromPolarDegrees(m_OrientationDegrees, m_cosmeticRadius);
    DebugDrawLine(m_position, m_position + forwardVector, Rgba8(255, 0, 0), lineThickness);
    Vec2 leftVector = forwardVector.GetRotatedDegrees(90.0f);
    DebugDrawLine(m_position, m_position + leftVector, Rgba8(0, 255, 0), lineThickness);
}

void Wasp::Hit()
{
    m_health--;
    if (m_health <= 0)
    {
        MarkAsDead();
    }
}

void Wasp::MarkAsDead()
{
    m_isDead = true;
    m_isGarbage = true;
	int Edebris = g_RNG.RollRandomIntInRange(3, 12);

	Vec2 velocity = m_velocity.GetNormalized();
	velocity *= DEBRIS_FIXED_SPEED;
	float physicsRadius = m_physicsRadius;
	Rgba8 color = Rgba8(250, 128, 114, 127);
	m_game->SpawnDebrisCluster(Edebris, m_position, velocity, m_OrientationDegrees, physicsRadius, color);
}

void Wasp::InitializeLocalVerts()
{
    m_localVerts[0].m_position = Vec3(2.f, 0.f, 0.f);
    m_localVerts[1].m_position = Vec3(0.f, 1.f, 0.f);
    m_localVerts[2].m_position = Vec3(0.f, 2.f, 0.f);

    m_localVerts[3].m_position = Vec3(2.f, 0.f, 0.f);
    m_localVerts[4].m_position = Vec3(0.f, -2.f, 0.f);
    m_localVerts[5].m_position = Vec3(0.f, -1.f, 0.f);

    m_localVerts[6].m_position = Vec3(2.f, 0.f, 0.f);
    m_localVerts[7].m_position = Vec3(0.f, 1.f, 0.f);
    m_localVerts[8].m_position = Vec3(-2.f, 0.f, 0.f);

    m_localVerts[9].m_position = Vec3(2.f, 0.f, 0.f);
    m_localVerts[10].m_position = Vec3(0.f, -1.f, 0.f);
    m_localVerts[11].m_position = Vec3(-2.f, 0.f, 0.f);

    m_localVerts[0].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[1].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[2].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[3].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[4].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[5].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[6].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[7].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[8].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[9].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[10].m_color = Rgba8(250, 128, 114, 255);
    m_localVerts[11].m_color = Rgba8(250, 128, 114, 255);
}

