#include "pch"
#include "ShaderCompiler.h"

#include <core/app/GameApplication.h>

// ShaderC
#include <shaderc/shaderc.hpp>

// SpirV Cross
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Xen
{
	// Global constants: ---------------------------------------------------------------------------
	// Don't optimize shaders in debug mode
#ifdef XEN_DEBUG
	const bool optimizeShaders = false;
#else
	const bool optimizeShaders = true;
#endif

	// Private functions: --------------------------------------------------------------------------
	std::string ToStringFromShaderType(ShaderType shaderType)
	{
		switch (shaderType)
		{
		case ShaderType::Vertex:                return "Vertex Shader";
		case ShaderType::Fragment:              return "Fragment Shader";
		case ShaderType::Geometry:              return "Geometry Shader";
		case ShaderType::Compute:               return "Compute Shader";
		case ShaderType::TessellationControl:   return "Tessellation Control Shader";
		case ShaderType::TessellationEvaluate:  return "Tessellation Evaluation Shader";
		}
	}

	shaderc_shader_kind ToShaderCShaderKind(ShaderType shaderType)
	{
		switch (shaderType)
		{
		case ShaderType::Vertex:                return shaderc_vertex_shader;
		case ShaderType::Fragment:              return shaderc_fragment_shader;
		case ShaderType::Geometry:              return shaderc_geometry_shader;
		case ShaderType::Compute:               return shaderc_compute_shader;
		case ShaderType::TessellationControl:   return shaderc_tess_control_shader;
		case ShaderType::TessellationEvaluate:  return shaderc_tess_evaluation_shader;
		}

		XEN_ENGINE_LOG_ERROR("Unknown shaderType!");
		TRIGGER_BREAKPOINT;

		return (shaderc_shader_kind)0;
	}

	// TODO: This should be temporary(or should it?):
	shaderc_target_env ToShaderCTargetEnv(GraphicsAPI api)
	{
		switch (api)
		{
		case GraphicsAPI::XEN_OPENGL_API:   return shaderc_target_env_opengl;
		case GraphicsAPI::XEN_VULKAN_API:   return shaderc_target_env_vulkan;
		}

		XEN_ENGINE_LOG_ERROR("Unknown api!");
		TRIGGER_BREAKPOINT;

		return (shaderc_target_env)0;
	}

	// TODO: This should be temporary(or should it?):
	uint32_t ToShaderCTargetEnvVersion(GraphicsAPI api)
	{
		switch (api)
		{
		case GraphicsAPI::XEN_OPENGL_API: return shaderc_env_version_opengl_4_5;
		case GraphicsAPI::XEN_VULKAN_API: return shaderc_env_version_vulkan_1_3;
		}

		XEN_ENGINE_LOG_ERROR("Unknown api!");
		TRIGGER_BREAKPOINT;

		return 0;
	}

	// Implementation: ---------------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------------------
	Buffer ShaderCompiler::CompileShader(
		const std::string& shaderSource, 
		const std::string& fileName,
		const Vector<Pair<std::string, std::string>>& defines, 
		ShaderType type)
	{
		GraphicsAPI currentAPI = GetApplicationInstance()->GetGraphicsAPI();

		Buffer vulkanSpirVBinary = CompileToVulkanSpirV(shaderSource, fileName, defines, type);
		if (currentAPI == GraphicsAPI::XEN_VULKAN_API)
			return vulkanSpirVBinary;
		else
		{
			const std::string& apiSpecificShaderSource = GetAPISpecificSource(vulkanSpirVBinary, type, currentAPI, fileName);
			ReflectSpirVBinary(vulkanSpirVBinary, type, currentAPI, fileName);

			// Free the vulkan spirv once the shader is cross compiled.
			vulkanSpirVBinary.Free();

			return GetFinalShaderBinary(apiSpecificShaderSource, type, currentAPI, fileName);
		}
	}

	// Private methods: -------------------------------------------------------------------------
	Buffer ShaderCompiler::CompileToVulkanSpirV(
		const std::string& shaderSource, 
		const std::string& fileName,
		const Vector<Pair<std::string, std::string>>& defines,
		ShaderType type)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		// Set the appropriate optimization level(No optimizations in the debug build).
		options.SetOptimizationLevel(
			optimizeShaders ? shaderc_optimization_level_performance : shaderc_optimization_level_zero
		);

		// The source language is GLSL and compiling for Vulkan
		options.SetSourceLanguage(shaderc_source_language_glsl);
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

		// Add all the #defines.
		for (auto& define : defines)
			options.AddMacroDefinition(define.first, define.second);

		Buffer vulkanSpirV;
		shaderc::CompilationResult compilationResult =
			compiler.CompileGlslToSpv(shaderSource, ToShaderCShaderKind(type), fileName.c_str());

		// Check for any errors and warnings:
		if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			XEN_ENGINE_LOG_ERROR("{0} shader compilation to Vulkan SPIR-V failed! ShaderType: {1}", 
				fileName, 
				ToStringFromShaderType(type)
			);

			XEN_ENGINE_LOG_ERROR("{0} error(s) found", compilationResult.GetNumErrors());
			XEN_ENGINE_LOG_ERROR("{0}", compilationResult.GetErrorMessage());

		}
		else
		{
			XEN_ENGINE_LOG_INFO("Shader file {0} as {1} compiler successfully with {2} warning(s)",
				fileName, 
				ToStringFromShaderType(type), 
				compilationResult.GetNumWarnings()
			);

			Size compiledBinaryBufferSize = compilationResult.end() - compilationResult.begin();
			uint32_t* binaryBuffer = new uint32_t[compiledBinaryBufferSize];

			vulkanSpirV.buffer = binaryBuffer;
			vulkanSpirV.size = compiledBinaryBufferSize;
			vulkanSpirV.alloc = true;
		}

		return vulkanSpirV;
	}

	std::string ShaderCompiler::GetAPISpecificSource(
		Buffer spirv, 
		ShaderType type, 
		GraphicsAPI targetAPI, 
		const std::string& fileName)
	{
		if (targetAPI == GraphicsAPI::XEN_OPENGL_API || targetAPI == GraphicsAPI::XEN_OPENGLES_API)
		{
			spirv_cross::CompilerGLSL glslCompiler(reinterpret_cast<uint32_t*>(spirv.buffer), spirv.size);
			spirv_cross::CompilerGLSL::Options options;

			// TODO: Look into more options.
			{
				options.es = targetAPI == GraphicsAPI::XEN_OPENGLES_API ? true : false;
				options.vulkan_semantics = false;
				options.version = 450;

				// Never include plain uniforms:(To maintain cross API compatibility)
				options.emit_push_constant_as_uniform_buffer = true;   // Emits a uniform buffer instead of plain uniforms
				options.emit_uniform_buffer_as_plain_uniforms = false; // Preserves Uniform buffer.


				// "Vertex-like shader" here is any shader stage that can write BuiltInPosition.

				// GLSL: In vertex-like shaders, rewrite [0, w] depth (Vulkan/D3D style) to [-w, w] depth (GL style).
				// MSL: In vertex-like shaders, rewrite [-w, w] depth (GL style) to [0, w] depth.
				// HLSL: In vertex-like shaders, rewrite [-w, w] depth (GL style) to [0, w] depth.
				options.vertex.fixup_clipspace = false;  // Default value
			}

			glslCompiler.set_common_options(options);
			return glslCompiler.compile();
		}

		else
		{
			XEN_ENGINE_LOG_ERROR("Not implemented for current Graphics API!");
			TRIGGER_BREAKPOINT;
		}
	}

	std::string ShaderCompiler::ReflectSpirVBinary(
		Buffer spirv, 
		ShaderType type, 
		GraphicsAPI targetAPI, 
		const std::string& fileName)
	{
		spirv_cross::Compiler compiler(reinterpret_cast<uint32_t*>(spirv.buffer), spirv.size);
		spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources();

		XEN_ENGINE_LOG_INFO("Shader Reflection: {0}, Stage: {1}----------", fileName, ToStringFromShaderType(type));

		// Uniform Buffers: 
		XEN_ENGINE_LOG_INFO("Uniform Buffers ------------- Count: {0}", shaderResources.uniform_buffers.size());
		for (auto& uniformBufferResource : shaderResources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(uniformBufferResource.base_type_id);
			Size bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t bindingIndex = compiler.get_decoration(uniformBufferResource.id, spv::DecorationBinding);
			Size memberCount = bufferType.member_types.size();

			XEN_ENGINE_LOG_INFO("\tSize: {0}", bufferSize);
			XEN_ENGINE_LOG_INFO("\tBinding Index: {0}", bindingIndex);
			XEN_ENGINE_LOG_INFO("\tMember Count: {0}", memberCount);
		}

		// Storage Buffers: 
		XEN_ENGINE_LOG_INFO("Storage Buffers ------------- Count: {0}", shaderResources.storage_buffers.size());
		for (auto& storageBufferResource : shaderResources.storage_buffers)
		{
			const auto& bufferType = compiler.get_type(storageBufferResource.base_type_id);
			Size bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t bindingIndex = compiler.get_decoration(storageBufferResource.id, spv::DecorationBinding);
			Size memberCount = bufferType.member_types.size();

			XEN_ENGINE_LOG_INFO("\tSize: {0}", bufferSize);
			XEN_ENGINE_LOG_INFO("\tBinding Index: {0}", bindingIndex);
			XEN_ENGINE_LOG_INFO("\tMember Count: {0}", memberCount);
		}

		// TODO: Add the Reflection to Textures, samplers and other resources too.

		XEN_ENGINE_LOG_INFO("End of shader reflection --------------------------------------------------------------");
		return "";
	}

	Buffer ShaderCompiler::GetFinalShaderBinary(
		const std::string& source, 
		ShaderType type, 
		GraphicsAPI targetAPI, 
		const std::string& fileName)
	{
		// Compile using ShaderC for OpenGL and Vulkan
		// Do some research for Direct3D
		if (targetAPI == GraphicsAPI::XEN_OPENGL_API || 
			targetAPI == GraphicsAPI::XEN_VULKAN_API || 
			targetAPI == GraphicsAPI::XEN_OPENGLES_API)
		{
			shaderc::Compiler compiler;
			shaderc::CompileOptions options;

			// Set the appropriate optimization level(No optimizations in the debug build).
			options.SetOptimizationLevel(
				optimizeShaders ? shaderc_optimization_level_performance : shaderc_optimization_level_zero
			);

			// The source language is GLSL for both OpenGL and Vulkan.
			options.SetSourceLanguage(shaderc_source_language_glsl);
			options.SetTargetEnvironment(ToShaderCTargetEnv(targetAPI), ToShaderCTargetEnvVersion(targetAPI));

			shaderc::CompilationResult compilationResult = compiler.CompileGlslToSpv(source, ToShaderCShaderKind(type), "DummyFileName");
			
			if (compilationResult.GetCompilationStatus() == shaderc_compilation_status_success)
			{
				Size compiledBinaryBufferSize = compilationResult.end() - compilationResult.begin();
				uint32_t* binaryBuffer = new uint32_t[compiledBinaryBufferSize];

				Buffer finalCompiledBinary;

				finalCompiledBinary.buffer = binaryBuffer;
				finalCompiledBinary.size = compiledBinaryBufferSize;
				finalCompiledBinary.alloc = true;

				return finalCompiledBinary;
			}
			else
			{
				// This should never happen because the shader is checked for errors 
				// before cross compiling to api specific source.
				TRIGGER_BREAKPOINT;
			}

		}
		else
		{
			// Look how to do it for other APIs(Metal and Direct3D)
		}
	}
}
