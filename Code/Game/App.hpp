#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EventSystem.hpp"

class Game;

class App
{
public:
	App();
	~App();

	void Startup();
	void Shutdown();
	void RunFrame();

	void RunMainLoop();
	bool IsQuitting() const { return m_isQuitting; }
	void HandleQuitRequested();

	void AttractModeUpdate();
	void AttractModeRender() const;


	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();

	void RenderDevConsole() const;
	static bool	QuitGame(EventArgs& args);

public:
	bool IsInAttractMode = true;
	Game* m_game = nullptr;

private:
 	bool m_isQuitting = false;
	float varyTime = 0.f;

	Camera m_attractModeCamera;	
};

