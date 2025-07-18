#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Game;

class Entity
{
public:
	Entity(Game* owner, Vec2 const& position, float OrientationDegrees, Rgba8 color);
	virtual ~Entity();
	virtual void Update(float deltaseconds) = 0;
	virtual void Render() const = 0;
	virtual void DebugRender() const;

public: 
	bool IsOffScreen() const;
	bool IsAlive();
	bool IsGarbage() const;

	virtual Vec2 GetPosition() const { return m_position; }
	virtual float GetPD() const { return m_physicsRadius; }
	virtual float GetCD() const { return m_cosmeticRadius; }
	virtual int GetHealth() const;
	virtual void MarkAsDead();
	virtual void Hit();

protected: 
	
	bool m_isDead = false; 
	bool m_isGarbage = false; 

	Vec2 m_position;   
	Vec2 m_velocity;        
	float m_angularvelocity = 0.f; 
	

	float m_OrientationDegrees = 0.f; 
	float m_physicsRadius = 10.f;
	float m_cosmeticRadius = 20.f;  
	Rgba8 m_color;

	int m_health = 1; 

	Game* m_game = nullptr;     
	
	 
};