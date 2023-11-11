#include "pch"
#include "PostProcessPipeline.h"

#include "core/app/Log.h"

namespace Xen {

	// BloomEffect -------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------------

	BloomEffect::BloomEffect(BloomProperties* properties)
		:m_Properties(properties)
	{
		m_DownSampleShader = ComputeShader::CreateComputeShader(m_DownSampleShaderPath);
		m_DownSampleShader->LoadShader();

		m_UpSampleShader = ComputeShader::CreateComputeShader(m_UpSampleShaderPath);
		m_UpSampleShader->LoadShader();

		TextureProperties p;
		p.width = 1;
		p.height = 1;
		p.format = TextureFormat::R11G11B10F;
		p.mipLevels = m_SamplePasses;

		m_DownSampledTexture = Texture2D::CreateTexture2D(p, nullptr, 0);
		m_UpSampledTexture = Texture2D::CreateTexture2D(p, nullptr, 0);

		for (int i = 0; i < m_SamplePasses; i++)
		{
			m_DownSampledTextures.push_back(Texture2D::CreateTexture2D(p, nullptr, 0));
			m_DownSampledTextures[i]->SetTextureFilterMode(TextureFilterMode::Linear);
		}
		for (int i = 0; i < m_SamplePasses; i++)
		{
			m_UpSampledTextures.push_back(Texture2D::CreateTexture2D(p, nullptr, 0));
			m_UpSampledTextures[i]->SetTextureFilterMode(TextureFilterMode::Linear);
		}

		VertexBufferLayout l;
		m_BloomSettingsBuffer = UniformBuffer::CreateUniformBuffer(16, l, 7);
	}

	BloomEffect::~BloomEffect()
	{

	}

	void BloomEffect::ComputeProcess(const Ref<FrameBuffer>& inputFB, const Ref<FrameBuffer>& outputFB, uint32_t width, uint32_t height)
	{
		if (m_FrameBufferWidth != width || m_FrameBufferHeight != height)
		{
			m_DownSampledTexture = Texture2D::CreateTexture2D(
				{ width, height, TextureFormat::R11G11B10F, m_SamplePasses },
				nullptr,
				0);
			m_UpSampledTexture = Texture2D::CreateTexture2D(
					{ width, height, TextureFormat::R11G11B10F, m_SamplePasses },
					nullptr,
					0
			);

			m_UpSampledTexture->SetTextureWrapMode(TextureWrapMode::ClampToEdge);
			for (int i = 0; i < m_SamplePasses; i++)
			{
				m_DownSampledTextures[i] = Texture2D::CreateTexture2D(
					{ width / (uint32_t)pow(2, i + 1), height / (uint32_t)pow(2, i + 1), TextureFormat::R11G11B10F, 0},
					nullptr, 
					0);

				m_UpSampledTextures[i] = Texture2D::CreateTexture2D(
					{ width / (uint32_t)pow(2, (m_SamplePasses - i) - 1), height / (uint32_t)pow(2, (m_SamplePasses - i) - 1), TextureFormat::R11G11B10F, 0 },
					nullptr,
					0);

				m_DownSampledTextures[i]->SetTextureFilterMode(TextureFilterMode::Linear);
				m_UpSampledTextures[i]->SetTextureFilterMode(TextureFilterMode::Linear);

				m_DownSampledTextures[i]->SetTextureWrapMode(TextureWrapMode::ClampToEdge);
				m_UpSampledTextures[i]->SetTextureWrapMode(TextureWrapMode::ClampToEdge);

				XEN_ENGINE_LOG_WARN("DEADLOCK");
			}

			m_FrameBufferWidth = width;
			m_FrameBufferHeight = height;
		}

		// Downsampling
		{
			for (int i = 0; i < m_SamplePasses - 1; i++)
			{
				bs.i = i == 0 ? true : false;
				bs.currentMipLevel = i;
			
				m_BloomSettingsBuffer->Put(0, &bs, sizeof(bs));
			
				Texture2D::BindTexture(inputFB->GetColorAttachmentRendererID(0), 0);
				Texture2D::BindToImageUnit(inputFB->GetColorAttachmentRendererID(0), TextureFormat::R11G11B10F, 1, i + 1);
			
				//Texture2D::BindToImageUnit(m_DownSampledTexture, 0, i);
				//m_DownSampledTexture->Bind(0);
				//Texture2D::BindToImageUnit(inputFB->GetColorAttachmentRendererID(0), TextureFormat::R11G11B10F, 1, i + 1);
			
				m_DownSampleShader->DispatchCompute(width / (uint32_t)pow(2, i), height / (uint32_t)pow(2, i), 1);
			}
		}

		 // Upsampling
		{
			for (int i = m_SamplePasses - 2; i >= 0; i--)
			{
				Texture2D::BindTexture(inputFB->GetColorAttachmentRendererID(0), 0);
				Texture2D::BindToImageUnit(inputFB->GetColorAttachmentRendererID(0), TextureFormat::R11G11B10F, 1, i);
		
				//if (i == 0)
				//	Texture2D::BindToImageUnit(outputFB->GetColorAttachmentRendererID(0), TextureFormat::R11G11B10F, 1, 0);
				Texture2D::BindToImageUnit(inputFB->GetColorAttachmentRendererID(0), TextureFormat::R11G11B10F, 2, i);
		
		
				bs.currentMipLevel = i + 1;
				bs.texelSize = { 1.0f / (width / (uint32_t)pow(2, i + 1)), 1.0f / (width / (uint32_t)pow(2, i + 1)) };
		
				m_BloomSettingsBuffer->Put(0, &bs, sizeof(bs));
		
				m_UpSampleShader->DispatchCompute(width / (uint32_t)pow(2, i), height / (uint32_t)pow(2, i), 1);
			}
		}

	}

	// PostProcessPipline -----------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------

	std::vector<Ref<PostProcessEffect>> s_Effects;
	uint32_t s_Width, s_Height;

	void PostProcessPipeline::AddPostEffects(const std::vector<Ref<PostProcessEffect>>& effects)
	{
		s_Effects = effects;
	}
	void PostProcessPipeline::ProcessPostEffects(const Ref<FrameBuffer>& inputFB, const Ref<FrameBuffer>& outputFB, const std::vector<bool>& enabled)
	{
		for (int i = 0; i < s_Effects.size(); i++)
			if (enabled[i])
				s_Effects[i]->ComputeProcess(inputFB, outputFB, s_Width, s_Height);
	}
	void PostProcessPipeline::OnFrameBufferResize(uint32_t frameBufferWidth, uint32_t frameBufferHeight)
	{
		s_Width = frameBufferWidth;
		s_Height = frameBufferHeight;
	}
	Ref<Texture2D> PostProcessPipeline::GetOutputTexture()
	{
		return Ref<Texture2D>();
	}
}