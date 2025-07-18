#pragma once
#include "Game/Gamecommon.hpp"
#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"

class Game;

constexpr int NUM_DEBRIS_VERTS = 3 * NUM_DEBRIS_TRIS;


class Debris : public Entity
{
public:
	Debris(Game* owner, Vec2 const& position, Vec2 const& velocity, float OrientationDegrees, float physicsRadius, Rgba8 color);
	~Debris();

	virtual void Update(float deltaseconds) override;
	virtual void Render() const override;
	virtual void DebugRender() const override;

	Rgba8 m_color;
	float m_angularvelocity;

	float m_restlife;
	float m_alpha;

	virtual void MarkAsDead() override;

	virtual int GetHealth() const override { return m_health; }

private:
	void InitializeLocalVerts();

private:
	Vertex_PCU m_localVerts[NUM_DEBRIS_VERTS];
};