#pragma once
#include <Core.h>

#include <core/renderer/FrameBuffer.h>
#include <core/renderer/Shader.h>
#include <core/renderer/Buffer.h>


namespace Xen {
	enum class PostEffect
	{
		// Will implement more...
		Bloom, ColorGrading
	};

	class PostProcessEffect
	{
	public:
		friend class PostProcessPipeline;
		virtual ~PostProcessEffect() = default;

		virtual inline PostEffect GetPostEffectType() const = 0;
	private:
		virtual void ComputeProcess(const Ref<FrameBuffer>& inputFB, const Ref<FrameBuffer>& outputFB, uint32_t width, uint32_t height) = 0;
	};

	struct BloomProperties
	{
		float threshold, intensity;
	};

	class BloomEffect : public PostProcessEffect
	{
	public:
		BloomEffect(BloomProperties* properties);
		virtual ~BloomEffect();

		inline PostEffect GetPostEffectType() const override	{ return PostEffect::Bloom; };

	private:
		void ComputeProcess(const Ref<FrameBuffer>& inputFB, const Ref<FrameBuffer>& outputFB, uint32_t width, uint32_t height) override;
		BloomProperties* m_Properties;

		Ref<ComputeShader> m_DownSampleShader;
		Ref<ComputeShader> m_UpSampleShader;

		Ref<Texture2D> m_DownSampledTexture;
		Ref<Texture2D> m_UpSampledTexture;

		std::vector<Ref<Texture2D>> m_DownSampledTextures;
		std::vector<Ref<Texture2D>> m_UpSampledTextures;

		uint32_t m_FrameBufferWidth = 1, m_FrameBufferHeight = 1;

		const std::string m_UpSampleShaderPath = "assets/shaders/upsample.cshader";
		const std::string m_DownSampleShaderPath = "assets/shaders/downsample.cshader";

		Ref<Texture2D> m_InputTexture;
		Ref<Texture2D> m_OutputTexture;

		Ref<UniformBuffer> m_BloomSettingsBuffer;

		struct {
			bool i = 1.0;
			int currentMipLevel = 1;
			Vec2 texelSize;
		}bs;

		const uint8_t m_SamplePasses = 10;
	};

	class PostProcessPipeline
	{
	public:
		static void AddPostEffects(const std::vector<Ref<PostProcessEffect>>& effects);
		static void ProcessPostEffects(const Ref<FrameBuffer>& inputFB, const Ref<FrameBuffer>& outputFB, const std::vector<bool>& enabled);
		static Ref<Texture2D> GetOutputTexture();

		static void OnFrameBufferResize(uint32_t frameBufferWidth, uint32_t frameBufferHeight);
	};
}