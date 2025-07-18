#pragma once
#include "Game/Entity.hpp"
#include "Game/Gamecommon.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Rgba8.hpp"


class PlayerShip: public Entity
{
public:
	PlayerShip(Game* owner, Vec2 const& position, float orientationDegrees);
	~PlayerShip();

	
	virtual void Update(float deltaseconds) override;
	void Bounce();
	void UpdateFromKeyboard(float deltaseconds);
	virtual void Render() const override;
	virtual void DebugRender() const override;

	virtual void MarkAsDead() override;

	virtual void Hit() override;

	virtual int GetHealth() const override { return m_health; }
	virtual Vec2 GetPosition() const override { return m_position; }
	Vec2 GetForwardNormal() const;

	void Rebirth();

	void UpdateFromController(float deltaseconds);

	int m_extraLives = 3;

	static void InitializeVerts(Vertex_PCU* vertsToFillIn, Rgba8 Color);

	float m_thrustFraction;

private:
	Vertex_PCU m_localVerts[NUM_SHIP_VERTS];

};