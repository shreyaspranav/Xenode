#pragma once

#include <Core.h>
#include <glm/glm.hpp>

namespace Xen {
	class XEN_API OrthographicCamera
	{
	public:
		OrthographicCamera(uint32_t framebuffer_width, uint32_t framebuffer_height);
		~OrthographicCamera();

		inline void SetPosition(const glm::vec3& postion)		{ m_CameraPosition = postion; }
		inline void SetRotation(float rotation)					{ m_RotationZ = rotation; m_Rotation.z = rotation; }
		inline void SetRotation(const glm::vec3& rotation)		{ m_Rotation = rotation; }

		inline void SetScale(float scale)						{ m_CameraScale = scale; }

		inline const glm::mat4& GetProjectionMatrix()			{ return m_ProjectionMatrix; }
		inline const glm::mat4& GetViewMatrix()					{ return m_ViewMatrix; }
		inline const glm::mat4& GetViewProjectionMatrix()		{ return m_ViewProjectionMatrix; }

		void Update();
		void UpdateOnlyPosition();

		void OnWindowResize(uint32_t framebuffer_width, uint32_t framebuffer_height);

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		float m_RotationZ = 0.0f, m_CameraScale = 1.0f;
	};
}
