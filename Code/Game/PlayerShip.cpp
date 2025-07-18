#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

#include "Game/PlayerShip.hpp"
#include "Game/App.hpp"
#include "Game/Gamecommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"


extern RandomNumberGenerator g_RNG;
extern InputSystem* g_theInput;


PlayerShip::PlayerShip(Game* owner, Vec2 const& position, float OrientationDegrees)
    : Entity(owner, position, OrientationDegrees, Rgba8(102,153,204))
{
    m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
    m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
    InitializeVerts(&m_localVerts[0], m_color);
    m_extraLives = 3;
}

PlayerShip::~PlayerShip()
{
}


void PlayerShip::Update(float deltaseconds) 
{
    UpdateFromKeyboard(deltaseconds);
    UpdateFromController(deltaseconds);
    m_position += m_velocity * deltaseconds;
    Bounce();
}

void PlayerShip::Bounce()
{
    if (m_position.x < PLAYER_SHIP_PHYSICS_RADIUS)
    {
        m_position.x = PLAYER_SHIP_PHYSICS_RADIUS;
        m_velocity.x *= -1.f;
    }
    if (m_position.x > WORLD_SIZE_X - PLAYER_SHIP_PHYSICS_RADIUS)
    {
        m_position.x = WORLD_SIZE_X - PLAYER_SHIP_PHYSICS_RADIUS;
        m_velocity.x *= -1.f;
    }
    if (m_position.y < PLAYER_SHIP_PHYSICS_RADIUS)
    {
        m_position.y = PLAYER_SHIP_PHYSICS_RADIUS;
        m_velocity.y *= -1.f;
    }
    if (m_position.y > WORLD_SIZE_Y - PLAYER_SHIP_PHYSICS_RADIUS)
    {
        m_position.y = WORLD_SIZE_Y - PLAYER_SHIP_PHYSICS_RADIUS;
        m_velocity.y *= -1.f;
    }

}

void PlayerShip::UpdateFromKeyboard(float deltaseconds)
{
    bool isKeySDown = g_theInput->IsKeyDown('S');
    bool isKeyFDown = g_theInput->IsKeyDown('F');  
    bool isKeyEDown = g_theInput->IsKeyDown('E');
    bool wasSpacePressed = g_theInput->WasKeyJustPressed(' ');

    if (m_isDead) {
        return;
    }

    if (isKeySDown) 
    {
        m_OrientationDegrees += TURN_SPEED_DEGREES_PER_SECOND * deltaseconds;
    }
    if (isKeyFDown) 
    {
        m_OrientationDegrees -= TURN_SPEED_DEGREES_PER_SECOND * deltaseconds;
    }
	if (isKeyEDown||
        g_theInput->GetController(0).WasButtonJustPressed(XboxButtonID::XBOX_BUTTON_B) )
    {
        Vec2 fwdNormal = Vec2::MakeFromPolarDegrees(m_OrientationDegrees);
        Vec2 a = fwdNormal * PLAYER_SHIP_ACCELERATION;
        m_velocity += a * deltaseconds;
    }
    if (wasSpacePressed) 
    {
		SoundID bulletSound = g_theAudio->CreateOrGetSound("Data/Audio/PlayerShootNormal.wav");
        Vec2 Spawnpos = m_position;
        m_game->SpawnBullets(Spawnpos, m_OrientationDegrees);
        g_theAudio->StartSound(bulletSound);
    }
}

void PlayerShip::Render() const
{
    Vertex_PCU worldSpaceVerts[NUM_SHIP_VERTS];
    for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++ vertIndex)
    {
        worldSpaceVerts[vertIndex] = m_localVerts[vertIndex];
    }
    TransformVertexArrayXY3D(NUM_SHIP_VERTS, &worldSpaceVerts[0]
                          ,1.f, m_OrientationDegrees, m_position);

    g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, &worldSpaceVerts[0]);

}

void PlayerShip::DebugRender() const
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

void PlayerShip::MarkAsDead()
{
    m_isDead = true;
	int Bdebris = g_RNG.RollRandomIntInRange(5, 30);
	float physicsRadius = m_physicsRadius;
	Vec2 velocity = m_velocity.GetNormalized();
	velocity *= DEBRIS_FIXED_SPEED;
	Rgba8 color = Rgba8(102, 153, 204, 127);
	m_game->SpawnDebrisCluster(Bdebris, m_position, velocity, m_OrientationDegrees, physicsRadius, color);
}

void PlayerShip::Hit()
{
    m_health -=1;

    if (m_health <= 0)
    {
        MarkAsDead();
    }
}

Vec2 PlayerShip::GetForwardNormal() const
{
    return Vec2::MakeFromPolarDegrees( m_OrientationDegrees );
}


void PlayerShip::Rebirth()
{
    m_position.x = WORLD_CENTER_X;
    m_position.y = WORLD_CENTER_Y;
    m_OrientationDegrees = 0;
    m_velocity = Vec2(0.f, 0.f);
    m_isDead = false;
    m_health = 1;
    m_extraLives--;
}

void PlayerShip::UpdateFromController(float deltaseconds)
{
    UNUSED(deltaseconds);

    XboxController const& controller = g_theInput->GetController(0);

    if (m_isDead)
    {
        if (controller.WasButtonJustPressed(XboxButtonID::XBOX_BUTTON_START) && m_extraLives > 0)
        {
            Rebirth();
        }
        return;
    }

    float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
    if (leftStickMagnitude > 0.f)
    {
        m_thrustFraction = leftStickMagnitude;
        m_OrientationDegrees = controller.GetLeftStick().GetOrientationDegrees();
		Vec2 fwdNormal = Vec2::MakeFromPolarDegrees(m_OrientationDegrees);
		Vec2 a = fwdNormal * PLAYER_SHIP_ACCELERATION;
		m_velocity += a * deltaseconds * leftStickMagnitude;
    }

    if (controller.WasButtonJustPressed(XboxButtonID::XBOX_BUTTON_A))
    {
        Vec2 forwardNormal = GetForwardNormal();
        Vec2 nosePosition = m_position + (forwardNormal * 1.f);
        m_game->SpawnBullets(nosePosition, m_OrientationDegrees);
    }
}

void PlayerShip::InitializeVerts(Vertex_PCU* vertsToFillIn,Rgba8 color)
{
	vertsToFillIn[0].m_position = Vec3(2.f, 1.f, 0.f);
	vertsToFillIn[1].m_position = Vec3(0.f, 2.f, 0.f);
	vertsToFillIn[2].m_position = Vec3(-2.f, 1.f, 0.f);

	vertsToFillIn[3].m_position = Vec3(-2.f, 1.f, 0.f);
	vertsToFillIn[4].m_position = Vec3(0.f, 1.f, 0.f);
	vertsToFillIn[5].m_position = Vec3(-2.f, -1.f, 0.f);

	vertsToFillIn[6].m_position = Vec3(-2.f, -1.f, 0.f);
	vertsToFillIn[7].m_position = Vec3(0.f, 1.f, 0.f);
	vertsToFillIn[8].m_position = Vec3(0.f, -1.f, 0.f);

	vertsToFillIn[9].m_position = Vec3(0.f, 1.f, 0.f);
	vertsToFillIn[10].m_position = Vec3(0.f, -1.f, 0.f);
	vertsToFillIn[11].m_position = Vec3(1.f, 0.f, 0.f);

	vertsToFillIn[12].m_position = Vec3(2.f, -1.f, 0.f);
	vertsToFillIn[13].m_position = Vec3(-2.f, -1.f, 0.f);
	vertsToFillIn[14].m_position = Vec3(0.f, -2.f, 0.f);

	for (int i = 0; i < NUM_SHIP_VERTS; ++i)
	{
        vertsToFillIn[i].m_color = color;
	}
}
