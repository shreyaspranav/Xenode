#pragma once

#include <Core.h>
#include <glm/glm.hpp>

#include "Structs.h"

namespace Xen {
	
	enum class CameraType {Perspective, Orthographic};

	class XEN_API Camera
	{
	public:
		Camera(CameraType type, uint32_t framebuffer_width, uint32_t framebuffer_height);
		~Camera();

		void SetProjectionType(CameraType type);
		inline CameraType GetProjectionType()					{ return m_CameraType; }

		inline void SetPosition(const Vec3& postion)			{ m_CameraPosition = postion; }
		inline void SetRotation(float rotation)					{ m_RotationZ = rotation; m_Rotation.z = rotation; }
		inline void SetRotation(const Vec3& rotation)			{ m_Rotation = rotation; }

		inline void SetNearPoint(float near_point)				{ m_z_Near = near_point; }
		inline void SetFarPoint(float far_point)				{ m_z_Far = far_point; }

		inline void SetFovAngle(float fov_angle)				{ m_FovAngle = fov_angle; }

		inline void SetScale(float scale)						{ m_CameraScale = Vec3(scale); }
		inline void SetScale(const Vec3& scale)					{ m_CameraScale = scale; }

		inline const glm::mat4& GetProjectionMatrix()			{ return m_ProjectionMatrix; }
		inline const glm::mat4& GetViewMatrix()					{ return m_ViewMatrix; }
		inline const glm::mat4& GetViewProjectionMatrix()		{ return m_ViewProjectionMatrix; }

		inline float GetNearPoint()								{ return m_z_Near; }
		inline float GetFarPoint()								{ return m_z_Far; }

		inline float GetFovAngle()								{ return m_FovAngle; }

		inline Vec3& GetPosition()								{ return m_CameraPosition; }
		inline Vec3& GetRotation()								{ return m_Rotation; }
		inline Vec3& GetScale()									{ return m_CameraScale; }

		inline uint32_t GetFrameBufferWidth()					{ return m_FrameBufferWidth; }
		inline uint32_t GetFrameBufferHeight()					{ return m_FrameBufferHeight; }

		void Update(bool usedLookAt = 0);
		void UpdateOnlyPosition();
		void LookAtPoint(const Vec3& point, const Vec3& up = Vec3(0.0f, 1.0f, 0.0f));

		void OnViewportResize(uint32_t framebuffer_width, uint32_t framebuffer_height);
		static const glm::mat4& OnViewportResize(uint32_t framebuffer_width, uint32_t framebuffer_height, glm::mat4& view_projection, glm::mat4& view_mat);


	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		Vec3 m_CameraPosition;
		Vec3 m_Rotation;
		Vec3 m_CameraScale = Xen::Vec3(1.0f);

		CameraType m_CameraType;
		float m_AspectRatio;

		float m_RotationZ = 0.0f;

		float m_z_Near, m_z_Far;
		float m_FovAngle = 60.0f;

		uint32_t m_FrameBufferWidth = 1, m_FrameBufferHeight = 1;
	};
}
