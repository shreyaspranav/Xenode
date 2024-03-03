#pragma once

#include <glm/glm.hpp>
#include <Core.h>
#include <core/app/Log.h>

namespace Xen {

	// Just a Vec4 but with a different name.
	struct Color
	{
		float r, g, b, a;

		Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

		Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
		Color(const glm::vec4& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}

		Color(float value) : r(value), g(value), b(value), a(1.0f) {}				//Greyscale Color
		Color(float value, float alpha) : r(value), g(value), b(value), a(alpha) {}	//Greyscale Color

		glm::vec4 GetVec() const { return glm::vec4(r, g, b, a); }

		// Don't multiply alpha?
		Color operator*(float val)				{ return Color(r * val, g * val, b * val, a); }
		Color operator+(const Color& color)		{ return Color(r + color.r, g + color.g, b + color.b, a + color.a); }
		
		bool operator==(const Color& color)		{ return r == color.r && g == color.g && b == color.b && a == color.a; }
		// Color operator-(const Color& color)		{ return Color(r - color.r, g - color.g, b - color.b, a - color.a); }
	};

	struct ColorGradient
	{
		static const int32_t MAX_COLORS_IN_COLOR_GRADIENT = 8;

		struct ColorSlider 
		{
			Color color;

			// Value between 0 - 1:
			float sliderValue = 0.5f;

			ColorSlider() {}

			ColorSlider(const Color& color, float sliderValue) 
				:color(color), sliderValue(sliderValue) {}

			bool operator< (const ColorSlider& colorSlider) { return sliderValue <  colorSlider.sliderValue; }
			bool operator==(const ColorSlider& colorSlider) { return sliderValue == colorSlider.sliderValue; }
		};

		struct AlphaSlider
		{
			float alpha = 1.0f;

			// Value between 0 - 1:
			float sliderValue = 0.5f;

			AlphaSlider() {}

			AlphaSlider(float alpha, float sliderValue)
				:alpha(alpha), sliderValue(sliderValue) {}

			bool operator< (const AlphaSlider& alphaSlider) { return sliderValue <  alphaSlider.sliderValue; }
			bool operator==(const AlphaSlider& alphaSlider) { return sliderValue == alphaSlider.sliderValue; }

		};

	private:
		// std::array<std::pair<Color, float>, MAX_COLORS_IN_COLOR_GRADIENT> m_Colors;
		// std::array<std::pair<float, float>, MAX_COLORS_IN_COLOR_GRADIENT> m_Alphas;

		std::vector<ColorSlider> m_Colors;
		std::vector<AlphaSlider> m_Alphas;

	public:
		ColorGradient()
		{
			m_Colors.push_back({ Color(1.0f), 0.0f});
			m_Colors.push_back({ Color(0.0f), 1.0f});

			m_Alphas.push_back({ 1.0f, 0.0f });
			m_Alphas.push_back({ 0.0f, 1.0f });
		}

		ColorGradient(const Color& leftColor, const Color& rightColor)
		{
			m_Colors.push_back({ leftColor, 0.0f });
			m_Colors.push_back({ rightColor, 1.0f });

			m_Alphas.push_back({ 1.0f, 0.0f });
			m_Alphas.push_back({ 1.0f, 1.0f });
		}

		inline void AddColor(const Color& color, float sliderValue)
		{
			if (m_Colors.size() >= MAX_COLORS_IN_COLOR_GRADIENT)
				return;

			// Clamping of position from [0, 1]:
			sliderValue = std::max(std::min(sliderValue, 1.0f), 0.0f);

			m_Colors.push_back({ color, sliderValue });
		}
		inline void AddAlpha(float alpha, float sliderValue)
		{
			if (m_Alphas.size() >= MAX_COLORS_IN_COLOR_GRADIENT)
				return;

			// Clamping of sliderValue and alpha from [0, 1]:
			sliderValue = std::max(std::min(sliderValue, 1.0f), 0.0f);
			alpha		= std::max(std::min(alpha,		 1.0f), 0.0f);

			m_Alphas.push_back({ alpha, sliderValue });
		}

		inline void EditColor(float sliderValue, const Color& color) 
		{
			// Clamping of position from [0, 1]:
			sliderValue = std::max(std::min(sliderValue, 1.0f), 0.0f);

			for (auto& colorSlider : m_Colors)
				if (colorSlider.sliderValue == sliderValue)
					colorSlider.color = color;
		}

		inline void EditAlpha(float sliderValue, float alpha)
		{
			// Clamping of position and alpha from [0, 1]:
			sliderValue = std::max(std::min(sliderValue, 1.0f), 0.0f);
			alpha		= std::max(std::min(alpha,		 1.0f), 0.0f);

			for (auto& alphaSlider : m_Alphas)
				if (alphaSlider.sliderValue == sliderValue)
					alphaSlider.alpha = alpha;
		}

		inline void EditColorPosition(const Color& color, float sliderValue)
		{
			for (auto& colorSlider : m_Colors)
				if (colorSlider.color == color)
					colorSlider.sliderValue = sliderValue;
		}

		inline void EditAlphaPosition(float alpha, float sliderValue)
		{
			for (auto& alphaSlider : m_Alphas)
				if (alphaSlider.alpha == alpha)
					alphaSlider.sliderValue = sliderValue;
		}

		Color GetFinalColor(float sliderValue)
		{
			Color finalColor;
			float finalAlpha;

			std::sort(m_Colors.begin(), m_Colors.end());
			std::sort(m_Alphas.begin(), m_Alphas.end());

			// Calculate finalColor:
			auto firstColor = *(m_Colors.begin());  // std::pair, first -> position, second -> color
			auto lastColor  = *(m_Colors.end());

			if (sliderValue < firstColor.sliderValue)
				finalColor = firstColor.color;
			else if (sliderValue > lastColor.sliderValue)
				finalColor = lastColor.color;
			else {
				ColorSlider leftColor, rightColor;

				for (auto it = m_Colors.begin(); it != m_Colors.end() - 1; ++it) 
				{
					float currentSliderValue	= (*it).sliderValue;
					Color currentColor			= (*it).color;
					float nextSliderValue		= (*(it + 1)).sliderValue;
					Color nextColor				= (*(it + 1)).color;

					if (currentSliderValue <= sliderValue && nextSliderValue > sliderValue) {
						leftColor	= { currentColor, currentSliderValue };
						rightColor	= { nextColor, nextSliderValue };
						break;
					}
				}

				// normalize position between leftColor and rightColor and linearly interpolate between them
				float normalizedPosition = (rightColor.sliderValue - leftColor.sliderValue) * sliderValue;
				finalColor = leftColor.color * (1.0f - normalizedPosition) + rightColor.color* normalizedPosition;
			}

			auto firstAlpha		= *(m_Alphas.begin());  // std::pair, first -> position, second -> alpha
			auto lastAlpha		= *(m_Alphas.end());	

			if (sliderValue < firstAlpha.sliderValue)
				finalAlpha = firstAlpha.alpha;
			else if (sliderValue > lastAlpha.sliderValue)
				finalAlpha = lastAlpha.alpha;
			else {
				AlphaSlider leftAlpha, rightAlpha;

				for (auto&& it = m_Alphas.begin(); it != m_Alphas.end() - 1; ++it) 
				{
					float currentSliderValue	= (*it).sliderValue;
					float currentAlpha			= (*it).alpha;
					float nextSliderValue		= (*(it + 1)).sliderValue;
					float nextAlpha				= (*(it + 1)).alpha;

					if (currentSliderValue <= sliderValue && nextSliderValue > sliderValue) {
						leftAlpha	= { currentAlpha, currentSliderValue };
						rightAlpha	= { nextAlpha, nextSliderValue };
					}
				}

				// normalize position between leftAlpha and rightAlpha and linearly interpolate between them
				float normalizedPosition = (rightAlpha.sliderValue - leftAlpha.sliderValue) * sliderValue;
				finalAlpha = leftAlpha.alpha * (1.0f - normalizedPosition) + rightAlpha.alpha * normalizedPosition;
			}

			return Color(finalColor.r, finalColor.g, finalColor.b, finalAlpha);
		}

		inline auto ColorBegin()	{ return m_Colors.begin(); }
		inline auto ColorEnd()		{ return m_Colors.begin() + m_Colors.size(); }
		inline auto AlphaBegin()	{ return m_Alphas.begin(); }
		inline auto AlphaEnd()		{ return m_Alphas.begin() + m_Alphas.size(); }

		inline const std::vector<ColorSlider>& GetColorSliders() { return m_Colors; }
		inline const std::vector<AlphaSlider>& GetAlphaSliders() { return m_Alphas; }

		inline Size GetColorsSize()	{ return m_Colors.size(); }
		inline Size GetAlphasSize()	{ return m_Alphas.size(); }
	};

	struct Vec2
	{
		float x, y;

		Vec2() : x(0.0f), y(0.0f) {}

		Vec2(float value) : x(value), y(value) {}

		Vec2(float x, float y) : x(x), y(y) {}
		Vec2(const glm::vec2& vec) : x(vec.x), y(vec.y) {}

		glm::vec2 GetVec() const { return glm::vec2(x, y); }

		Vec2 operator+(const Vec2& vec) { return Vec2(x + vec.x, y + vec.y); }
		Vec2 operator-(const Vec2& vec) { return Vec2(x - vec.x, y - vec.y); }
		Vec2 operator*(float val)		{ return Vec2(x * val, y * val); }

	};

	struct Vec3
	{
		float x, y, z;

		Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

		Vec3(float value) : x(value), y(value), z(value) {}

		Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		Vec3(const glm::vec3& vec) : x(vec.x), y(vec.y), z(vec.z) {}

		glm::vec3 GetVec() const { return glm::vec3(x, y, z); }

		Vec3 operator+(const Vec3& vec) { return Vec3(x + vec.x, y + vec.y, z + vec.z); }
		Vec3 operator-(const Vec3& vec) { return Vec3(x - vec.x, y - vec.y, z - vec.z); }
		Vec3 operator*(float val)		{ return Vec3(x * val, y * val, z * val); }

	};

	struct Vec4
	{
		float x, y, z, w;

		Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

		Vec4(float value) : x(value), y(value), z(value), w(value) {}

		Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		Vec4(const glm::vec4& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

		glm::vec4 GetVec() const { return glm::vec4(x, y, z, w); }

		Vec4 operator+(const Vec4& vec) { return Vec4(x + vec.x, y + vec.y, z + vec.z, w + vec.w); }
		Vec4 operator-(const Vec4& vec) { return Vec4(x - vec.x, y - vec.y, z - vec.z, w - vec.w); }
		Vec4 operator*(float val)		{ return Vec4(x * val, y * val, z * val, w * val); }
	};
}