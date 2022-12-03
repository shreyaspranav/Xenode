#include "pch"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include "core/app/Log.h"

namespace Xen {
	Camera::Camera(CameraType type, uint32_t framebuffer_width, uint32_t framebuffer_height) : m_CameraType(type)
	{
		if (type == CameraType::Orthographic)
		{
			m_z_Near = -1.0f;
			m_z_Far = 1.0f;
			m_AspectRatio = (float)framebuffer_width / (float)framebuffer_height;

			m_ProjectionMatrix = glm::ortho(-m_AspectRatio, m_AspectRatio, -1.0f, 1.0f, m_z_Near, m_z_Far);
		}

		else if (type == CameraType::Perspective)
		{
			m_z_Near = 0.01f;
			m_z_Far = 1000.0f;
			m_AspectRatio = (float)framebuffer_width / (float)framebuffer_height;

			m_ProjectionMatrix = glm::perspective(glm::radians(m_FovAngle), m_AspectRatio, m_z_Near, m_z_Far);
		}
		m_ViewMatrix = glm::mat4(1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	Camera::~Camera()
	{
	}
	void Camera::Update()
	{
		glm::mat4 mat;

		mat = glm::translate(glm::mat4(1.0f), m_CameraPosition.GetVec());
		mat = glm::rotate(mat, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
		mat = glm::rotate(mat, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
		mat = glm::rotate(mat, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));

		if (m_CameraType == CameraType::Perspective)
			mat = glm::scale(mat, glm::vec3(m_CameraScale.x, m_CameraScale.y, m_CameraScale.z));
		else
			mat = glm::scale(mat, glm::vec3(m_CameraScale.GetVec()));

		m_ViewMatrix = glm::inverse(mat);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::SetProjectionType(CameraType type)
	{
		m_CameraType = type;
		if (m_CameraType == CameraType::Orthographic)
		{
			m_z_Near = -1.0f;
			m_z_Far = 1.0f;

			m_ProjectionMatrix = glm::ortho(-m_AspectRatio, m_AspectRatio, -1.0f, 1.0f, m_z_Near, m_z_Far);
		}

		else if (m_CameraType == CameraType::Perspective)
		{
			m_z_Near = 0.01f;
			m_z_Far = 1000.0f;

			m_ProjectionMatrix = glm::perspective(glm::radians(m_FovAngle), m_AspectRatio, m_z_Near, m_z_Far);
		}

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::UpdateOnlyPosition()
	{
		m_ViewMatrix = glm::translate(m_ViewMatrix, m_CameraPosition.GetVec());
		m_ViewMatrix = glm::scale(m_ViewMatrix, glm::vec3(m_CameraScale.GetVec()));

		m_ViewMatrix = glm::inverse(m_ViewMatrix);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void Camera::OnViewportResize(uint32_t framebuffer_width, uint32_t framebuffer_height)
	{
		m_AspectRatio = (float)framebuffer_width / (float)framebuffer_height;

		if (m_CameraType == CameraType::Orthographic)
			m_ProjectionMatrix = glm::ortho(-m_AspectRatio, m_AspectRatio, -1.0f, 1.0f, m_z_Near, m_z_Far);
		
		else if (m_CameraType == CameraType::Perspective)
			m_ProjectionMatrix = glm::perspectiveLH(glm::radians(m_FovAngle), m_AspectRatio, m_z_Near, m_z_Far);
	}
}