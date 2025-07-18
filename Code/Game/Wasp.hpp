#pragma once
#include "Game/Gamecommon.hpp"
#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"

class Game;
constexpr int NUM_WASP_TRIS = 4;
constexpr int NUM_WASP_VERTS = 3 * NUM_WASP_TRIS;


class Wasp : public Entity
{
public:
	Wasp(Game* owner, Vec2 const& position, float OrientationDegrees);
	~Wasp();

	void Update(float deltaseconds);
	virtual void Render() const override;
	virtual void DebugRender() const override;

	virtual void Hit() override;

	virtual void MarkAsDead() override;

	virtual int GetHealth() const override { return m_health; }

private:
	void InitializeLocalVerts();
private:
	Vertex_PCU m_localVerts[NUM_WASP_VERTS];
};