#include "pch"
#include "LuaFunctions.h"

#include <core/app/Log.h>
#include <core/app/Input.h>
#include <core/app/DesktopApplication.h>
#include <lua.hpp>

#include <core/scene/Components.h>

namespace Xen {

	Entity currentEntity;
	Ref<Input> input;

	void LuaFunctions::Init() { input = Input::GetInputInterface(); input->SetWindow(DesktopApplication::GetWindow()); }
	void LuaFunctions::SetCurrentEntity(const Entity& entity) { currentEntity = entity; }

	int LuaFunctions::lua_IsKeyPressed(lua_State* L)
	{
		std::string key = lua_tostring(L, 1);
		XEN_ENGINE_LOG_INFO(key);

		lua_pushboolean(L, IsKeyPressed(key.at(0)));

		return 1;
	}

	int LuaFunctions::lua_GetCurrentTransform(lua_State* L)
	{
		Component::Transform& transform = currentEntity.GetComponent<Component::Transform>();

		lua_createtable(L, 0, 3);
		{
			lua_pushstring(L, "Position");
			lua_createtable(L, 0, 3);
			{

				lua_pushnumber(L, transform.position.x);
				lua_setfield(L, -2, "x");

				lua_pushnumber(L, transform.position.y);
				lua_setfield(L, -2, "y");

				lua_pushnumber(L, transform.position.z);
				lua_setfield(L, -2, "z");

				lua_settable(L, -3);
			}

			lua_pushstring(L, "Rotation");
			lua_createtable(L, 0, 3);
			{

				lua_pushnumber(L, transform.rotation.x);
				lua_setfield(L, -2, "x");

				lua_pushnumber(L, transform.rotation.y);
				lua_setfield(L, -2, "y");

				lua_pushnumber(L, transform.rotation.z);
				lua_setfield(L, -2, "z");

				lua_settable(L, -3);
			}

			lua_pushstring(L, "Scale");
			lua_createtable(L, 0, 3);
			{

				lua_pushnumber(L, transform.scale.x);
				lua_setfield(L, -2, "x");

				lua_pushnumber(L, transform.scale.y);
				lua_setfield(L, -2, "y");

				lua_pushnumber(L, transform.scale.z);
				lua_setfield(L, -2, "z");

				lua_settable(L, -3);
			}
		}

		return 1;
	}

	int LuaFunctions::lua_SetCurrentTransform(lua_State* L)
	{
		Component::Transform& transform = currentEntity.GetComponent<Component::Transform>();

		lua_pushstring(L, "Position");
		lua_gettable(L, -2);
		
		// position.x -----------------------
		lua_pushstring(L, "x");
		lua_gettable(L, -2);

		transform.position.x = lua_tonumber(L, -1);
		lua_pop(L, 1);

		// position.y -----------------------
		lua_pushstring(L, "y");
		lua_gettable(L, -2);

		transform.position.y = lua_tonumber(L, -1);
		lua_pop(L, 1);

		// position.z -----------------------
		lua_pushstring(L, "z");
		lua_gettable(L, -2);

		transform.position.z = lua_tonumber(L, -1);
		lua_pop(L, 1);

		return 0;
	}

	int LuaFunctions::lua_LogErrorSevere(lua_State* L)
	{
		std::string s = lua_tostring(L, 1);
		XEN_APP_LOG_ERROR_SEVERE(s);

		return 0;
	}
	int LuaFunctions::lua_LogError(lua_State* L)
	{
		std::string s = lua_tostring(L, 1);
		XEN_APP_LOG_ERROR(s);

		return 0;
	}
	int LuaFunctions::lua_LogWarning(lua_State* L)
	{
		std::string s = lua_tostring(L, 1);
		XEN_APP_LOG_WARN(s);

		return 0;
	}
	int LuaFunctions::lua_LogInfo(lua_State* L)
	{
		std::string s = lua_tostring(L, 1);
		XEN_APP_LOG_INFO(s);

		return 0;
	}
	int LuaFunctions::lua_LogTrace(lua_State* L)
	{	
		std::string s = lua_tostring(L, 1);
		XEN_APP_LOG_TRACE(s);

		return 0;
	}

	bool LuaFunctions::IsKeyPressed(char key)
	{
		if (key >= 'a' && key <= 'z')
			key -= 32;

		switch (key)
		{
		case 'A':	return input->IsKeyPressed(KeyCode::KEY_A);
		case 'B':	return input->IsKeyPressed(KeyCode::KEY_B);
		case 'C':	return input->IsKeyPressed(KeyCode::KEY_C);
		case 'D':	return input->IsKeyPressed(KeyCode::KEY_D);
		case 'E':	return input->IsKeyPressed(KeyCode::KEY_E);
		case 'F':	return input->IsKeyPressed(KeyCode::KEY_F);
		case 'G':	return input->IsKeyPressed(KeyCode::KEY_G);
		case 'H':	return input->IsKeyPressed(KeyCode::KEY_H);
		case 'I':	return input->IsKeyPressed(KeyCode::KEY_I);
		case 'J':	return input->IsKeyPressed(KeyCode::KEY_J);
		case 'K':	return input->IsKeyPressed(KeyCode::KEY_K);
		case 'L':	return input->IsKeyPressed(KeyCode::KEY_L);
		case 'M':	return input->IsKeyPressed(KeyCode::KEY_M);
		case 'N':	return input->IsKeyPressed(KeyCode::KEY_N);
		case 'O':	return input->IsKeyPressed(KeyCode::KEY_O);
		case 'P':	return input->IsKeyPressed(KeyCode::KEY_P);
		case 'Q':	return input->IsKeyPressed(KeyCode::KEY_Q);
		case 'R':	return input->IsKeyPressed(KeyCode::KEY_R);
		case 'S':	return input->IsKeyPressed(KeyCode::KEY_S);
		case 'T':	return input->IsKeyPressed(KeyCode::KEY_T);
		case 'U':	return input->IsKeyPressed(KeyCode::KEY_U);
		case 'V':	return input->IsKeyPressed(KeyCode::KEY_V);
		case 'W':	return input->IsKeyPressed(KeyCode::KEY_W);
		case 'X':	return input->IsKeyPressed(KeyCode::KEY_X);
		case 'Y':	return input->IsKeyPressed(KeyCode::KEY_Y);
		case 'Z':	return input->IsKeyPressed(KeyCode::KEY_Z);

		case '`':	return input->IsKeyPressed(KeyCode::KEY_GRAVE_ACCENT);
		case ';':	return input->IsKeyPressed(KeyCode::KEY_SEMICOLON);
		case '\'':	return input->IsKeyPressed(KeyCode::KEY_APOSTROPHE);
		case ',':	return input->IsKeyPressed(KeyCode::KEY_COMMA);
		case '.':	return input->IsKeyPressed(KeyCode::KEY_PERIOD);
		case '/':	return input->IsKeyPressed(KeyCode::KEY_SLASH);
		case '\\':	return input->IsKeyPressed(KeyCode::KEY_BACKSLASH);
		case '[':	return input->IsKeyPressed(KeyCode::KEY_LEFT_BRACKET);
		case ']':	return input->IsKeyPressed(KeyCode::KEY_RIGHT_BRACKET);
		case '-':	return input->IsKeyPressed(KeyCode::KEY_MINUS);
		case '=':	return input->IsKeyPressed(KeyCode::KEY_EQUAL);


		default:
			break;
		}
	}

}