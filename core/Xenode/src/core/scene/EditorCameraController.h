#pragma once
#include <Core.h>
#include <core/renderer/Structs.h>
#include <core/renderer/Camera.h>

namespace Xen {

	enum class EditorCameraType { _2D, _3D };

	class XEN_API EditorCameraController 
	{
	public:
		EditorCameraController() {}
		EditorCameraController(Ref<Input> input, EditorCameraType type = EditorCameraType::_3D)
			:m_Input(input), m_CameraType(type)
		{
			m_CameraAngleAlongFocalPoint = Xen::Vec2(90.0f, 0.0f);
			if (type == EditorCameraType::_2D)
				m_FocalDistance = 1.5f;
		}
		~EditorCameraController() 
		{

		}

		void Orbit(const Vec2& delta)
		{
			Vec2 delta_angle = (Vec2&)delta;
			m_CameraAngleAlongFocalPoint = m_CameraAngleAlongFocalPoint + (delta_angle * m_OrbitSpeed);
		}

		void Pan(const Vec2& delta)
		{
			m_FocalPoint.x = m_FocalPoint.x + (-delta.x * m_PanSpeed * m_CameraRightPosition.x * m_FocalDistance);
			m_FocalPoint.y = m_FocalPoint.y + ( delta.y * m_PanSpeed * m_CameraUpPosition.y * m_FocalDistance);
			m_FocalPoint.z = m_FocalPoint.z + (-delta.x * m_PanSpeed * m_CameraRightPosition.z * m_FocalDistance);
		}

		void Zoom(float delta)
		{
			m_FocalDistance += delta * m_ZoomSpeed; 

			if (m_FocalDistance < 0.01f)
				m_FocalDistance = 0.01f;
		}

		void Update(bool* active, uint32_t frameBufferHeight)
		{
			Xen::Vec2 mouse = Xen::Vec2(m_Input->GetMouseX(), m_Input->GetMouseY());
			m_MouseDelta = ((Vec2&)mouse - m_InitialMouseCoords) * (1.0f / (2.0f * frameBufferHeight));
			m_InitialMouseCoords = mouse;

			m_OrbitSpeed = frameBufferHeight;

			if (m_CameraType == EditorCameraType::_3D)
			{
				if (*active) {
					if (m_Input->IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) && m_Input->IsKeyPressed(KEY_LEFT_SHIFT))
						Pan(m_MouseDelta);
					else if (m_Input->IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
						Orbit(m_MouseDelta);
					else if (m_Input->IsKeyPressed(KEY_LEFT_CONTROL))
						Zoom(m_MouseDelta.y);
				}

				m_CameraPosition.x = m_FocalPoint.x + m_FocalDistance * glm::cos(glm::radians(m_CameraAngleAlongFocalPoint.y)) * glm::cos(glm::radians(-m_CameraAngleAlongFocalPoint.x));
				m_CameraPosition.y = m_FocalPoint.y + m_FocalDistance * glm::sin(glm::radians(m_CameraAngleAlongFocalPoint.y));
				m_CameraPosition.z = m_FocalPoint.z + m_FocalDistance * glm::cos(glm::radians(m_CameraAngleAlongFocalPoint.y)) * glm::sin(glm::radians(-m_CameraAngleAlongFocalPoint.x));

				m_CameraUpPosition.x = glm::cos(glm::radians(m_CameraAngleAlongFocalPoint.y + 90.0f)) * glm::cos(glm::radians(-m_CameraAngleAlongFocalPoint.x));
				m_CameraUpPosition.y = glm::sin(glm::radians(m_CameraAngleAlongFocalPoint.y + 90.0f));
				m_CameraUpPosition.z = glm::cos(glm::radians(m_CameraAngleAlongFocalPoint.y + 90.0f)) * glm::sin(glm::radians(-m_CameraAngleAlongFocalPoint.x));

				m_CameraRightPosition.x = -1.0f * glm::cos(glm::radians(m_CameraAngleAlongFocalPoint.y)) * glm::cos(glm::radians(-(m_CameraAngleAlongFocalPoint.x + 90.0f)));
				m_CameraRightPosition.y =  1.0f * glm::sin(glm::radians(m_CameraAngleAlongFocalPoint.y));
				m_CameraRightPosition.z = -1.0f * glm::cos(glm::radians(m_CameraAngleAlongFocalPoint.y)) * glm::sin(glm::radians(-(m_CameraAngleAlongFocalPoint.x + 90.0f)));
			}

			else if (m_CameraType == EditorCameraType::_2D)
			{
				if (*active) 
				{
					if (m_Input->IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
						Pan(m_MouseDelta);
					else if (m_Input->IsKeyPressed(KEY_LEFT_CONTROL))
						Zoom(m_MouseDelta.y);
				}

				m_CameraPosition = m_FocalPoint;
				m_CameraUpPosition     = Vec3(0.0f, 1.0f, 0.0f);
				m_CameraRightPosition  = Vec3(1.0f, 0.0f, 0.0f);
			}
		}

		inline const Vec2& GetMouseDelta()			{ return m_MouseDelta; }
		inline const Vec3& GetFocalPoint()			{ return m_FocalPoint; };
		inline const Vec3& GetCameraPosition()		{ return m_CameraPosition; };

		inline void SetCameraPosition(const Vec3& position) 
		{ 
			if (m_CameraType == EditorCameraType::_2D)
			{
				m_CameraPosition = position; 
				m_FocalPoint = position;
			}

			else 
			{
				// Find out a way of determining the focal distance and the direction.
			}
		}

		inline float GetFocalDistance()				{ return m_FocalDistance; }
		inline float GetZoom()						{ return m_FocalDistance; }

		inline void SetZoom(float zoom_value)		{ m_FocalDistance = zoom_value; }
		inline void SetCameraType(EditorCameraType type)	{ m_CameraType = type; }
	private:
		EditorCameraType m_CameraType;

		Vec3 m_FocalPoint;

		Vec3 m_CameraPosition;
		Vec3 m_CameraUpPosition;
		Vec3 m_CameraRightPosition;

		Vec2 m_CameraAngleAlongFocalPoint;
		Vec2 m_InitialMouseCoords;
		
		Vec2 m_MouseDelta;

		Ref<Input> m_Input;

		float m_FocalDistance = 4.0f;
		
		float m_OrbitSpeed = 100.0f;
		float m_PanSpeed = 3.7f;
		float m_ZoomSpeed = 9.0f;

		uint8_t zoom_iterations = 20;
	};
}