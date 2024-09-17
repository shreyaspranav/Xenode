#include "pch"
#include "ParticlesRenderer2D.h"
#include "Renderer2D.h"

#include "QueryObject.h"
#include "BufferObjectBindings.h"
#include "Shader.h"
#include "RenderCommand.h"

namespace Xen
{
	// Global Settings: ---------------------------------------------------------------------------------------------------------------

	// The maximum amount of particles stored in the vertex buffer:
	constexpr uint32_t maxParticleBufferCount = 1000000;

	// The particle emitter shader and particle renderer shader path that is used to render all the particles:
	// TODO: Refactor the code when the engine uses an actual asset system
	// UPDATE TODO: The "Common Resources" should be an asset pack and the shader should be loaded from that.
	const std::string particleEmitterShaderPath  = std::string(COMMON_RESOURCES) + "/shaders/Renderer2D/ParticleEmitter2D.shader";
	const std::string particleRendererShaderPath = std::string(COMMON_RESOURCES) + "/shaders/Renderer2D/ParticleRenderer2D.shader";

	// ---------------------------------------------------------------------------------------------------------------------------------

	// Structs section: ----------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------------------

	// Particle struct. Represents a single particle.
	struct Particle
	{
		Vec3 position;
		Vec3 originalGenPosition;
		Vec2 velocity;
		Vec2 size;
		Vec4 color;
		float currentLifetime, totalLifetime;
		uint32_t particleType;
	};

	enum class ParticleType : uint32_t { Generator = 0, Normal = 1 };

	// For Uniform Buffer m_PerFrameData
	struct PerFrameData {
		float frameTime;

		Vec3 __DUMMY_OFFSET_1__;

		Vec3 randomSeed;
		uint32_t numToGenerate;
	};

	struct ColorSlider
	{
		Color color;
		float sliderValue;
		Vec3 __DUMMY_PADDING__;
	};

	struct AlphaSlider
	{
		float alpha;
		float sliderValue;
		Vec2 __DUMMY_PADDING__;
	};

	struct ColorGradientShaderType
	{
		ColorSlider colors[8];
		AlphaSlider alphas[8];
		uint32_t colorSliderSize, alphaSliderSize;
	};

	// For Uniform Buffer m_ParticleSettings
	struct ParticleEmitterSettings
	{
		Vec3 genPosition;
		float __DUMMY_OFFSET_1__;

		ColorGradientShaderType colorGradient;
		Vec2 __DUMMY_OFFSET_2__;

		// Used to represent AffectColorBy
		uint32_t affectColorBy;
		float colorDistanceScale;
		Vec2 __DUMMY_OFFESET_3__;

		Color startColor, endColor;
		Vec2 lifeRange, sizeRange, velocityRange;
		float velocityDamping;

		float __DUMMY_OFFESET_4__;
	};

	struct ParticlesRenderer2DStorage
	{
		Ref<Shader> emitterShader, rendererShader;

		Ref<UniformBuffer> perFrameData, particleEmitterSettings;
		PerFrameData perFrameDataStruct;
		ParticleEmitterSettings particleEmitterSettingsStruct;

		Ref<TransformFeedback> transformFeedback;

		Ref<QueryObject> queryObject;

		std::vector<Ref<ParticleInstance2D>> particleInstances;
		uint32_t particleInstanceIndex = 0;
		uint32_t particleInstanceCount = 0;

	}particlesRendererStorage;

	// Output Attributes from the geometry shader of the particle_emitter shader program:
	std::vector<std::string> outAttributes = 
	{
		"outPosition",
		"outOriginalGenPosition",
		"outVelocity",
		"outSize",
		"outColor",
		"outCurrentLifetime",
		"outTotalLifetime",
		"outParticleType"
	};

	// Random Number Generation:
	std::random_device randomDevice;
	std::mt19937 randomEngine = std::mt19937(randomDevice());
	std::uniform_real_distribution<float> realRandom;

	// Private functions: ----------------------------------------------------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------------------------------------

	static void InitializeBuffers(const Ref<ParticleInstance2D>& particleInstance)
	{
		if (particleInstance->buffersInitialized)
			return;

		VertexBufferLayout particleVertexBufferLayout = {
			{ "aPosition",					VertexBufferDataType::Float3,			0 },
			{ "aOriginalGenPosition",		VertexBufferDataType::Float3,			1 },
			{ "aVelocity",					VertexBufferDataType::Float2,			2 },
			{ "aSize",						VertexBufferDataType::Float2,			3 },
			{ "aColor",						VertexBufferDataType::Float4,			4 },
			{ "aCurrentLifetime",			VertexBufferDataType::Float,			5 },
			{ "aTotalLifetime",				VertexBufferDataType::Float,			6 },
			{ "aParticleType",				VertexBufferDataType::UnsignedInt,		7 }
		};

		Particle initParticle;
		initParticle.particleType = (uint32_t)ParticleType::Generator;

		for (int i = 0; i < 2; i++)
		{
			particleInstance->particleReadWriteBuffer[i] = VertexBuffer::CreateVertexBuffer(maxParticleBufferCount * sizeof(Particle), particleVertexBufferLayout);
			particleInstance->particleReadWriteBuffer[i]->Put(&initParticle, sizeof(Particle));
		}

		particleInstance->buffersInitialized = true;
	}


	// ParticleRenderer2D Implementation: (Implements function from Renderer2D (except Renderer2D::Init())) ---------------------------------------------------------------
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ParticlesRenderer2D::Init()
	{
		VertexBufferLayout dummyBufferLayout;

		particlesRendererStorage.perFrameData = UniformBuffer::CreateUniformBuffer(sizeof(PerFrameData), dummyBufferLayout, UniformBufferBinding::PARTICLE_EMITTER_PER_FRAME_DATA);
		particlesRendererStorage.particleEmitterSettings = UniformBuffer::CreateUniformBuffer(sizeof(ParticleEmitterSettings), dummyBufferLayout, UniformBufferBinding::PARTICLE_EMITTER_SETTINGS);

		particlesRendererStorage.transformFeedback = TransformFeedback::CreateTransformFeedback(outAttributes, TransformFeedbackPrimitive::Points);

		// TODO: Change this after using a proper asset management system
		particlesRendererStorage.emitterShader = Shader::CreateShader(particleEmitterShaderPath);
		particlesRendererStorage.emitterShader->LoadShader(particlesRendererStorage.transformFeedback);

		particlesRendererStorage.rendererShader = Shader::CreateShader(particleRendererShaderPath);
		particlesRendererStorage.rendererShader->LoadShader(nullptr);

		particlesRendererStorage.queryObject = QueryObject::CreateQueryObject(QueryTarget::TransformFeedbackPrimitivesWritten);
	}

	void ParticlesRenderer2D::RenderFrame(float timestep)
	{

		for (auto& particleInstance : particlesRendererStorage.particleInstances)
		{
			InitializeBuffers(particleInstance);

			// Bind the emitter shader:
			particlesRendererStorage.emitterShader->Bind();

			// Setting the "perFrameDataStruct" for the uniform buffer "perFrameData":
			{
				particlesRendererStorage.perFrameDataStruct.frameTime = timestep;

				float xFactor = 1.0f;
				float yFactor = 1.0f;
				float zFactor = 1.0f;

				particlesRendererStorage.perFrameDataStruct.randomSeed = {
					realRandom(randomEngine) * xFactor,
					realRandom(randomEngine) * yFactor,
					realRandom(randomEngine) * zFactor
				};

				// TODO: Work on the Emission type:
				// if (particleInstance->particleSettings.emissionType == EmissionType::RateOverTime)
				// 	perFrameDataStruct.numToGenerate = (int32_t)(particleInstance->particleSettings.rate - particleInstance->numParticlesInBuffer) < 0 ? 
				// 		0 : particleInstance->particleSettings.rate - particleInstance->numParticlesInBuffer;

				// TODO: Fix this:
				particlesRendererStorage.perFrameDataStruct.numToGenerate = particleInstance->particleSettings.rate;
			}

			// XEN_ENGINE_LOG_ERROR("{0}, {1}", perFrameDataStruct.numToGenerate, particleInstance->numParticlesInBuffer);

			// Setting the "particleEmitterSettingsStruct" for the uniform buffer "particleEmitterSettings":
			{
				particlesRendererStorage.particleEmitterSettingsStruct.genPosition = particleInstance->particleSettings.position;

				// For the color gradient:
				auto colorSliders = particleInstance->particleSettings.colorGradient.GetColorSliders();
				auto alphaSliders = particleInstance->particleSettings.colorGradient.GetAlphaSliders();

				std::sort(colorSliders.begin(), colorSliders.end());
				std::sort(alphaSliders.begin(), alphaSliders.end());

				for (int i = 0; i < colorSliders.size(); i++) {
					particlesRendererStorage.particleEmitterSettingsStruct.colorGradient.colors[i].color = colorSliders[i].color;
					particlesRendererStorage.particleEmitterSettingsStruct.colorGradient.colors[i].sliderValue = colorSliders[i].sliderValue;
				}
				for (int i = 0; i < alphaSliders.size(); i++) {
					particlesRendererStorage.particleEmitterSettingsStruct.colorGradient.alphas[i].alpha = alphaSliders[i].alpha;
					particlesRendererStorage.particleEmitterSettingsStruct.colorGradient.alphas[i].sliderValue = alphaSliders[i].sliderValue;
				}

				particlesRendererStorage.particleEmitterSettingsStruct.colorGradient.colorSliderSize = colorSliders.size();
				particlesRendererStorage.particleEmitterSettingsStruct.colorGradient.alphaSliderSize = alphaSliders.size();

				particlesRendererStorage.particleEmitterSettingsStruct.affectColorBy = (uint32_t)particleInstance->particleSettings.affectColorBy;
				particlesRendererStorage.particleEmitterSettingsStruct.colorDistanceScale = particleInstance->particleSettings.colorDistanceScale;

				particlesRendererStorage.particleEmitterSettingsStruct.lifeRange = particleInstance->particleSettings.lifeRandomRange;
				particlesRendererStorage.particleEmitterSettingsStruct.sizeRange = particleInstance->particleSettings.sizeRandomRange;
				particlesRendererStorage.particleEmitterSettingsStruct.velocityRange = particleInstance->particleSettings.velocityRandomRange;

				particlesRendererStorage.particleEmitterSettingsStruct.velocityDamping = particleInstance->particleSettings.velocityDamping;
			}

			particlesRendererStorage.particleEmitterSettings->Put(0, &particlesRendererStorage.particleEmitterSettingsStruct, sizeof(ParticleEmitterSettings));
			particlesRendererStorage.perFrameData->Put(0, &particlesRendererStorage.perFrameDataStruct, sizeof(PerFrameData));

			// Emit particles from the emitter shader:
			{
				RenderCommand::EnableRasterizer(false);

				particlesRendererStorage.emitterShader->Bind();
				particlesRendererStorage.transformFeedback->Bind();

				particlesRendererStorage.transformFeedback->SetFeedbackBuffer(particleInstance->particleReadWriteBuffer[1 - particleInstance->currentBuffer]);

				particlesRendererStorage.queryObject->BeginQuery();
				particlesRendererStorage.transformFeedback->BeginFeedback();

				RenderCommand::DrawNonIndexed(
					PrimitiveType::Points,
					particleInstance->particleReadWriteBuffer[particleInstance->currentBuffer],
					particleInstance->numParticlesInBuffer
				);

				particlesRendererStorage.transformFeedback->EndFeedback();
				particlesRendererStorage.queryObject->EndQuery();

				particlesRendererStorage.queryObject->GetQueryResult((const void*)&particleInstance->numParticlesInBuffer);

				particleInstance->currentBuffer = 1 - particleInstance->currentBuffer;
				particlesRendererStorage.transformFeedback->Unbind();

				RenderCommand::EnableRasterizer(true);
			}

			//Render Particles:
			{
				particlesRendererStorage.rendererShader->Bind();

				RenderCommand::DrawNonIndexed(
					PrimitiveType::Points,
					particleInstance->particleReadWriteBuffer[particleInstance->currentBuffer],
					particleInstance->numParticlesInBuffer
				);
			}
		}

		if (particlesRendererStorage.particleInstanceIndex + 1 < particlesRendererStorage.particleInstanceCount)
		{
			uint32_t difference = particlesRendererStorage.particleInstanceCount - (particlesRendererStorage.particleInstanceIndex + 1);
			for (int i = 0; i < difference; i++)
				particlesRendererStorage.particleInstances.pop_back();
		}
	}

	void ParticlesRenderer2D::BeginScene(const Ref<Camera>& camera)
	{
		particlesRendererStorage.particleInstanceIndex = 0;
	}

	void ParticlesRenderer2D::EndScene()
	{
		
	}
	
	// Draw Functions Implementation: (Functions from Renderer2D.h) -------------------------------------------------------------------------
	// List of functions implemented: 
	//		Renderer2D::DrawParticles(const ParticleSettings2D& particleSettings)
	// --------------------------------------------------------------------------------------------------------------------------------------

	void Renderer2D::DrawParticles(const ParticleSettings2D& particleSettings)
	{
		if (particlesRendererStorage.particleInstanceIndex >= particlesRendererStorage.particleInstanceCount)
		{
			particlesRendererStorage.particleInstances.push_back(std::make_shared<ParticleInstance2D>());
			particlesRendererStorage.particleInstanceCount++;
		}

		particlesRendererStorage.particleInstances[particlesRendererStorage.particleInstanceIndex]->particleSettings = particleSettings;

		// For now genPosition is same as particleSettings.position
		particlesRendererStorage.particleInstances[particlesRendererStorage.particleInstanceIndex]->instancePosition = particleSettings.position;

		particlesRendererStorage.particleInstanceIndex++;
	}
}