#include "Game/Beetle.hpp"
#include "Game/Gamecommon.hpp"
#include "Game/Playership.hpp"
#include "Game/Entity.hpp"
#include "Game/Debris.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

#include <math.h>

extern RandomNumberGenerator g_RNG;
constexpr float PI = 3.1415926535897932384626433832795f;


Debris::Debris(Game* owner, Vec2 const& position, Vec2 const& velocity, float OrientationDegrees, float physicsRadius, Rgba8 color)
    : Entity(owner, position, OrientationDegrees, color)
{
    m_health = 1;
    m_physicsRadius = physicsRadius * 0.4f; 
    m_cosmeticRadius = physicsRadius * 1.f;

    m_color = color;
    m_restlife = DEBRIS_LIFETIME_SECONDS;

    m_angularvelocity = g_RNG.RollRandomFloatInRange(-200.f, 200.f);
    m_velocity = velocity;
    InitializeLocalVerts();
}

Debris::~Debris()
{
}

void Debris::Update(float deltaSeconds)
{
    m_OrientationDegrees += m_angularvelocity * deltaSeconds;
    m_position += m_velocity * deltaSeconds;
    if (IsOffScreen()) 
    {
        MarkAsDead();
    }
    m_restlife -= deltaSeconds;
    if (m_restlife <= 0.f)
    {
        MarkAsDead();
    }
}

void Debris::Render() const
{
    Vertex_PCU worldSpaceVerts[NUM_DEBRIS_VERTS];
    for (int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; ++vertIndex)
    {
        worldSpaceVerts[vertIndex] = m_localVerts[vertIndex];
    }
    TransformVertexArrayXY3D(NUM_DEBRIS_VERTS, &worldSpaceVerts[0]
        , 1.f, m_OrientationDegrees, m_position);

    float alpha = RangeMapClamped(m_restlife, 0.f, 2.f, 0.f, 127.f);
    Rgba8 rendercolor = m_color;
    rendercolor.a = static_cast<unsigned char>(alpha);
    for (int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; ++vertIndex)
    {
        worldSpaceVerts[vertIndex].m_color = rendercolor;
    }
    g_theRenderer->DrawVertexArray(NUM_DEBRIS_VERTS, &worldSpaceVerts[0]);
}

void Debris::DebugRender() const
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



void Debris::MarkAsDead()
{
    m_isDead = true;
    m_isGarbage = true;
}

void Debris::InitializeLocalVerts()
{
	const float angleStep = 360.0f / NUM_DEBRIS_TRIS; 

	float lastX = 0.0f, lastY = 0.0f;
	Vec3 centerVertex(0, 0, 0.0f); 

	float initialRadius = g_RNG.RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
	float x0 = initialRadius * cosf(0 * (PI / 180.0f));
	float y0 = initialRadius * sinf(0 * (PI / 180.0f));
	lastX = x0;
	lastY = y0;

	for (int i = 0; i < NUM_DEBRIS_TRIS; ++i) {
		float angleEnd = (i + 1) * angleStep;

		m_localVerts[i * 3 + 0] = Vertex_PCU(centerVertex, m_color, Vec2(0.5f, 0.5f));

        float x1 = lastX;
        float y1 = lastY;
		Vec3 vertex1(x1, y1, 0.0f);
		m_localVerts[i * 3 + 1] = Vertex_PCU(vertex1, m_color, Vec2(0.0f, 0.0f));

		float radius2 = g_RNG.RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
		float x2 = radius2 * cosf(angleEnd * (PI / 180.0f));
		float y2 = radius2 * sinf(angleEnd * (PI / 180.0f));
		Vec3 vertex2(x2, y2, 0.0f);
		m_localVerts[i * 3 + 2] = Vertex_PCU(vertex2, m_color, Vec2(1.0f, 0.0f));

        lastX = x2;
        lastY = y2;
	}
}