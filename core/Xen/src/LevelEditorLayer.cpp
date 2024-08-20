#include "LevelEditorLayer.h"
#include "core/scene/ScriptableEntity.h"

#include <core/app/Timer.h>
#include <core/app/Utils.h>
#include <core/renderer/RenderCommand.h>
#include <core/math/Math.h>

#include <ImGuizmo.h>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <project/ProjectManager.h>
#include <project/ProjectSerializer.h>

#include <core/app/GameApplication.h>
#include <core/app/input/MouseInput.h>
#include <core/app/input/keyboardInput.h>

#include <core/scene/SceneRuntime.h>
#include <core/scene/Components.h>

Xen::Vec2 mouseInitialPos;

bool operator&(const KeyTransformOperation& o1, const KeyTransformOperation& o2) { return static_cast<uint16_t>(o1) & static_cast<uint16_t>(o2); }

LevelEditorLayer::LevelEditorLayer()
{
	m_Timestep = 0.0f;
}

LevelEditorLayer::~LevelEditorLayer()
{
}

void LevelEditorLayer::OnAttach()
{
	m_GameMode = Xen::GetApplicationInstance()->GetGameType();

	// input = Xen::Input::GetInputInterface();
	// input->SetWindow(Xen::DesktopApplication::GetWindow());

	m_EditorCamera = std::make_shared<Xen::Camera>(
		m_EditorCameraType == Xen::EditorCameraType::_2D ? Xen::CameraType::Orthographic : Xen::CameraType::Perspective, 
		m_ViewportFrameBufferWidth, 
		m_ViewportFrameBufferHeight
	);

	Xen::ProjectSettings& projectSettings = Xen::ProjectManager::GetCurrentProject()->GetProjectSettings();

	m_EditorScene = std::make_shared<Xen::Scene>(Xen::SceneType::_2D);
	m_RuntimeScene = std::make_shared<Xen::Scene>(Xen::SceneType::_2D);
	m_ActiveScene = m_EditorScene;

	m_EditorCamera->Update();

	// To add native scripts to the entities
	//class CameraControlScript : public Xen::ScriptableEntity
	//{
	//private:
	//	Xen::Ref<Xen::Input> input;
	//public:
	//	void OnCreate() override{
	//		input = GetInput();
	//	}
	//
	//	void OnUpdate(double timestep) override{
	//		
	//	}
	//
	//	void OnDestroy() override{
	//
	//	}
	//};
	//quad_entity_1.AddComponent<Xen::Component::NativeScript>().Bind<CameraControlScript>(quad_entity_1);

	m_HierarchyPanel = SceneHierarchyPanel(m_ActiveScene);
	m_PropertiesPanel = PropertiesPanel(m_HierarchyPanel.GetSelectedEntity());

	Xen::ProjectSettings settings = Xen::ProjectManager::GetCurrentProject()->GetProjectSettings();
	Xen::ProjectProperties properties = Xen::ProjectManager::GetCurrentProject()->GetProjectProperties();
	
	// Get the active projects asset directory
	std::filesystem::path projectPath = Xen::ProjectManager::GetCurrentProjectPath();
	m_ContentBrowserPanel = ContentBrowserPanel(projectPath / settings.relAssetDirectory);

	m_SceneSettingsPanel = SceneSettingsPanel(m_ActiveScene);

	m_PropertiesPanel.SetTextureLoadDropType(m_ContentBrowserPanel.GetTextureLoadDropType());
	m_PropertiesPanel.SetScriptLoadDropType(m_ContentBrowserPanel.GetScriptLoadDropType());

	m_EditorCameraController = Xen::EditorCameraController(m_EditorCameraType);
	
	// Open the scene after initialising the editor camera controller because it uses the m_EditorCameraController object
	OpenScene((projectPath / settings.relStartScenePath).string());

	// Load all the resource textures:
	m_ResourceTextures.insert({ "Play",   Xen::Texture2D::CreateTexture2D("assets/textures/play.png",     false) });
	m_ResourceTextures.insert({ "Stop",   Xen::Texture2D::CreateTexture2D("assets/textures/stop.png",     false) });
	m_ResourceTextures.insert({ "Pause",  Xen::Texture2D::CreateTexture2D("assets/textures/pause.png",    false) });
	m_ResourceTextures.insert({ "Step",   Xen::Texture2D::CreateTexture2D("assets/textures/step.png",     false) });
	m_ResourceTextures.insert({ "2D",     Xen::Texture2D::CreateTexture2D("assets/textures/2d-quad.png",  false) });
	m_ResourceTextures.insert({ "3D",     Xen::Texture2D::CreateTexture2D("assets/textures/3d-cube.png",  false) });

	for (auto& [textureTag, texture] : m_ResourceTextures)
		texture->LoadTexture();

	// Assuming that m_EditorState is m_EditorState::Edit in the beginning
	m_PlayOrPause = m_ResourceTextures["Play"];

	// Assuming 2D view is the default:
	if (m_EditorCameraType == Xen::EditorCameraType::_2D)
	{
		m_2DOr3DView = m_ResourceTextures["2D"];
		Xen::RenderCommand::EnableDepthTest(false);
	}
	else {
		m_2DOr3DView = m_ResourceTextures["3D"];
		Xen::RenderCommand::EnableDepthTest(true);
	}

	// Xen::ProjectProperties properties;
	// properties.name = "default_project";
	// properties.gameType = Xen::GameType::_2D;
	// properties.scriptingLanguage = Xen::ScriptingLanguage::Lua;
	// 
	// std::filesystem::path path = "../../resources/projects";
	// 
	// Xen::ProjectManager::CreateProject(path, properties);

	// XEN_ENGINE_LOG_INFO("Project Exists: {0}", Xen::ProjectSerializer::IsValidProjectFile("../../resources/projects/default_project/default_project.xenproject"));
	// 
	// Xen::Ref<Xen::Project> p = Xen::ProjectManager::LoadProject("../../resources/projects/default_project/default_project.xenproject");
	// Xen::ProjectProperties props = p->GetProjectProperties();
	// Xen::ProjectSettings settings = p->GetProjectSettings();
	// 
	// XEN_ENGINE_LOG_INFO("Name: {0}", props.name);

	Xen::DesktopGameApplication* dGameApp = (Xen::DesktopGameApplication*)Xen::GetApplicationInstance();

	Xen::SceneRuntime::Initialize(dGameApp->GetGameWindow()->GetFrameBufferWidth(), dGameApp->GetGameWindow()->GetFrameBufferHeight());

	Xen::SceneRuntime::SetActiveScene(m_ActiveScene);
	Xen::SceneRuntime::SetAdditionalCamera(m_EditorCamera);

	m_SceneSettings.renderSource = Xen::RenderSource::AdditionalCamera;
}

void LevelEditorLayer::OnDetach()
{
	Xen::SceneRuntime::Finalize();
}

void LevelEditorLayer::OnUpdate(double timestep)
{
	m_EditorCameraController.SetCameraType(m_EditorCameraType);

	Xen::MousePointer p = Xen::MouseInput::GetMousePointer();
	Xen::Vec2 mouse = Xen::Vec2(p.x, p.y);
	Xen::Vec2 delta = (mouse - mouseInitialPos) * 0.3f;
	mouseInitialPos = mouse;

	bool active = true;

	Xen::RenderCommand::Clear();
	m_Timestep = timestep;
	
	Xen::SceneRuntime::Begin(m_SceneSettings);

	if (m_FirstRuntimeIteration)
	{
		Xen::SceneRuntime::ResizeFrameBuffer(m_ViewportFrameBufferWidth, m_ViewportFrameBufferHeight);
		m_FirstRuntimeIteration = false;
	}

	if (m_EditorState == EditorState::Edit)
	{
		// m_ActiveScene->SetMouseCoordinates(m_ViewportMousePos.x, m_ViewportMousePos.y);

		m_EditorCameraController.Update(&active, m_ViewportFrameBufferHeight);
		m_EditorCamera->SetPosition(m_EditorCameraController.GetCameraPosition());

		if (m_EditorCameraType == Xen::EditorCameraType::_2D) 
		{
			m_EditorCamera->SetScale({ m_EditorCameraController.GetFocalDistance(), m_EditorCameraController.GetFocalDistance(), 1.0f });
			m_EditorCamera->Update(false);
		}

		else if (m_EditorCameraType == Xen::EditorCameraType::_3D)
		{
			m_EditorCamera->LookAtPoint(m_EditorCameraController.GetFocalPoint());
			m_EditorCamera->Update(true);
		}

		Xen::SceneRuntime::Update(timestep);
		m_EditorScene = m_ActiveScene;
	}

	else if (m_EditorState == EditorState::Play || m_EditorState == EditorState::Pause) 
	{
		m_ActiveScene = m_RuntimeScene;

		// m_SceneStepped will be reset to false in OnFixedUpdate function
		if (m_SceneStepped)
		{
			Xen::SceneRuntime::UpdateRuntime(timestep, false);
			m_SceneStepped = false;
		}
		else
			Xen::SceneRuntime::UpdateRuntime(timestep, m_ScenePaused);
	}


	Xen::SceneRuntime::End();

# if 0
	// Line Rendering Test

	Xen::RenderCommand::SetLineWidth(1.0f);
	
	for (int i = -5; i < 6; i++)
	{
		Xen::Renderer2D::DrawLine(Xen::Vec3(-5.0f, i, 0.0f), Xen::Vec3(5.0f, i, 0.0f), Xen::Color(0.9f, 0.9f, 0.9f, 1.0f));
		Xen::Renderer2D::DrawLine(Xen::Vec3(i, -5.0f, 0.0f), Xen::Vec3(i, 5.0f, 0.0f), Xen::Color(0.9f, 0.9f, 0.9f, 1.0f));
	}
#endif


	if (Xen::MouseInput::IsMouseButtonPressed(Xen::MouseButtonCode::MOUSE_BUTTON_LEFT) && m_IsMouseHoveredOnViewport && m_IsMousePickingWorking)
	{
		Xen::SceneRuntime::GetActiveFrameBuffer()->Bind();

		// For some reason the red integer attachment is flipped!
		int entt_id = Xen::SceneRuntime::GetActiveFrameBuffer()->ReadIntPixel(1, m_ViewportMousePos.x, m_ViewportFrameBufferHeight - m_ViewportMousePos.y);
		m_HierarchyPanel.SetSelectedEntity(Xen::Entity((entt::entity)entt_id, m_ActiveScene.get()));

		Xen::SceneRuntime::GetActiveFrameBuffer()->Unbind();
	}
}
void LevelEditorLayer::OnRender()
{
	Xen::SceneRuntime::Render();
}


void LevelEditorLayer::OnImGuiUpdate()
{
	// Sets up the dockspace and determines the positions of the panels that are pre-docked:
	LevelEditorLayer::ImGuiSetupDockSpace();

	// Renders the menu bar (Also has code for the options in the menu bar: Like the open and save functionality).
	LevelEditorLayer::ImGuiRenderMenuBar();

	// Renders all the panels such as the Scene Hierarchy Panel, Properties Panel, Content Browser Panel, etc.
	LevelEditorLayer::ImGuiRenderPanels();

	// Renders the Viewport and the overlays in the viewport: Like the gizmos and whatnot.
	LevelEditorLayer::ImGuiRenderViewport();

	// Renders the main toolbar:
	LevelEditorLayer::ImGuiRenderToolbar();
}

// ImGui Rendering divided into chunks: ---------------------------------------------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LevelEditorLayer::ImGuiSetupDockSpace()
{
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// The corresponding ImGui::End() is in LevelEditorLayer::
	ImGui::Begin("DockSpace", nullptr, windowFlags);
	
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

		static auto first_time = true;
		if (first_time)
		{
			first_time = false;

			ImGui::DockBuilderRemoveNode(dockspaceID); // clear any previous layout
			ImGui::DockBuilderAddNode(dockspaceID, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

			// split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
			// window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id 
			// we want (which ever one we DON'T set as NULL, will be returned by the function)
			// out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction

			auto dock_id_left = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.22f, nullptr, &dockspaceID); // For the Scene hierarchy panel and properties panel
			auto dock_id_down = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down, 0.3f, nullptr, &dockspaceID);  // For the Content Browser panel
			auto dock_id_up = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Up, 0.065f, nullptr, &dockspaceID);    // For the toolbar

			auto dock_id_left_down = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.5f, nullptr, &dock_id_left);

			// we now dock our windows into the docking node we made above
			ImGui::DockBuilderDockWindow(m_HierarchyPanel.GetPanelTitle().c_str(), dock_id_left);
			ImGui::DockBuilderDockWindow(m_ContentBrowserPanel.GetPanelTitle().c_str(), dock_id_down);
			ImGui::DockBuilderDockWindow("##toolbar", dock_id_up);
			ImGui::DockBuilderDockWindow(m_SceneSettingsPanel.GetPanelTitle().c_str(), dock_id_left_down);
			ImGui::DockBuilderDockWindow(m_PropertiesPanel.GetPanelTitle().c_str(), dock_id_left_down);
			ImGui::DockBuilderDockWindow((std::string(ICON_FA_MOUNTAIN_SUN) + std::string("  2D Viewport")).c_str(), dockspaceID); // IMP: To Dock In Centre!! use directly 'dockspaceID'
			ImGui::DockBuilderFinish(dockspaceID);
		}
	}
}

void LevelEditorLayer::ImGuiRenderMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Xenode", NULL, false, false);
			if (ImGui::MenuItem("New"))
			{
				// TODO: Ask for a conformation menu:
				m_EditorScene->DestroyAllEntities();
				m_HierarchyPanel.SetActiveScene(m_ActiveScene);
			}
			if (ImGui::MenuItem("Open", "Ctrl+O"))
			{
				const std::string& filePath = Xen::Utils::OpenFileDialogOpen("Xenode 2D Scene (*.xen)\0*.*\0");

				if (!filePath.empty())
				{
					OpenScene(filePath);
					m_ActiveScene = m_EditorScene;

					m_HierarchyPanel.SetActiveScene(m_ActiveScene);
				}
			}

			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				const std::string& filePath = Xen::Utils::OpenFileDialogSave("Xenode 2D Scene (*.xen)\0*.*\0");

				if (!filePath.empty())
					SaveScene(filePath);

			}
			if (ImGui::MenuItem("Save As..")) {}

			ImGui::Separator();

			if (ImGui::BeginMenu("Options"))
			{
				static bool b = true;
				ImGui::Checkbox("Auto Save", &b);
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Quit", "Alt+F4")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) { ImGui::EndMenu(); }
		if (ImGui::BeginMenu("Project")) { ImGui::EndMenu(); }
		if (ImGui::BeginMenu("View")) { ImGui::EndMenu(); }
		if (ImGui::BeginMenu("Build")) { ImGui::EndMenu(); }
		if (ImGui::BeginMenu("Tools")) { ImGui::EndMenu(); }
		if (ImGui::BeginMenu("About")) { ImGui::EndMenu(); }
		ImGui::EndMenuBar();
	}

	// This is for ImGui::Begin() in LevelEditorLayer::ImGuiSetupDockspace()
	ImGui::End();
}

void LevelEditorLayer::ImGuiRenderPanels()
{
	m_HierarchyPanel.OnImGuiRender();
	m_ContentBrowserPanel.OnImGuiRender();
	m_SceneSettingsPanel.OnImGuiRender();
	m_PropertiesPanel.OnImGuiRender();

	// Temp:
	m_PropertiesPanel.SetActiveEntity(m_HierarchyPanel.GetSelectedEntity());
	// if (m_EditorState == EditorState::Edit)
	// else
	// 	m_PropertiesPanel.SetActiveEntity(m_ActiveScene->GetRuntimeEntity(m_HierarchyPanel.GetSelectedEntity(), m_ActiveScene));
}

void LevelEditorLayer::ImGuiRenderViewport()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin((std::string(ICON_FA_MOUNTAIN_SUN) + std::string("  2D Viewport")).c_str());

	m_IsMouseHoveredOnViewport = ImGui::IsWindowHovered();

	auto [sx, sy] = ImGui::GetCursorScreenPos();
	auto [mx, my] = ImGui::GetMousePos();

	m_ViewportMousePos.x = mx - sx;
	m_ViewportMousePos.y = my - sy;

	if (m_ViewportFrameBufferWidth != ImGui::GetContentRegionAvail().x || m_ViewportFrameBufferHeight != ImGui::GetContentRegionAvail().y)
	{
		if (ImGui::GetContentRegionAvail().x <= 0 || ImGui::GetContentRegionAvail().y <= 0) {}
		else {
			m_ViewportFrameBufferWidth = ImGui::GetContentRegionAvail().x;
			m_ViewportFrameBufferHeight = ImGui::GetContentRegionAvail().y;

			Xen::SceneRuntime::ResizeFrameBuffer(m_ViewportFrameBufferWidth, m_ViewportFrameBufferHeight);
			m_EditorCamera->OnViewportResize(m_ViewportFrameBufferWidth, m_ViewportFrameBufferHeight);
		}
	}

	// ImGui::Image((void*)m_ActiveScene->GetUnlitSceneFrameBuffer()->GetColorAttachmentRendererID(0), ImVec2(m_ViewportFrameBufferWidth, m_ViewportFrameBufferHeight), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::Image((void*)Xen::SceneRuntime::GetActiveFrameBuffer()->GetColorAttachmentRendererID(0), ImVec2(m_ViewportFrameBufferWidth, m_ViewportFrameBufferHeight), ImVec2(0, 1), ImVec2(1, 0));


	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_ContentBrowserPanel.GetSceneLoadDropType().c_str()))
		{
			if (m_EditorState != EditorState::Edit)
				m_EditorState = EditorState::Edit;

			std::string path = (const char*)payload->Data;
			uint32_t size = path.size();

			OpenScene(path);
			m_ActiveScene = m_EditorScene;

			m_HierarchyPanel.SetActiveScene(m_ActiveScene);
		}
		ImGui::EndDragDropTarget();
	}

	LevelEditorLayer::ImGuiRenderOverlay();
	ImGui::End();
	ImGui::PopStyleVar();
}

void LevelEditorLayer::ImGuiRenderOverlay()
{
	float y_offset = ImGui::GetWindowHeight() - m_ViewportFrameBufferHeight;
	m_IsMousePickingWorking = true;

	if (m_EditorState == EditorState::Edit)
	{
		Xen::Entity selectedEntity = m_HierarchyPanel.GetSelectedEntity();
		if (!selectedEntity.IsNull() && selectedEntity.IsValid())
		{
			if (ImGuizmo::IsOver())
				m_IsMousePickingWorking = false;

			if (m_EditorCameraType == Xen::EditorCameraType::_2D)
				ImGuizmo::SetGizmoSizeClipSpace(0.07f * m_EditorCameraController.GetFocalDistance());
			else
				ImGuizmo::SetGizmoSizeClipSpace(0.12f);

			ImGuizmo::SetOrthographic(m_EditorCamera->GetProjectionType() == Xen::CameraType::Orthographic ? true : false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + y_offset, m_ViewportFrameBufferWidth, m_ViewportFrameBufferHeight);

			glm::mat4 camera_view = m_EditorCamera->GetViewMatrix();
			glm::mat4 camera_projection = m_EditorCamera->GetProjectionMatrix();

			Xen::Component::Transform& entity_transform_comp = selectedEntity.GetComponent<Xen::Component::Transform>();

			glm::vec3 radians_rotation_vec = glm::vec3(glm::radians(entity_transform_comp.rotation.x),
				glm::radians(entity_transform_comp.rotation.y),
				glm::radians(entity_transform_comp.rotation.z));

			glm::mat4 entity_transform = glm::translate(glm::mat4(1.0f), entity_transform_comp.position.GetVec())
				* glm::toMat4(glm::quat(radians_rotation_vec))
				* glm::scale(glm::mat4(1.0f), entity_transform_comp.scale.GetVec());

			switch (m_GizmoOperation)
			{
			case GizmoOperation::Translate:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			case GizmoOperation::Rotate2D:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::ROTATE_Z, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			case GizmoOperation::Rotate3D:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::ROTATE_Z, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			case GizmoOperation::Scale:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::SCALE, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			}

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale, skew;
				glm::quat orientation;
				glm::vec4 perspective;

				glm::decompose(entity_transform, scale, orientation, translation, skew, perspective);

				rotation = glm::eulerAngles(orientation);

				rotation.x = glm::degrees(rotation.x);
				rotation.y = glm::degrees(rotation.y);
				rotation.z = glm::degrees(rotation.z);

				glm::vec3 delta_rotation = rotation - entity_transform_comp.rotation.GetVec();

				entity_transform_comp.rotation.x += delta_rotation.x;
				entity_transform_comp.rotation.y += delta_rotation.y;
				entity_transform_comp.rotation.z += delta_rotation.z;

				entity_transform_comp.position = translation;
				entity_transform_comp.scale = scale;
			}

			// Key Transform Operations:
			// TODO: Work on the speed of the translation thing:
			// TODO: Rotation for X and Y axis:

			float speed = 0.01f * m_EditorCameraController.GetFocalDistance();

			if (m_KeyTransformOperation != KeyTransformOperation::None)
			{
				m_IsMousePickingWorking = false;
				auto& transformComp = selectedEntity.GetComponent<Xen::Component::Transform>();
				if (m_KeyTransformOperation & KeyTransformOperation::TranslateX)
				{
					if (m_EditorCameraType == Xen::EditorCameraType::_2D)
						transformComp.position.x += m_EditorCameraController.GetMouseDelta().x * speed;
				}

				if (m_KeyTransformOperation & KeyTransformOperation::TranslateY)
				{
					if (m_EditorCameraType == Xen::EditorCameraType::_2D)
						transformComp.position.y -= m_EditorCameraController.GetMouseDelta().y * speed;
				}

				if (m_KeyTransformOperation & KeyTransformOperation::RotateZ)
					transformComp.rotation.z -= m_EditorCameraController.GetMouseDelta().y;

				if (m_KeyTransformOperation & KeyTransformOperation::ScaleX)
					transformComp.scale.x += m_EditorCameraController.GetMouseDelta().x * speed;
				if (m_KeyTransformOperation & KeyTransformOperation::ScaleY)
					transformComp.scale.y += m_EditorCameraController.GetMouseDelta().x * speed;

				if (m_KeyTransformOperation != KeyTransformOperation::None && Xen::MouseInput::IsMouseButtonPressed(Xen::MouseButtonCode::MOUSE_BUTTON_LEFT))
				{
					m_KeyTransformOperation = KeyTransformOperation::None;
					m_IsMousePickingWorking = true;
				}
			}
		}
	}
}

void LevelEditorLayer::ImGuiRenderToolbar()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGuiWindowClass windowClass;
	windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
	ImGui::SetNextWindowClass(&windowClass);

	ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	ImGui::Begin("##toolbar", nullptr, toolbarFlags);

	ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

	float avail = ImGui::GetContentRegionAvail().x;

	if (ImGui::ImageButton((ImTextureID)m_2DOr3DView->GetNativeTextureID(), { 25.0f, 25.0f }))
	{
		if (m_2DOr3DView == m_ResourceTextures["2D"])
		{
			m_2DOr3DView = m_ResourceTextures["3D"];
			m_EditorCameraType = Xen::EditorCameraType::_3D;
			m_EditorCamera->SetProjectionType(Xen::CameraType::Perspective);
			Xen::RenderCommand::EnableDepthTest(true);
		}
		else {
			m_2DOr3DView = m_ResourceTextures["2D"];
			m_EditorCameraType = Xen::EditorCameraType::_2D;
			m_EditorCamera->SetProjectionType(Xen::CameraType::Orthographic);
			Xen::RenderCommand::EnableDepthTest(false);
		}
	}

	ImGui::SameLine();

	ImGuiStyle& style = ImGui::GetStyle();
	float width = 0.0f;
	width += 25.0f;			// Play/Pause Button
	width += style.ItemSpacing.x;
	width += 25.0f;			// Stop Button
	width += style.ItemSpacing.x;
	width += 25.0f;			// Step Button
	width += style.ItemSpacing.x;

	float off = (avail - width) * 0.5f;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

	if (ImGui::ImageButton((ImTextureID)m_PlayOrPause->GetNativeTextureID(), { 25.0f, 25.0f }))
	{
		if (m_PlayOrPause == m_ResourceTextures["Play"])
		{
			m_PlayOrPause = m_ResourceTextures["Pause"];
			OnScenePlay();
		}
		else {
			m_PlayOrPause = m_ResourceTextures["Play"];
			m_EditorState = EditorState::Pause;

			OnScenePause();
		}

		m_EditMode = false;
	}

	ImGui::SameLine();

	ImGui::BeginDisabled(m_EditMode); 

	if (ImGui::ImageButton((ImTextureID)m_ResourceTextures["Stop"]->GetNativeTextureID(), { 25.0f, 25.0f }))
	{
		m_EditorState = EditorState::Edit;
		m_EditMode = true;
		m_PlayOrPause = m_ResourceTextures["Play"];

		OnSceneStop();
	}

	ImGui::SameLine();

	ImGui::BeginDisabled(m_EditorState != EditorState::Pause);
	if (ImGui::ImageButton((ImTextureID)m_ResourceTextures["Step"]->GetNativeTextureID(), { 25.0f, 25.0f }))
	{
		m_SceneStepped = true;
	}
	ImGui::EndDisabled();
	ImGui::EndDisabled();

	ImGui::PopStyleColor();

	ImGui::End();

	ImGui::PopStyleVar();
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LevelEditorLayer::OnFixedUpdate()
{
	if (m_EditorState == EditorState::Play || m_EditorState == EditorState::Pause)
	{
		if (m_SceneStepped)
			Xen::SceneRuntime::FixedUpdate();
		else
			Xen::SceneRuntime::FixedUpdate();
	}
}

void LevelEditorLayer::OnEvent(Xen::Event& event)
{
	Xen::EventDispatcher::Dispatch<Xen::KeyboardEvent>(event, XEN_BIND_FN(LevelEditorLayer::OnKeyboardEvent));

	Xen::EventDispatcher::Dispatch<Xen::MouseButtonEvent>(event, XEN_BIND_FN(LevelEditorLayer::OnMouseButtonEvent));
	Xen::EventDispatcher::Dispatch<Xen::MouseMoveEvent>(event, XEN_BIND_FN(LevelEditorLayer::OnMouseMoveEvent));
	Xen::EventDispatcher::Dispatch<Xen::MouseScrollEvent>(event, XEN_BIND_FN(LevelEditorLayer::OnMouseScrollEvent));
}

void LevelEditorLayer::OnScenePlay()
{
	m_ScenePaused = false;

	if (m_EditorState != EditorState::Pause)
	{
		Xen::SceneUtils::CopyScene(m_RuntimeScene, m_EditorScene);
		m_ActiveScene = m_RuntimeScene;
		Xen::SceneRuntime::SetActiveScene(m_ActiveScene);
		Xen::SceneRuntime::RuntimeBegin();
	}
	m_HierarchyPanel.SetActiveScene(m_RuntimeScene);

	m_EditorState = EditorState::Play;

	// By default, run with the runtime camera
	// if no primary camera entities, render from the editor camera itself:
	m_SceneSettings.renderSource = Xen::RenderSource::RuntimeCamera;

	m_FirstRuntimeIteration = true;
}

void LevelEditorLayer::OnSceneStop()
{
	Xen::SceneRuntime::RuntimeEnd();

	m_ActiveScene->DestroyAllEntities();
	m_ActiveScene = m_EditorScene;
	
	Xen::SceneRuntime::SetActiveScene(m_ActiveScene);
	m_HierarchyPanel.SetActiveScene(m_ActiveScene);

	// By default, edit with the additional camera:
	m_SceneSettings.renderSource = Xen::RenderSource::AdditionalCamera;
}

void LevelEditorLayer::OnScenePause()
{
	m_ScenePaused = true;
}

void LevelEditorLayer::OpenScene(const std::string& filePath)
{
	m_EditorScene->DestroyAllEntities();
	Xen::Component::Transform editorCameraTransform = Xen::SceneSerializer::Deserialize(m_EditorScene, filePath);

	m_EditorCameraController.SetCameraPosition(editorCameraTransform.position);
	m_EditorCameraController.SetZoom(editorCameraTransform.scale.x);

	m_EditorCamera->Update();
}

void LevelEditorLayer::SaveScene(const std::string& filePath)
{
	Xen::Component::Transform editorCameraTransform;
	editorCameraTransform.position = m_EditorCamera->GetPosition();
	editorCameraTransform.rotation = m_EditorCamera->GetRotation();
	editorCameraTransform.scale = m_EditorCamera->GetScale();

	Xen::SceneSerializer::Serialize(m_EditorScene, editorCameraTransform, filePath);
}

void LevelEditorLayer::OnWindowResizeEvent(Xen::WindowResizeEvent& event)
{
	m_EditorCamera->OnViewportResize(m_ViewportFrameBufferWidth, m_ViewportFrameBufferHeight);
	Xen::SceneRuntime::ResizeFrameBuffer(m_ViewportFrameBufferWidth, m_ViewportFrameBufferHeight);
}

void LevelEditorLayer::OnMouseScrollEvent(Xen::MouseScrollEvent& event)
{

}

void LevelEditorLayer::OnMouseMoveEvent(Xen::MouseMoveEvent& event)
{

}

void LevelEditorLayer::OnMouseButtonEvent(Xen::MouseButtonEvent& event)
{

}

void LevelEditorLayer::OnKeyboardEvent(Xen::KeyboardEvent& event)
{
	if (m_IsMouseHoveredOnViewport)
	{
		switch (event.GetKey())
		{
		case Xen::KeyboardKeyCode::KEY_Q:
			m_GizmoOperation = GizmoOperation::Translate;
			break;
		case Xen::KeyboardKeyCode::KEY_W:
			m_GizmoOperation = m_EditorCamera->GetProjectionType() == Xen::CameraType::Orthographic ? GizmoOperation::Rotate2D : GizmoOperation::Rotate3D;
			break;
		case Xen::KeyboardKeyCode::KEY_E:
			m_GizmoOperation = GizmoOperation::Scale;
			break;

		// Setting Up Key transformations here:
		case Xen::KeyboardKeyCode::KEY_T:
			m_KeyTransformOperation = KeyTransformOperation::Translate;
			break;
		case Xen::KeyboardKeyCode::KEY_R:
			m_KeyTransformOperation = m_GameMode == Xen::GameType::_2D ? KeyTransformOperation::Rotate2D : KeyTransformOperation::Rotate;
			break;
		case Xen::KeyboardKeyCode::KEY_S:
			m_KeyTransformOperation = m_GameMode == Xen::GameType::_2D ? KeyTransformOperation::Scale2D : KeyTransformOperation::Scale;
			break;

		case Xen::KeyboardKeyCode::KEY_X:
			if (m_KeyTransformOperation & TranslateX || m_KeyTransformOperation & TranslateY || m_KeyTransformOperation & TranslateZ)
				m_KeyTransformOperation = TranslateX;
			else if (m_KeyTransformOperation & RotateX || m_KeyTransformOperation & RotateY || m_KeyTransformOperation & RotateZ)
				m_KeyTransformOperation = RotateX;
			else if (m_KeyTransformOperation & ScaleX || m_KeyTransformOperation & ScaleY || m_KeyTransformOperation & ScaleZ)
				m_KeyTransformOperation = ScaleX;
			break;
		case Xen::KeyboardKeyCode::KEY_Y:
			if (m_KeyTransformOperation & TranslateX || m_KeyTransformOperation & TranslateY || m_KeyTransformOperation & TranslateZ)
				m_KeyTransformOperation = TranslateY;
			else if (m_KeyTransformOperation & RotateX || m_KeyTransformOperation & RotateY || m_KeyTransformOperation & RotateZ)
				m_KeyTransformOperation = RotateY;
			else if (m_KeyTransformOperation & ScaleX || m_KeyTransformOperation & ScaleY || m_KeyTransformOperation & ScaleZ)
				m_KeyTransformOperation = ScaleY;
			break;
		case Xen::KeyboardKeyCode::KEY_Z:
			break;
		}
	}	
}
