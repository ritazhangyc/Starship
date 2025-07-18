#include "Game/Asteroid.hpp"
#include "Game/Gamecommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

#include <math.h>

constexpr float PI = 3.1415926535897932384626433832795f;
extern RandomNumberGenerator g_RNG;

const int numTriangles = 16; 
const int numVerts = 3 * numTriangles;

Asteroid::Asteroid(Game* owner, Vec2 const& position, float OrientationDegrees)
	: Entity(owner, position, OrientationDegrees, Rgba8(100, 100, 100, 255))
{
    m_health = 3;
    m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
    m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;

    float randomAngle = g_RNG.RollRandomFloatInRange(0.0f, 360.0f);
    m_velocity = Vec2::MakeFromPolarDegrees(randomAngle, 10.0f);
    m_angularvelocity = g_RNG.RollRandomFloatInRange(-200.0f, 200.0f);

    InitializeLocalVerts();
}


void Asteroid::MarkAsDead()
{
    m_isDead = true;
    m_isGarbage = true;
    int Edebris = g_RNG.RollRandomIntInRange(3, 12);

    Vec2 velocity = m_velocity.GetNormalized();
    velocity *= DEBRIS_FIXED_SPEED;
    float physicsRadius = m_physicsRadius;
    Rgba8 color = Rgba8(100, 100, 100, 127);
    m_game->SpawnDebrisCluster(Edebris, m_position, velocity, m_OrientationDegrees, physicsRadius, color);
}

void Asteroid::InitializeLocalVerts()
{
    const float angleStep = 360.0f / 16.f; 
    float lastX = 0.0f, lastY = 0.0f; 
    Vec3 centerVertex(0, 0, 0.0f);  
    float initialRadius = g_RNG.RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
    float x0 = initialRadius * cosf(0 * (PI / 180.0f));
    float y0 = initialRadius * sinf(0 * (PI / 180.0f));
    lastX = x0;
    lastY = y0;

    for (int i = 0; i < 16; ++i) 
    {
        float angleEnd = (i + 1) * angleStep;

        m_localVerts[i * 3 + 0] = Vertex_PCU(centerVertex, Rgba8(100, 100, 100, 255), Vec2(0.5f, 0.5f));

        float x1 = lastX;
        float y1 = lastY;
        Vec3 vertex1(x1, y1, 0.0f);
        m_localVerts[i * 3 + 1] = Vertex_PCU(vertex1, Rgba8(100, 100, 100, 255), Vec2(0.0f, 0.0f));

        float radius2 = g_RNG.RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
        float x2 = radius2 * cosf(angleEnd * (PI / 180.0f));
        float y2 = radius2 * sinf(angleEnd * (PI / 180.0f));
        Vec3 vertex2(x2, y2, 0.0f);
        m_localVerts[i * 3 + 2] = Vertex_PCU(vertex2, Rgba8(100, 100, 100, 255), Vec2(1.0f, 0.0f));

        lastX = x2;
        lastY = y2;
    }
}
void Asteroid::Update(float deltaseconds)
{
    m_position += m_velocity * deltaseconds;

    m_OrientationDegrees += m_angularvelocity * deltaseconds;
    if (m_OrientationDegrees >= 360.0f) 
    {
        m_OrientationDegrees -= 360.0f;
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
}

void Asteroid::Render() const
{
    //Vertex_PCU m_localVerts[48];
    Vertex_PCU worldVerts[numVerts];
    for (int i = 0; i < numVerts; ++i) 
    {
        worldVerts[i] = m_localVerts[i];
    }
    TransformVertexArrayXY3D(48, &worldVerts[0], 1.0f, m_OrientationDegrees, m_position);

    g_theRenderer->DrawVertexArray(48, &worldVerts[0]);
}

void Asteroid::DebugRender() const
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

