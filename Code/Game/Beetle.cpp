#include "Game/Beetle.hpp"
#include "Game/Game.hpp"
#include "Game/Gamecommon.hpp"
#include "Game/Playership.hpp"
#include "Game/Entity.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

extern RandomNumberGenerator g_RNG;

Beetle::Beetle(Game* owner, Vec2 const& position, float OrientationDegrees)
    : Entity(owner, position, OrientationDegrees, Rgba8(0, 139, 139, 255))
{
    m_health = 2;
    m_physicsRadius = BEETLE_PHYSICS_RADIUS;
    m_cosmeticRadius = BEETLE_COSMETIC_RADIUS;
    InitializeLocalVerts();
}

Beetle::~Beetle()
{
}

void Beetle::Update(float deltaseconds)
{
    Vec2 playerPosition = m_game->GetPlayerShip()->GetPosition();
    Vec2 cha = playerPosition - m_position;
    cha = cha.GetNormalized();

    float targetDegrees = cha.GetOrientationDegrees();
    m_OrientationDegrees = targetDegrees;

    m_velocity = Vec2::MakeFromPolarDegrees(m_OrientationDegrees, BEETLE_SPEED);
    m_position += m_velocity * deltaseconds;

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
}

void Beetle::Render() const
{
    Vertex_PCU worldSpaceVerts[NUM_BEETLE_VERTS];
    for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++vertIndex)
    {
        worldSpaceVerts[vertIndex] = m_localVerts[vertIndex];
    }
    TransformVertexArrayXY3D(NUM_BEETLE_VERTS, &worldSpaceVerts[0]
        , 1.f, m_OrientationDegrees, m_position);

    g_theRenderer->DrawVertexArray(NUM_BEETLE_VERTS, &worldSpaceVerts[0]);
}

void Beetle::DebugRender() const
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

void Beetle::Hit()
{
    m_health--;
    if (m_health <= 0)
    {
        MarkAsDead();
    }
}

void Beetle::MarkAsDead()
{
    m_isDead = true;
    m_isGarbage = true;
	int Edebris = g_RNG.RollRandomIntInRange(3, 12);
	Vec2 velocity = m_velocity.GetNormalized();
	velocity *= DEBRIS_FIXED_SPEED;
	float physicsRadius = m_physicsRadius;
	Rgba8 color = Rgba8(0, 139, 139, 127);
	m_game->SpawnDebrisCluster(Edebris, m_position, velocity, m_OrientationDegrees, physicsRadius, color);
}

void Beetle::InitializeLocalVerts()
{
    m_localVerts[0].m_position = Vec3(1.6f, 1.2f, 0.f);
    m_localVerts[1].m_position = Vec3(1.6f, -1.2f, 0.f);
    m_localVerts[2].m_position = Vec3(-1.4f, -1.4f, 0.f);

    m_localVerts[3].m_position = Vec3(-1.4f, -1.4f, 0.f);
    m_localVerts[4].m_position = Vec3(1.6f, 1.2f, 0.f);
    m_localVerts[5].m_position = Vec3(-1.4f, 1.4f, 0.f);

    m_localVerts[0].m_color = Rgba8(0, 139, 139, 255);
    m_localVerts[1].m_color = Rgba8(0, 139, 139, 255);
    m_localVerts[2].m_color = Rgba8(0, 139, 139, 255);
    m_localVerts[3].m_color = Rgba8(0, 139, 139, 255);
    m_localVerts[4].m_color = Rgba8(0, 139, 139, 255);
    m_localVerts[5].m_color = Rgba8(0, 139, 139, 255);
}