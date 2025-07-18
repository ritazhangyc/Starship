#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Clock.hpp"

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
XboxController* g_xbox = nullptr;
AudioSystem* g_theAudio = nullptr;
Window* g_theWindow = nullptr;
App* g_theApp = nullptr;


App::App()
{
}

App::~App()
{
}

void App::Startup()
{
	// create all Engine subsystems
	DevConsoleConfig devConsoleConfig;
	devConsoleConfig.m_font = std::string("Data/Fonts/SquirrelFixedFont");
	devConsoleConfig.m_numLines = 35;
	g_theDevConsole = new DevConsole(devConsoleConfig);

	InputSystemConfig inputConfig;
	g_theInput = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_aspectRatio = 2.0f;
	windowConfig.m_inputSystem = g_theInput;
	windowConfig.m_windowTitle = "Starship";
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);

	AudioSystemConfig audioSystemConfig;
	g_theAudio = new AudioSystem(audioSystemConfig);

	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);

	g_theEventSystem->SubscribeEventCallbackFunction("quit", App::QuitGame);

	// start up all Engine subsystems
	g_theInput->Startup();
	g_theWindow->Startup();
	g_theRenderer->Startup();
	g_theAudio->Startup();
	g_theDevConsole->Startup();
	g_theEventSystem->StartUp();

	// create and start the game
	m_game = new Game();

	g_theDevConsole->AddLine(Rgba8::TRANSPARENT_GREY, "E - accelerate");
	g_theDevConsole->AddLine(Rgba8::TRANSPARENT_GREY, "D - decelerate");
	g_theDevConsole->AddLine(Rgba8::TRANSPARENT_GREY, "S - turn anticlockwise");
	g_theDevConsole->AddLine(Rgba8::TRANSPARENT_GREY, "F - turn clockwise");
	g_theDevConsole->AddLine(Rgba8::TRANSPARENT_GREY, "N - Respawn");
	g_theDevConsole->AddLine(Rgba8::TRANSPARENT_GREY, "F1 - Debugmode");
	g_theDevConsole->AddLine(Rgba8::TRANSPARENT_GREY, "F8 - Restart Game");
}

void App::Shutdown() 
{
	delete m_game;
	m_game = nullptr;

	g_theAudio->ShutDown();
	g_theRenderer->ShutDown();
	g_theWindow->ShutDown();
	g_theInput->ShutDown();
	g_theDevConsole->Shutdown();
	g_theEventSystem->StartUp();

	delete g_theAudio;
	g_theAudio = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;

	delete g_theWindow;
	g_theWindow = nullptr;

	delete g_theInput;
	g_theInput = nullptr;

	delete g_theDevConsole;
	g_theDevConsole = nullptr;

	delete g_theEventSystem;
	g_theEventSystem = nullptr;

}

void App::RunFrame() 
{
	BeginFrame();
	Update();
	Render();
	EndFrame();
}

void App::HandleQuitRequested() 
{
    m_isQuitting = true;
}

void App::BeginFrame()
{
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();
	g_theEventSystem->BeginFrame();
	g_theDevConsole->BeginFrame();

	if (g_theInput->WasKeyJustPressed(KEYCODE_TILDE))
	{
		g_theDevConsole->ToggleMode();
	}
}

void App::RunMainLoop()
{
	while (!IsQuitting())
	{
		RunFrame();
	}
}

void App::Update()
{
	m_attractModeCamera.SetOrthoView(Vec2(0.0f, 0.0f), Vec2(200.0f, 100.0f));

	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC)
	 || g_theInput->GetController(0).WasButtonJustPressed(XboxButtonID::XBOX_BUTTON_X))
	{
		if (IsInAttractMode)
		{
			m_isQuitting = true;
		}
		if (!IsInAttractMode)
		{
			IsInAttractMode = true;
			g_theApp->AttractModeRender();
		}
	}

    if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
    {
        delete m_game;
        m_game = new Game();
    }

	if (IsInAttractMode)
	{
		AttractModeUpdate();
	}
	if(!IsInAttractMode)
	{
        m_game->Update();
	}

	float deltaSeconds = static_cast<float>(Clock::GetSystemClock().GetDeltaSeconds());
	varyTime += deltaSeconds;
	if (varyTime > 360.f)
	{
		varyTime = 0.f;
	}
}

void App::AttractModeUpdate()
{
	if (g_theInput->IsKeyDown(' ') || 
		g_theInput->IsKeyDown('N') ||
		g_theInput->GetController(0).WasButtonJustPressed(XboxButtonID::XBOX_BUTTON_START) ||
		g_theInput->GetController(0).WasButtonJustPressed(XboxButtonID::XBOX_BUTTON_A))
	{
		IsInAttractMode = false;
	}

}

void App::AttractModeRender() const
{	
	Rgba8 m_color;
	Vertex_PCU shipVerts[21];
	
	shipVerts[0] = Vertex_PCU(Vec3(-40.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[1] = Vertex_PCU(Vec3(-20.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[2] = Vertex_PCU(Vec3(-20.f, 40.f, 0.f), m_color, Vec2(0.5f, 0.5f));
									 				  
	shipVerts[3] = Vertex_PCU(Vec3(-40.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[4] = Vertex_PCU(Vec3(-20.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[5] = Vertex_PCU(Vec3(-20.f,-40.f, 0.f), m_color, Vec2(0.5f, 0.5f));
									
	shipVerts[6] = Vertex_PCU(Vec3( 20.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[7] = Vertex_PCU(Vec3( 40.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[8] = Vertex_PCU(Vec3( 20.f, 40.f, 0.f), m_color, Vec2(0.5f, 0.5f));

	shipVerts[9]  = Vertex_PCU(Vec3( 20.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[10] = Vertex_PCU(Vec3( 40.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[11] = Vertex_PCU(Vec3( 20.f,-40.f, 0.f), m_color, Vec2(0.5f, 0.5f));

	shipVerts[12] = Vertex_PCU(Vec3(-20.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[13] = Vertex_PCU(Vec3( 20.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[14] = Vertex_PCU(Vec3(  0.f, 20.f, 0.f), m_color, Vec2(0.5f, 0.5f));

	shipVerts[15] = Vertex_PCU(Vec3(-20.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[16] = Vertex_PCU(Vec3( 20.f,  0.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[17] = Vertex_PCU(Vec3( 20.f,-40.f, 0.f), m_color, Vec2(0.5f, 0.5f));
									  
	shipVerts[18] = Vertex_PCU(Vec3(-20.f, -40.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[19] = Vertex_PCU(Vec3( 20.f, -40.f, 0.f), m_color, Vec2(0.5f, 0.5f));
	shipVerts[20] = Vertex_PCU(Vec3(-20.f,   0.f, 0.f), m_color, Vec2(0.5f, 0.5f));


	Vertex_PCU playVerts[3];
	playVerts[0] = Vertex_PCU(Vec3( 20.f,  0.f, 0.f), Rgba8(60, 179, 113), Vec2(0.5f, 0.5f));
	playVerts[1] = Vertex_PCU(Vec3(-20.f, 20.f, 0.f), Rgba8(60, 179, 113), Vec2(0.5f, 0.5f));
	playVerts[2] = Vertex_PCU(Vec3(-20.f,-20.f, 0.f), Rgba8(60, 179, 113), Vec2(0.5f, 0.5f));


    Vertex_PCU worldShipVerts[21];
	for (int shipIndex = 0; shipIndex < 21; ++shipIndex)
	{
        worldShipVerts[shipIndex] = shipVerts[shipIndex];
	}
	TransformVertexArrayXY3D(21, &worldShipVerts[0], 1.0f, 0.0f, Vec2(75.f, 50.f));

	unsigned char R = static_cast<unsigned char>(  80.f + sinf(varyTime) * (160.f -  80.f) );
	unsigned char G = static_cast<unsigned char>( 160.f + sinf(varyTime) * (240.f - 160.f) );
	unsigned char B = static_cast<unsigned char>( 230.f + sinf(varyTime) * (250.f - 230.f) );
	Rgba8 A_color = Rgba8(R, G, B);
	for (int vertIndex = 0; vertIndex < 21; ++vertIndex)
	{
		worldShipVerts[vertIndex].m_color = A_color;
	}
	g_theRenderer->DrawVertexArray(21, &worldShipVerts[0]);

	Vertex_PCU worldPlayVerts[3];
	for (int vertIndex = 0; vertIndex < 3; ++vertIndex)
	{
		worldPlayVerts[vertIndex] = playVerts[vertIndex];
	}

	TransformVertexArrayXY3D(3, &worldPlayVerts[0], 1.0f, 0.0f, Vec2(160.f, 50.0f));

    g_theRenderer->DrawVertexArray( 3, &worldPlayVerts[0]);
}

void App::Render() const 
{
    g_theRenderer->ClearScreen(Rgba8(0,0,0));
	float cameraAspect = g_theWindow->GetAspect();

	Camera devConsoleCamera;
	devConsoleCamera.SetOrthoView(Vec2(0.0f, 0.0f), Vec2(800.0f * cameraAspect, 800.0f));
	g_theRenderer->BeginCamera(devConsoleCamera);
	RenderDevConsole();
	g_theRenderer->EndCamera(devConsoleCamera);

	if (IsInAttractMode)
	{
		g_theRenderer->BeginCamera(m_attractModeCamera);
		AttractModeRender();
		g_theRenderer->EndCamera(m_attractModeCamera);
	}
	if(!IsInAttractMode)
	{
		m_game->Render();
	}
}

void App::RenderDevConsole() const
{
	AABB2 box = AABB2(Vec2(0.0f, 0.0f), Vec2(1600.0f, 800.0f));
	g_theDevConsole->Render(box, g_theRenderer);

}

void App::EndFrame() 
{
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	g_theRenderer->EndFrame();
	g_theAudio->EndFrame();
	g_theInput->EndFrame();
	g_theDevConsole->EndFrame();
}

bool App::QuitGame(EventArgs& args)
{
	UNUSED(args);
	g_theApp->HandleQuitRequested();
	return false;
}

