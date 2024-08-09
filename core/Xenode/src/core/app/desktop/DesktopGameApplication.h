#pragma once

#include <Core.h>

#ifdef XEN_DEVICE_DESKTOP

#include <core/app/Layer.h>
#include <core/app/GameApplication.h>
#include <core/renderer/GraphicsContext.h>

#include <imgui/ImGuiLayer.h>

#include "Window.h"

namespace Xen
{
	struct DesktopGameProperties
	{
		// Window Properties: ----------------------------------
		uint32_t windowWidth, windowHeight;
		std::string windowTitle;

		bool windowVsync, windowResizable, windowOnTop;

		// If 0, the window is "windowed", if fullscreen, it will be the index of the monitor.
		uint8_t fullScreenMonitorIndex;
		// -----------------------------------------------------

		// TODO: Look after the debug rendering situation, whether it should be used with a #define or a boolean variable
		// 
		// Debug Rendering Includes:
		//		ImGui Overlays
		//		Rendering from the debug renderer
		bool enableDebugRenderer;

		GameType gameType;
		GraphicsAPI api;
		ScriptLang scriptLang;

		DesktopGameProperties()
		{
			// Default values are provided here
			windowWidth = 1600;
			windowHeight = 900;
			windowTitle = "Xenode Game";
			windowVsync = false;
			windowResizable = true;
			windowOnTop = false;

			fullScreenMonitorIndex = 0;

			enableDebugRenderer = true;
			gameType = GameType::_2D;
			api = GraphicsAPI::XEN_OPENGL_API;
			scriptLang = ScriptLang::Lua;
		}
	};

	class XEN_API DesktopGameApplication : public GameApplication
	{
	public:
		DesktopGameApplication();
		virtual ~DesktopGameApplication();

		// Functions related to adding/removing Layers: ------------------------------
		void PushLayer(const Ref<Layer>& layer);
		void PopLayer();
		// ---------------------------------------------------------------------------

		// Basic functions that define the lifecycle of the game: --------------------
		
		// Functions that run once for initialization
		virtual void OnCreate();
		virtual void OnStart();

		// Functions that Run Repeatedly
		virtual void OnUpdate(float timestep);
		virtual void OnFixedUpdate();
		virtual void OnRender();

		// Functions that run once during shutdown
		virtual void OnExit();
		// ---------------------------------------------------------------------------

		// The Event Callback functions
		void OnEvent(Event& event);
		void OnWindowCloseEvent(WindowCloseEvent& windowCloseEvent);
		void OnCharEnterEvent(CharEnterEvent& charEnterEvent);

		// Renders all the ImGui code from all the layers:
		void ImGuiRender();

		// Make sure this method could be accessed only by the entry point:
		void Run();

		// Getters: ----------------------------------------------------------------------------------------------------
		inline Ref<Window> GetGameWindow() const				{ return m_GameWindow; }

		// Getters inherited from the GameApplication:
		inline GameType GetGameType() const override			{ return gameProperties.gameType; };
		inline GraphicsAPI GetGraphicsAPI() const override		{ return gameProperties.api; };
		inline ScriptLang GetScriptLang() const override		{ return gameProperties.scriptLang; }

	protected:
		DesktopGameProperties gameProperties;

	private:
		Ref<Window> m_GameWindow;
		GraphicsContext* m_GraphicsContext;
		Vector<Ref<Layer>> m_LayerStack;

		Ref<ImGuiLayer> m_ImGuiLayer;

		bool m_IsGameRunning;
	};

	// Should be implemented in the game runtime:
	XEN_API DesktopGameApplication* CreateApplication();
}

#endif