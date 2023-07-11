#pragma once
#include <Core.h>
#include <imgui/IconsFontAwesome.h>

namespace Xen {
	namespace StringValues
	{
		// Panel titles:
		const std::string
			PANEL_TITLE_SCENE_HIERARCHY		= std::string(ICON_FA_LIST)          + std::string(" Scene Hierarchy"),
			PANEL_TITLE_SCENE_SETTINGS		= std::string(ICON_FA_GEAR)          + std::string(" Scene Settings"),
			PANEL_TITLE_PROPERTIES			= std::string(ICON_FA_INFO)          + std::string(" Properties"),
			PANEL_TITLE_CONTENT_BROWSER		= std::string(ICON_FA_FOLDER)        + std::string(" Content Browser"),
			PANEL_TITLE_VIEWPORT			= std::string(ICON_FA_MOUNTAIN_SUN)  + std::string(" 2D Viewport");

		// Component Titles:
		const std::string 
			COMPONENT_TRANSFORM				= std::string(ICON_FA_CUBES)         + std::string(" Transform"),
			COMPONENT_SPRITE_RENDERER		= std::string(ICON_FA_TREE)          + std::string(" Sprite Renderer"),
			COMPONENT_NATIVE_SCRIPT			= std::string(ICON_FA_FILE_CODE)     + std::string(" Native Script"),
			COMPONENT_SCRIPT				= std::string(ICON_FA_CODE)          + std::string(" Script"),
			COMPONENT_CAMERA				= std::string(ICON_FA_CAMERA)        + std::string(" Orthographic Camera"),
			COMPONENT_POINT_LIGHT			= std::string(ICON_FA_LIGHTBULB)     + std::string(" Point Light"),
			COMPONENT_AMBIENT_LIGHT			= std::string(ICON_FA_SUN)           + std::string(" Ambient Light"),
			COMPONENT_BOX_COLLIDER_2D		= std::string(ICON_FA_SQUARE)        + std::string(" Box Collider 2D"),
			COMPONENT_RIGID_BODY_2D			= std::string(ICON_FA_CUBES_STACKED) + std::string(" Rigid Body 2D");

		// Drop down options:
		const std::string
			DROPDOWN_SPRITETYPE_TRIANGLE	= std::string(ICON_FA_SHAPES) + std::string(" Triangle"),
			DROPDOWN_SPRITETYPE_QUAD		= std::string(ICON_FA_SQUARE) + std::string(" Quad"),
			DROPDOWN_SPRITETYPE_POLYGON		= std::string(ICON_FA_SHAPES) + std::string(" Polygon"),
			DROPDOWN_SPRITETYPE_CIRCLE		= std::string(ICON_FA_CIRCLE) + std::string(" Circle");
	}
}