#include <pch>

#include "ParticleSystem2D.h"

#include "BufferObjectBindings.h"
#include "Buffer.h"
#include "Shader.h"
#include "Structs.h"
#include "RenderCommand.h"
#include "QueryObject.h"

#include <core/app/Log.h>

#include <glad/gl.h>

namespace Xen {

	// Maximum number of particles that can fit in a buffer:
	const uint32_t maxParticleBufferCount = 1000000;

	const std::string particleEmitterShaderPath  = "assets/shaders/particle_emitter.shader";
	const std::string particleRendererShaderPath = "assets/shaders/particle_renderer.shader";

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
	}perFrameDataStruct;

	struct ColorSliderGLSL 
	{ 
		Color color; 
		float sliderValue; 
		Vec3 __DUMMY_PADDING__; 
	};

	struct AlphaSliderGLSL 
	{ 
		float alpha; 
		float sliderValue; 
		Vec2 __DUMMY_PADDING__; 
	};

	struct ColorGradientGLSL 
	{ 
		ColorSliderGLSL colors[8]; 
		AlphaSliderGLSL alphas[8]; 
		uint32_t colorSliderSize, alphaSliderSize; 
	};

	// For Uniform Buffer m_ParticleSettings
	struct ParticleEmitterSettings
	{
		Vec3 genPosition;
		float __DUMMY_OFFSET_1__;

		ColorGradientGLSL colorGradient;
		Vec2 __DUMMY_OFFSET_2__;

		// Used to represent AffectColorBy
		uint32_t affectColorBy;
		float colorDistanceScale;
		Vec2 __DUMMY_OFFESET_3__;

		Color startColor, endColor;
		Vec2 lifeRange, sizeRange, velocityRange;
		float velocityDamping;

		float __DUMMY_OFFESET_4__;		
	}particleEmitterSettingsStruct;

	// Private Globals:------------------------------------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------

	Ref<Shader> particleEmitterShader, particleRendererShader;
	Ref<TransformFeedback> transformFeedback;

	Ref<UniformBuffer> perFrameData, particleEmitterSettings;

	// Random Number Generation:
	std::random_device randomDevice;
	std::mt19937 randomEngine(randomDevice());
	std::uniform_real_distribution<float> realRandom;

	// Output Attributes from the geometry shader of the particle_emitter shader program:
	std::vector<std::string> outAttributes = {
		"outPosition",
		"outOriginalGenPosition",
		"outVelocity",
		"outSize",
		"outColor",
		"outCurrentLifetime",
		"outTotalLifetime",
		"outParticleType"
	};

	Ref<QueryObject> queryObject;

	// --------------------------------------------------------------------------------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------------------------------------------------------------------------------

	void ParticleSystem2D::Initialize()
	{
		VertexBufferLayout dummyBufferLayout;

		perFrameData = UniformBuffer::CreateUniformBuffer(sizeof(PerFrameData), dummyBufferLayout, UniformBufferBinding::PARTICLE_EMITTER_PER_FRAME_DATA);
		particleEmitterSettings = UniformBuffer::CreateUniformBuffer(sizeof(ParticleEmitterSettings), dummyBufferLayout, UniformBufferBinding::PARTICLE_EMITTER_SETTINGS);

		transformFeedback = TransformFeedback::CreateTransformFeedback(outAttributes, TransformFeedbackPrimitive::Points);

		// TODO: Change this after using a proper asset management system
		particleEmitterShader = Shader::CreateShader("assets/shaders/particle_emitter.shader");
		particleRendererShader = Shader::CreateShader("assets/shaders/particle_renderer.shader");

		particleEmitterShader->LoadShader(transformFeedback);
		particleRendererShader->LoadShader(nullptr);

		queryObject = QueryObject::CreateQueryObject(QueryTarget::TransformFeedbackPrimitivesWritten);
	}


	void ParticleSystem2D::RenderParticles(ParticleInstance2D* particleInstance, double timestep)
	{
		ParticleSystem2D::InitializeBuffers(particleInstance);

		// Bind the emitter shader:
		particleEmitterShader->Bind();

		// Setting the "perFrameDataStruct" for the uniform buffer "perFrameData":
		perFrameDataStruct.frameTime = timestep;

		float xFactor = 1.0f;
		float yFactor = 1.0f;
		float zFactor = 1.0f;

		perFrameDataStruct.randomSeed = {
			realRandom(randomEngine) * xFactor,
			realRandom(randomEngine) * yFactor,
			realRandom(randomEngine) * zFactor
		};

		// TODO: Work on the Emission type:
		// if (particleInstance->particleSettings.emissionType == EmissionType::RateOverTime)
		// 	perFrameDataStruct.numToGenerate = (int32_t)(particleInstance->particleSettings.rate - particleInstance->numParticlesInBuffer) < 0 ? 
		// 		0 : particleInstance->particleSettings.rate - particleInstance->numParticlesInBuffer;

		perFrameDataStruct.numToGenerate = particleInstance->particleSettings.rate;

		// XEN_ENGINE_LOG_ERROR("{0}, {1}", perFrameDataStruct.numToGenerate, particleInstance->numParticlesInBuffer);

		// Setting the "particleEmitterSettingsStruct" for the uniform buffer "particleEmitterSettings":
		particleEmitterSettingsStruct.genPosition = particleInstance->particleSettings.position;

		// For the color gradient:
		auto colorSliders = particleInstance->particleSettings.colorGradient.GetColorSliders();
		auto alphaSliders = particleInstance->particleSettings.colorGradient.GetAlphaSliders();

		std::sort(colorSliders.begin(), colorSliders.end());
		std::sort(alphaSliders.begin(), alphaSliders.end());

		for (int i = 0; i < colorSliders.size(); i++) {
			particleEmitterSettingsStruct.colorGradient.colors[i].color = colorSliders[i].color;
			particleEmitterSettingsStruct.colorGradient.colors[i].sliderValue = colorSliders[i].sliderValue;
		}
		for (int i = 0; i < alphaSliders.size(); i++) {
			particleEmitterSettingsStruct.colorGradient.alphas[i].alpha = alphaSliders[i].alpha;
			particleEmitterSettingsStruct.colorGradient.alphas[i].sliderValue = alphaSliders[i].sliderValue;
		}

		particleEmitterSettingsStruct.colorGradient.colorSliderSize = colorSliders.size();
		particleEmitterSettingsStruct.colorGradient.alphaSliderSize = alphaSliders.size();

		particleEmitterSettingsStruct.affectColorBy = (uint32_t)particleInstance->particleSettings.affectColorBy;
		particleEmitterSettingsStruct.colorDistanceScale = particleInstance->particleSettings.colorDistanceScale;

		particleEmitterSettingsStruct.lifeRange = particleInstance->particleSettings.lifeRandomRange;
		particleEmitterSettingsStruct.sizeRange = particleInstance->particleSettings.sizeRandomRange;
		particleEmitterSettingsStruct.velocityRange = particleInstance->particleSettings.velocityRandomRange;

		particleEmitterSettingsStruct.velocityDamping = particleInstance->particleSettings.velocityDamping;

		perFrameData->Put(0, &perFrameDataStruct, sizeof(PerFrameData));
		particleEmitterSettings->Put(0, &particleEmitterSettingsStruct, sizeof(ParticleEmitterSettings));

		// Emit particles from the emitter shader:
		{
			RenderCommand::EnableRasterizer(false);

			particleEmitterShader->Bind();

			transformFeedback->Bind();

			transformFeedback->SetFeedbackBuffer(particleInstance->particleReadWriteBuffer[1 - particleInstance->currentBuffer]);

			queryObject->BeginQuery();
			transformFeedback->BeginFeedback();

			RenderCommand::DrawNonIndexed(
				PrimitiveType::Points, 
				particleInstance->particleReadWriteBuffer[particleInstance->currentBuffer], 
				particleInstance->numParticlesInBuffer
			);

			transformFeedback->EndFeedback();
			queryObject->EndQuery();

			queryObject->GetQueryResult((const void*)&particleInstance->numParticlesInBuffer);

			particleInstance->currentBuffer = 1 - particleInstance->currentBuffer;
			transformFeedback->Unbind();

			RenderCommand::EnableRasterizer(true);
		}

		//Render Particles:
		{
			particleRendererShader->Bind();

			RenderCommand::DrawNonIndexed(
				PrimitiveType::Points,
				particleInstance->particleReadWriteBuffer[particleInstance->currentBuffer],
				particleInstance->numParticlesInBuffer
			);
		}
	}

	// Private Methods: ----------------------------------------------------------------------------------------------------------
	void ParticleSystem2D::InitializeBuffers(ParticleInstance2D* particleInstance)
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
}
