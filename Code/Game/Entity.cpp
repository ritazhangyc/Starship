#include "Game/Entity.hpp"
#include "Game/Playership.hpp"
#include "Game/Bullet.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Gamecommon.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/MathUtils.hpp"


Entity::Entity(Game* owner, Vec2 const& position, float OrientationDegrees, Rgba8 color)
	: m_game(owner)
	, m_position(position)
	, m_OrientationDegrees(OrientationDegrees)
	, m_color(color)
{
	
}

Entity::~Entity()
{
}

void Entity::DebugRender() const
{
	
}

bool Entity::IsOffScreen() const
{
 	if (m_position.x < -m_cosmeticRadius)
	{
		return true;
	}
	if (m_position.x > m_cosmeticRadius+WORLD_SIZE_X) 
	{
		return true;
	}

	if (m_position.y < -m_cosmeticRadius)
	{
		return true;
	}
	if (m_position.y > m_cosmeticRadius + WORLD_SIZE_Y)
	{
		return true;
	}

	else {
		return false;
	}	
}

bool Entity::IsGarbage() const
{
	return m_isGarbage;
}

int Entity::GetHealth() const
{
	return m_health;
}

void Entity::MarkAsDead()
{
	m_isDead = true;
	m_isGarbage = true;
}

void Entity::Hit()
{
	m_health--;
	if (m_health <= 0)
	{
		MarkAsDead();
	}
}

bool Entity::IsAlive()
{
	return !m_isDead;
}

