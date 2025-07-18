#include "Game/Bullet.hpp"
#include "Game/Gamecommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

extern RandomNumberGenerator g_RNG;

Bullet::Bullet(Game* owner, Vec2 const& position, float OrientationDegrees)
	: Entity(owner, position, OrientationDegrees, Rgba8(255,255,0,255))
{
    m_health = 1;
    m_physicsRadius = BULLET_PHYSTCS_RADIUS;
    m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
    m_lifetime = 2.f;
	InitializeLocalVerts();
    m_velocity = Vec2::MakeFromPolarDegrees(m_OrientationDegrees, BULLET_SPEED);
}

Bullet::~Bullet()
{
}

void Bullet::Update(float deltaseconds)
{
	m_position += m_velocity * deltaseconds;
    
    if (IsOffScreen())
    {
        m_isDead = true;
        m_isGarbage = true;
    }

    m_lifetime -= deltaseconds;
    if (m_lifetime <= 0)
    {
        MarkAsDead();
    }
}

void Bullet::Render() const
{   
    Vertex_PCU worldSpaceVerts[NUM_BULLET_VERTS];
    for (int vertIndex = 0; vertIndex < NUM_BULLET_VERTS; ++vertIndex)
    {
        worldSpaceVerts[vertIndex] = m_localVerts[vertIndex];
    }
    TransformVertexArrayXY3D(NUM_BULLET_VERTS, &worldSpaceVerts[0]
        , 1.f, m_OrientationDegrees, m_position);

    g_theRenderer->DrawVertexArray(NUM_BULLET_VERTS, &worldSpaceVerts[0]);
}


void Bullet::DebugRender() const
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

void Bullet::Hit()
{
    MarkAsDead();
}

void Bullet::MarkAsDead()
{
    m_isDead = true;
    m_isGarbage = true;
	int Bdebris = g_RNG.RollRandomIntInRange(1, 3);
	Vec2 velocity = m_velocity.GetNormalized();
	velocity *= DEBRIS_FIXED_SPEED;
	float physicsRadius = m_physicsRadius;
	Rgba8 color = Rgba8(200, 0, 0, 127);
	m_game->SpawnDebrisCluster(Bdebris, m_position, velocity, m_OrientationDegrees, physicsRadius, color);
}

void Bullet::InitializeLocalVerts()
{
    m_localVerts[0].m_position = Vec3(0.f, 0.5f, 0.f);
    m_localVerts[1].m_position = Vec3(0.5f, 0.f, 0.f);
    m_localVerts[2].m_position = Vec3(0.f, -0.5f, 0.f);
    
    m_localVerts[3].m_position = Vec3(0.f, 0.5f, 0.f);
    m_localVerts[4].m_position = Vec3(0.f, -0.5f, 0.f);
    m_localVerts[5].m_position = Vec3(-2.f, 0.f, 0.f);

    m_localVerts[0].m_color = Rgba8(255, 255, 0, 255);
    m_localVerts[1].m_color = Rgba8(255, 255, 0, 255);
    m_localVerts[2].m_color = Rgba8(255, 255, 0, 255);
    m_localVerts[3].m_color = Rgba8(255, 0, 0, 255);
    m_localVerts[4].m_color = Rgba8(255, 0, 0, 255);
    m_localVerts[5].m_color = Rgba8(255, 0, 0, 0);
}
