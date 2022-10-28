#pragma once

#include <glm/glm.hpp>

namespace Xen {
	struct Color
	{
		float r, g, b, a;

		Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

		Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
		Color(const glm::vec4& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}

		Color(float value) : r(value), g(value), b(value), a(1.0f) {}				//Greyscale Color
		Color(float value, float alpha) : r(value), g(value), b(value), a(alpha) {}	//Greyscale Color

		glm::vec4 GetVec() const { return glm::vec4(r, g, b, a); }
	};

	struct Vec2
	{
		float x, y;

		Vec2() : x(0.0f), y(0.0f) {}

		Vec2(float value) : x(value), y(value) {}

		Vec2(float x, float y) : x(x), y(y) {}
		Vec2(const glm::vec2& vec) : x(vec.x), y(vec.y) {}

		glm::vec2 GetVec() const { return glm::vec2(x, y); }

	};

	struct Vec3
	{
		float x, y, z;

		Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

		Vec3(float value) : x(value), y(value), z(value) {}

		Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		Vec3(const glm::vec3& vec) : x(vec.x), y(vec.y), z(vec.z) {}

		glm::vec3 GetVec() const { return glm::vec3(x, y, z); }

	};

	struct Vec4
	{
		float x, y, z, w;

		Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

		Vec4(float value) : x(value), y(value), z(value), w(value) {}

		Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		Vec4(const glm::vec4& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

		glm::vec4 GetVec() const { return glm::vec4(x, y, z, w); }

	};
}