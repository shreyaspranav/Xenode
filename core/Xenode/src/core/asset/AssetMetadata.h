#pragma once
#include <Core.h>

#include <core/scene/Components.h>
#include <core/renderer/Texture.h>
#include <core/renderer/Shader.h>

namespace Xen
{
	// Metadata that is useful both during runtime and in editor ------------------------
	using TextureMetadata = struct { TextureProperties properties; TextureBufferType bufferType; };
	using ShaderMetadata  = UnorderedMap<ShaderType, Span>;
	using SceneMetadata   = struct {};
	using ScriptMetadata  = struct {};

	using AssetSpecificMetadata = std::variant<
		std::monostate,
		TextureMetadata,
		ShaderMetadata,
		SceneMetadata,
		ScriptMetadata
	>;

	// This is required to be used both during runtime and in the editor.
	struct AssetMetadata
	{
		AssetType type;
		Size size;
		Vector<AssetHandle> dependencies;

		// Depending on the type of the asset, holds specific metadata.
		AssetSpecificMetadata specific;
	};

	// Metadata that is useful only in the editor side -----------------------------------
	using EditorTextureMetadata = struct {};
	using EditorShaderMetadata  = struct {};
	using EditorSceneMetadata   = struct { Component::Transform editorCameraTransform; };
	using EditorScriptMetadata  = struct {};

	using EditorAssetSpecificMetadata = std::variant<
		std::monostate,
		EditorTextureMetadata,
		EditorShaderMetadata,
		EditorSceneMetadata,
		EditorScriptMetadata
	>;

	struct EditorAssetMetadata : AssetMetadata
	{
		std::filesystem::path relPath;
		Ref<Texture2D> thumbnail = nullptr;

		// Depending on the type of the asset, holds specific metadata.
		EditorAssetSpecificMetadata editorSpecific;
	};
}