#pragma once
#include "Game/Gamecommon.hpp"
#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"

class Game;
constexpr int NUM_BULLET_TRIS = 2;
constexpr int NUM_BULLET_VERTS = 3 * NUM_BULLET_TRIS;


class Bullet : public Entity
{
public:
	Bullet(Game* owner, Vec2 const& position, float OrientationDegrees);
	~Bullet();

	virtual void Update(float deltaseconds) override;
	virtual void Render() const override;
	virtual void DebugRender() const override;

	virtual void Hit() override;

	virtual void MarkAsDead() override;

	virtual int GetHealth() const override { return m_health; }


private:
	void InitializeLocalVerts();
	float m_lifetime;
private:
	Vertex_PCU m_localVerts[NUM_BULLET_VERTS];
};