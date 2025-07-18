#pragma once
#include "Game/Gamecommon.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerShip.hpp"

#include "Engine/Core/Vertex_PCU.hpp"

class Game;
constexpr int NUM_BEETLE_TRIS = 2;
constexpr int NUM_BEETLE_VERTS = 3 * NUM_BEETLE_TRIS;


class Beetle : public Entity
{
public:
	Beetle(Game* owner, Vec2 const& position, float OrientationDegrees);
	~Beetle();

	virtual void Update(float deltaseconds) override;
	virtual void Render() const override;
	virtual void DebugRender() const override;

	virtual void Hit() override;

	virtual void MarkAsDead() override;

	virtual int GetHealth() const override { return m_health; }

private:
	void InitializeLocalVerts();
private:
	Vertex_PCU m_localVerts[NUM_BEETLE_VERTS];
};