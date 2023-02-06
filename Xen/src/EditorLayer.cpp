#include "EditorLayer.h"
#include "core/scene/ScriptableEntity.h"

#include <core/app/Timer.h>

float rotation = 0.0f;

float bg_color[4] = { 0.0, 0.0f, 0.0f, 1.0f };

float quad_position[3] = { 0.0f, 0.0f, 0.0f };

bool dark = 0, light = 1, classic = 0;

Xen::Ref<Xen::Texture2D> tex_2d;

EditorLayer::EditorLayer()
{
	m_Timestep = 0.0f;
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::OnAttach()
{
	Xen::FrameBufferSpec specs;
	specs.width = Xen::DesktopApplication::GetWindow()->GetWidth();
	specs.height = Xen::DesktopApplication::GetWindow()->GetHeight();

	m_EditorCamera = std::make_shared<Xen::Camera>(Xen::CameraType::Orthographic, viewport_framebuffer_width, viewport_framebuffer_height);
	m_ViewportFrameBuffer = Xen::FrameBuffer::CreateFrameBuffer(specs);

	Xen::Renderer2D::Init();

	m_ActiveScene = std::make_shared<Xen::Scene>();
	circle_entity = m_ActiveScene->CreateEntity("Circle");
	quad_entity = m_ActiveScene->CreateEntity("Quad");
	camera_entity = m_ActiveScene->CreateEntity("Camera");

	tex = Xen::Texture2D::CreateTexture2D("assets/textures/CheckerBoardTexture.png", 1);
	tex_1 = Xen::Texture2D::CreateTexture2D("assets/textures/microsoft.png", 1);
	//tex_2d = Xen::Texture2D::CreateTexture2D("assets/textures/Consolas.png", 1);
	//tex_2d->LoadTexture();
	tex->LoadTexture();
	tex_1->LoadTexture();

	quad_entity.AddComponent<Xen::Component::SpriteRenderer>(Xen::Color(1.0f, 1.0f, 1.0f, 1.0f));
	circle_entity.AddComponent<Xen::Component::CircleRenderer>(Xen::Color(1.0f, 1.0f, 1.0f, 1.0f), 0.4f, 0.5f, 0.01f);

	Xen::Component::Transform& quad_transform = quad_entity.GetComponent<Xen::Component::Transform>();
	quad_transform.position.x = -0.6f;

	Xen::Component::Transform& circle_transform = circle_entity.GetComponent<Xen::Component::Transform>();
	circle_transform.position.x = 0.6f;

	camera_entity.AddComponent<Xen::Component::CameraComp>(Xen::CameraType::Orthographic, specs.width, specs.height);
	m_EditorCamera->Update();

	class CameraControlScript : public Xen::ScriptableEntity
	{
	private:
		Xen::Ref<Xen::Input> input;
	public:
		void OnCreate() override{
			input = GetInput();
		}

		void OnUpdate(double timestep) override{
			
		}

		void OnDestroy() override{

		}
	};

	//quad_entity_1.AddComponent<Xen::Component::NativeScript>().Bind<CameraControlScript>(quad_entity_1);

	hier_panel = SceneHierarchyPanel(m_ActiveScene);
	prop_panel = PropertiesPanel(hier_panel.GetSelectedEntity());

	//serialiser = Xen::SceneSerializer(m_ActiveScene);
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(double timestep)
{
	hier_panel.SetActiveScene(m_ActiveScene);

	Xen::RenderCommand::Clear();
	m_Timestep = timestep;

	m_ViewportFrameBuffer->Bind();
	Xen::RenderCommand::Clear();
	Xen::RenderCommand::SetClearColor(Xen::Color(0.0f, 0.0f, 0.0f, 1.0f));

	Xen::Renderer2D::BeginScene(m_EditorCamera);
	m_EditorCamera->Update();
	m_ActiveScene->OnUpdate(timestep);
	//Xen::Renderer2D::DrawClearCircle(Xen::Vec3(1.0f, 0.0f, 0.0f), Xen::Vec3(0.0f, 0.0f, 0.0f), Xen::Vec3(1.4f, 1.0f, 1.0f), Xen::Color(0.4f, 0.3f, 0.4f, 1.0f));
	//XEN_ENGINE_LOG_INFO("{0}", (float)viewport_framebuffer_width / (float)viewport_framebuffer_height);

	//Xen::Renderer2D::DrawTexturedQuad(tex_2d, Xen::Vec3(1.0f, 1.0f, 0.0f), Xen::Vec3(0.0f, 0.0f, 0.0f), Xen::Vec3(1.0f, 1.0f, 1.0f), Xen::Color(1.0f, 0.0f, 0.0f, 1.0f));

	Xen::Renderer2D::EndScene();

	Xen::Renderer2D::RenderFrame();
	m_ViewportFrameBuffer->Unbind();
}

void EditorLayer::OnImGuiUpdate()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		static auto first_time = true;
		if (first_time)
		{
			first_time = false;

			ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
			ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

			// split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
			//   window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
			//                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction

			auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, nullptr, &dockspace_id);
			auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.3f, nullptr, &dockspace_id);

			auto dock_id_left_down = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.5f, nullptr, &dock_id_left);

			// we now dock our windows into the docking node we made above
			ImGui::DockBuilderDockWindow(hier_panel.GetPanelTitle().c_str(), dock_id_left);
			ImGui::DockBuilderDockWindow("Window Two", dock_id_down);
			ImGui::DockBuilderDockWindow(prop_panel.GetPanelTitle().c_str(), dock_id_left_down);
			//ImGui::DockBuilderDockWindow("Window Three", dock_id_left_down);
			ImGui::DockBuilderDockWindow((std::string(ICON_FA_MOUNTAIN_SUN) + std::string("  2D Viewport")).c_str(), dockspace_id); // IMP: To Dock In Centre!! use directly 'dockspace_id'
			ImGui::DockBuilderFinish(dockspace_id);
		}
	}
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("(demo menu)", NULL, false, false);
			if (ImGui::MenuItem("New")) {}
			if (ImGui::MenuItem("Open", "Ctrl+O")) 
			{
				m_ActiveScene = std::make_shared<Xen::Scene>();
				Xen::SceneSerializer serialiser = Xen::SceneSerializer(m_ActiveScene);
				serialiser.Deserialize("assets/scene.xen");
			}
			if (ImGui::BeginMenu("Open Recent"))
			{
				ImGui::MenuItem("fish_hat.c");
				ImGui::MenuItem("fish_hat.inl");
				ImGui::MenuItem("fish_hat.h");
				if (ImGui::BeginMenu("More.."))
				{
					ImGui::MenuItem("Hello");
					ImGui::MenuItem("Sailor");
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save", "Ctrl+S")) 
			{
				Xen::SceneSerializer serialiser = Xen::SceneSerializer(m_ActiveScene);
				//serialiser.Deserialize("assets/scene.xen");
				serialiser.Serialize("assets/scene.xen");
			}
			if (ImGui::MenuItem("Save As..")) {}

			ImGui::Separator();
			if (ImGui::BeginMenu("Options"))
			{
				static bool enabled = true;
				ImGui::MenuItem("Enabled", "", &enabled);
				ImGui::BeginChild("child", ImVec2(0, 60), true);
				for (int i = 0; i < 10; i++)
					ImGui::Text("Scrolling Text %d", i);
				ImGui::EndChild();
				static float f = 0.5f;
				static int n = 0;
				ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
				ImGui::InputFloat("Input", &f, 0.1f);
				ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Colors"))
			{
				float sz = ImGui::GetTextLineHeight();
				for (int i = 0; i < ImGuiCol_COUNT; i++)
				{
					const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
					ImVec2 p = ImGui::GetCursorScreenPos();
					ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
					ImGui::Dummy(ImVec2(sz, sz));
					ImGui::SameLine();
					ImGui::MenuItem(name);
				}
				ImGui::EndMenu();
			}

			// Here we demonstrate appending again to the "Options" menu (which we already created above)
			// Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
			// In a real code-base using it would make senses to use this feature from very different code locations.
			if (ImGui::BeginMenu("Options")) // <-- Append!
			{
				static bool b = true;
				ImGui::Checkbox("SomeOption", &b);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Disabled", false)) // Disabled
			{
				IM_ASSERT(0);
			}
			if (ImGui::MenuItem("Checked", NULL, true)) {}
			if (ImGui::MenuItem("Quit", "Alt+F4")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))		{ ImGui::EndMenu(); }
		if (ImGui::BeginMenu("Project"))	{ ImGui::EndMenu(); }
		if (ImGui::BeginMenu("View"))		{ ImGui::EndMenu(); }
		if (ImGui::BeginMenu("Build"))		{ ImGui::EndMenu(); }
		if (ImGui::BeginMenu("Tools"))		{ ImGui::EndMenu(); }
		if (ImGui::BeginMenu("About"))		{ ImGui::EndMenu(); }
		ImGui::EndMenuBar();
	}

	ImGui::End();

	//ImGui::Begin("Window One");
	//ImGui::Text("Hello");
	//
	//ImGui::Checkbox("Render From Camera Entity", &render_from_entity_cam);
	//ImGui::Checkbox("Resizable Camera Entity", &entity_cam_resizable);
	//
	//ImGui::End();

	hier_panel.OnImGuiRender();
	
	ImGui::Begin("Window Two");
	ImGui::Text("Hello");
	ImGui::Text("Framerate: %f", m_Timestep);
	ImGui::Text("This is some content");

	//ImGui::Checkbox("Render From Second Camera", &render_from_entity_cam);

	ImGui::End();

	prop_panel.OnImGuiRender();
	prop_panel.SetActiveEntity(hier_panel.GetSelectedEntity());
	//
	//ImGui::SliderFloat3("Quad Position", quad_position, -5.0f, 5.0f);
	//ImGui::SliderAngle("Angle", &rotation);
	//
	//ImGui::ColorEdit4("Background Color", bg_color);
	//ImGui::ColorEdit4("Quad Color", bg_quad);
	//
	//if (ImGui::RadioButton("Light Theme", light))
	//{
	//	ImGui::StyleColorsLight();
	//	light = 1;
	//	dark = 0;
	//	classic = 0;
	//}
	//else if (ImGui::RadioButton("Dark Theme", dark))
	//{
	//	ImGui::StyleColorsDark();
	//	dark = 1;
	//	light = 0;
	//	classic = 0;
	//}
	//else if (ImGui::RadioButton("Classic Theme", classic))
	//{
	//	ImGui::StyleColorsClassic();
	//	dark = 0;
	//	light = 0;
	//	classic = 1;
	//}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin((std::string(ICON_FA_MOUNTAIN_SUN) + std::string("  2D Viewport")).c_str());

	m_ViewportFocused = ImGui::IsWindowHovered();

	if (viewport_framebuffer_width != ImGui::GetContentRegionAvail().x || viewport_framebuffer_height != ImGui::GetContentRegionAvail().y)
	{
		if (ImGui::GetContentRegionAvail().x <= 0 || ImGui::GetContentRegionAvail().y <= 0) {}
		else {
			viewport_framebuffer_width = ImGui::GetContentRegionAvail().x;
			viewport_framebuffer_height = ImGui::GetContentRegionAvail().y;

			m_ViewportFrameBuffer->Resize(viewport_framebuffer_width, viewport_framebuffer_height);
			m_EditorCamera->OnViewportResize(viewport_framebuffer_width, viewport_framebuffer_height);
		}
	}
	m_ActiveScene->OnViewportResize(viewport_framebuffer_width, viewport_framebuffer_height);

	ImGui::Image((void*)m_ViewportFrameBuffer->GetColorAttachmentRendererID(), ImVec2(viewport_framebuffer_width, viewport_framebuffer_height), ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
	ImGui::PopStyleVar();

}

void EditorLayer::OnFixedUpdate()
{

}

void EditorLayer::OnWindowResizeEvent(Xen::WindowResizeEvent& event)
{
	m_EditorCamera->OnViewportResize(viewport_framebuffer_width, viewport_framebuffer_height);
	m_ActiveScene->OnViewportResize(viewport_framebuffer_width, viewport_framebuffer_height);
}

void EditorLayer::OnMouseScrollEvent(Xen::MouseScrollEvent& event)
{
	if (m_ViewportFocused)
	{
		for (int i = 0; i < 10; i++)
		{
			cam_zoom += event.GetYOffset() * 0.01f;
			m_EditorCamera->SetScale(cam_zoom);
			m_EditorCamera->Update();
		}
	}
}

void EditorLayer::OnKeyPressEvent(Xen::KeyPressEvent& event)
{
	if (m_ViewportFocused)
	{
		if (event.GetKey() == Xen::KeyCode::KEY_W)
			m_EditorCamera->SetPosition(Xen::Vec3(m_EditorCamera->GetPosition().x, m_EditorCamera->GetPosition().y + 0.01f, m_EditorCamera->GetPosition().z));
		if (event.GetKey() == Xen::KeyCode::KEY_A)
			m_EditorCamera->SetPosition(Xen::Vec3(m_EditorCamera->GetPosition().x - 0.01f, m_EditorCamera->GetPosition().y, m_EditorCamera->GetPosition().z));
		if (event.GetKey() == Xen::KeyCode::KEY_S)
			m_EditorCamera->SetPosition(Xen::Vec3(m_EditorCamera->GetPosition().x, m_EditorCamera->GetPosition().y - 0.01f, m_EditorCamera->GetPosition().z));
		if (event.GetKey() == Xen::KeyCode::KEY_D)
			m_EditorCamera->SetPosition(Xen::Vec3(m_EditorCamera->GetPosition().x + 0.01f, m_EditorCamera->GetPosition().y, m_EditorCamera->GetPosition().z));
	}
}
