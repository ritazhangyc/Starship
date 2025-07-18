#pragma once
#include "Game/Gamecommon.hpp"
#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"


class Game;


class Asteroid : public Entity
{
public:
	Asteroid(Game* owner, Vec2 const& position, float OrientationDegrees);

	virtual void Update(float deltaseconds) override;
	virtual void Render() const override;
	virtual void DebugRender() const override;

	virtual int GetHealth() const override { return m_health; }

	virtual void MarkAsDead() override;

private:
	void InitializeLocalVerts();

	Vertex_PCU m_localVerts[48];
	

};
