#include "core/renderer/Texture.h"

namespace Xen 
{
	class XEN_API OpenGLTexture : public Texture2D
	{
	public:
		OpenGLTexture(const std::string& textureFilePath, bool flip_on_load);
		OpenGLTexture(TextureProperties properties, void* data, uint32_t size);
		OpenGLTexture(const Buffer& buffer, TextureBufferType bufferType, TextureProperties properties);
		OpenGLTexture(uint32_t rendererID, TextureProperties properties);

		virtual ~OpenGLTexture();

		void LoadTexture() override;

		void SetTextureWrapMode(TextureWrapMode mode) override;
		void SetTextureFilterMode(TextureFilterMode mode) override;

		inline uint32_t GetNativeTextureID() const override { return m_TextureID; }

		inline uint32_t GetWidth() const override                         { return m_TextureProperties.width; }
		inline uint32_t GetHeight() const override                        { return m_TextureProperties.height; }
		inline TextureProperties GetTextureProperties() const override    { return m_TextureProperties; }
		inline TextureWrapMode GetTextureWrapMode() const override        { return m_T_WrapMode; }
		inline TextureFilterMode GetTextureFilterMode() const override    { return m_T_FilterMode; }

		inline const std::string& GetFilePath() const override { return m_FilePath; }

		void Bind(uint8_t slot) const override;

		static void BindTextureExtID(uint32_t id, uint8_t slot);
		static void BindExtTextureToImageUnit(const Ref<Texture2D> texture, uint8_t slot, uint8_t mipLevel);
		static void BindExtTextureToImageUnit(uint32_t textureID, TextureFormat format, uint8_t slot, uint8_t mipLevel);

		static Ref<Texture2D> CopyTexture2D(const Ref<Texture2D>& texture);

		bool operator==(const Ref<Texture2D> texture) const override { return texture->GetNativeTextureID() == m_TextureID ? true : false; }

	private:
		std::string m_FilePath;
		uint32_t m_TextureID;

		TextureProperties m_TextureProperties;

		TextureWrapMode m_T_WrapMode = TextureWrapMode::Repeat;
		TextureFilterMode m_T_FilterMode = TextureFilterMode::Linear;
	};
}