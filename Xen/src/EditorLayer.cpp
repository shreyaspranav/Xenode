#include "EditorLayer.h"

float rotation = 0.0f;

float bg_color[4] = { 0.0, 0.0f, 0.0f, 1.0f };
float bg_quad[4] = { 1.0, 1.0f, 1.0f, 1.0f };

float quad_position[2] = { 0.0f, 0.0f };

bool dark = 0, light = 1, classic = 0;

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
	specs.width = Xen::GameApplication::GetWindow()->GetWidth();
	specs.height = Xen::GameApplication::GetWindow()->GetHeight();

	m_EditorCamera = std::make_shared<Xen::OrthographicCamera>(viewport_framebuffer_width, viewport_framebuffer_width);
	m_ViewportFrameBuffer = Xen::FrameBuffer::CreateFrameBuffer(specs);

	Xen::Renderer2D::Init();

	m_ActiveScene = std::make_shared<Xen::Scene>();
	quad_entity = m_ActiveScene->CreateEntity("Quad");

	tex = Xen::Texture2D::CreateTexture2D("assets/textures/microsoft.png", 1);
	tex->LoadTexture();

	//quad_entity.AddComponent<Xen::Component::Transform>(Xen::Vec3(quad_position[0], quad_position[1], 0.0f), glm::degrees(rotation), Xen::Vec2(1.0f));
	quad_entity.AddComponent<Xen::Component::SpriteRenderer>(Xen::Color(bg_quad[0], bg_quad[1], bg_quad[2], bg_quad[3]), tex);
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(double timestep)
{
	Xen::RenderCommand::Clear();
	m_Timestep = timestep;

	m_ViewportFrameBuffer->Bind();
	Xen::RenderCommand::Clear();
	Xen::RenderCommand::SetClearColor(Xen::Color(bg_color[0], bg_color[1], bg_color[2], bg_color[3]));


	Xen::Renderer2D::BeginScene(m_EditorCamera);

	//m_ActiveScene->m_Registry.replace<Xen::Component::Transform>(m_ActiveScene->quad_entity, Xen::Vec3(quad_position[0], quad_position[1], 0.0f), glm::degrees(rotation), Xen::Vec2(1.0f));
	//m_ActiveScene->m_Registry.replace<Xen::Component::SpriteRenderer>(m_ActiveScene->quad_entity, Xen::Color(bg_quad[0], bg_quad[1], bg_quad[2], bg_quad[3]));

	auto&& transform = quad_entity.GetComponent<Xen::Component::Transform>();
	transform.position.x = quad_position[0];
	transform.position.y = quad_position[1];

	transform.rotation = glm::degrees(rotation);

	auto&& sprite_renderer = quad_entity.GetComponent<Xen::Component::SpriteRenderer>();
	sprite_renderer.color = Xen::Color(bg_quad[0], bg_quad[1], bg_quad[2], bg_quad[3]);

	m_ActiveScene->OnUpdate(timestep);
	//Xen::Renderer2D::DrawClearQuad(Xen::Vec3(quad_position[0], quad_position[1], 0.0f), glm::degrees(rotation), Xen::Vec2(1.0f), Xen::Color(bg_quad[0], bg_quad[1], bg_quad[2], bg_quad[3]));
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
			ImGui::DockBuilderDockWindow("Window One", dock_id_left);
			ImGui::DockBuilderDockWindow("Window Two", dock_id_down);
			ImGui::DockBuilderDockWindow("Window Three", dock_id_left_down);
			ImGui::DockBuilderDockWindow("2D Viewport", dockspace_id); // IMP: To Dock In Centre!! use directly 'dockspace_id'
			ImGui::DockBuilderFinish(dockspace_id);
		}
	}

	ImGui::End();

	ImGui::Begin("Window One");
	ImGui::Text("Hello");


	ImGui::End();
	
	ImGui::Begin("Window Two");
	ImGui::Text("Hello");
	ImGui::Text("Framerate: %f", m_Timestep);
	ImGui::Text("This is some content");

	ImGui::End();

	ImGui::Begin("Window Three");

	ImGui::SliderFloat2("Quad Position", quad_position, -5.0f, 5.0f);
	ImGui::SliderAngle("Angle", &rotation);

	ImGui::ColorEdit4("Background Color", bg_color);
	ImGui::ColorEdit4("Quad Color", bg_quad);

	if (ImGui::RadioButton("Light Theme", light))
	{
		ImGui::StyleColorsLight();
		light = 1;
		dark = 0;
		classic = 0;
	}
	else if (ImGui::RadioButton("Dark Theme", dark))
	{
		ImGui::StyleColorsDark();
		dark = 1;
		light = 0;
		classic = 0;
	}
	else if (ImGui::RadioButton("Classic Theme", classic))
	{
		ImGui::StyleColorsClassic();
		dark = 0;
		light = 0;
		classic = 1;
	}
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("2D Viewport");

	m_ViewportFocused = ImGui::IsWindowHovered();

	if (viewport_framebuffer_width != ImGui::GetContentRegionAvail().x || viewport_framebuffer_height != ImGui::GetContentRegionAvail().y)
	{
		if (ImGui::GetContentRegionAvail().x <= 0 || ImGui::GetContentRegionAvail().y <= 0) {}
		else {
			viewport_framebuffer_width = ImGui::GetContentRegionAvail().x;
			viewport_framebuffer_height = ImGui::GetContentRegionAvail().y;

			m_ViewportFrameBuffer->Resize(viewport_framebuffer_width, viewport_framebuffer_height);
			m_EditorCamera->OnWindowResize(viewport_framebuffer_width, viewport_framebuffer_height);
		}
	}

	ImGui::Image((void*)m_ViewportFrameBuffer->GetColorAttachmentRendererID(), ImVec2(viewport_framebuffer_width, viewport_framebuffer_height));

	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorLayer::OnFixedUpdate()
{

}

void EditorLayer::OnWindowResizeEvent(Xen::WindowResizeEvent& event)
{
	m_EditorCamera->OnWindowResize(viewport_framebuffer_width, viewport_framebuffer_height);
}

void EditorLayer::OnMouseScrollEvent(Xen::MouseScrollEvent& event)
{
	if (m_ViewportFocused)
	{
		cam_zoom += event.GetYOffset() * m_Timestep;
		m_EditorCamera->SetScale(cam_zoom);
		m_EditorCamera->Update();

		cam_zoom += event.GetYOffset() * m_Timestep;
		m_EditorCamera->SetScale(cam_zoom);
		m_EditorCamera->Update();
	}
}
