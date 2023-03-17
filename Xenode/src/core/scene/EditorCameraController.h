#pragma once
#include <Core.h>
#include <core/renderer/Structs.h>
#include <core/renderer/Camera.h>

namespace Xen {
	class XEN_API EditorCameraController 
	{
	public:
		EditorCameraController(Vec3& position, Vec3& focal_point, Vec2& angles_along_focal_point)
		{

		}
		~EditorCameraController() 
		{

		}

		void SetProjectionType(CameraType type)
		{
			
		}

		void Orbit()
		{

		}
	private:
		Vec3 m_FocalPoint;

		Vec3 m_CameraPosition;
		Vec3 m_CameraUpPosition;
		Vec3 m_CameraRightPosition;

		Vec3 m_CameraAngleAlongFocalPoint;

		float m_FocalDistance = 4.0f;
	};
}