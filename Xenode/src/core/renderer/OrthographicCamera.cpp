#include "pch"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include "core/app/Log.h"

namespace Xen {
	OrthographicCamera::OrthographicCamera(uint32_t framebuffer_width, uint32_t framebuffer_height)
	{
		m_ProjectionMatrix = glm::ortho(-(float)framebuffer_width/framebuffer_height, (float)framebuffer_width / framebuffer_height, -1.0f, 1.0f, -1.0f, 1.0f);
		m_ViewMatrix = glm::mat4(1.0f);

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	OrthographicCamera::~OrthographicCamera()
	{
	}
	void OrthographicCamera::Update()
	{
		glm::mat4 mat;

		mat = glm::translate(glm::mat4(1.0f), m_CameraPosition.GetVec());
		mat = glm::rotate(mat, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
		mat = glm::rotate(mat, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
		mat = glm::rotate(mat, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));

		mat = glm::scale(mat, glm::vec3(m_CameraScale, m_CameraScale, 1.0f));

		m_ViewMatrix = glm::inverse(mat);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void OrthographicCamera::UpdateOnlyPosition()
	{
		m_ViewMatrix = glm::translate(m_ViewMatrix, m_CameraPosition.GetVec());
		m_ViewMatrix = glm::scale(m_ViewMatrix, glm::vec3(m_CameraScale, m_CameraScale, 1.0f));

		m_ViewMatrix = glm::inverse(m_ViewMatrix);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void OrthographicCamera::OnWindowResize(uint32_t framebuffer_width, uint32_t framebuffer_height)
	{
		m_ProjectionMatrix = glm::ortho(-(float)framebuffer_width / framebuffer_height, (float)framebuffer_width / framebuffer_height, -1.0f, 1.0f, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}