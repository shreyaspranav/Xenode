#include "pch"
#include "LuaFunctions.h"

#include <core/app/Log.h>
#include <core/app/Input.h>
#include <core/app/DesktopApplication.h>
#include <lua.hpp>

#include <core/scene/Components.h>
#include <core/physics/Physics2D.h>

namespace Xen {

	Entity currentEntity;
	Ref<Input> input;

	void LuaFunctions::Init() { input = Input::GetInputInterface(); input->SetWindow(DesktopApplication::GetWindow()); }
	void LuaFunctions::SetCurrentEntity(const Entity& entity) { currentEntity = entity; }

	int LuaFunctions::lua_IsKeyPressed(lua_State* L)
	{
		std::string key = lua_tostring(L, 1);

		if(key.size() == 1)
			lua_pushboolean(L, IsKeyPressed(key.at(0)));
		else
			lua_pushboolean(L, IsKeyPressed(key));

		return 1;
	}

	int LuaFunctions::lua_IsMouseButtonPressed(lua_State* L)
	{
		int key = lua_tointeger(L, 1);
		
		if (key > 7)
			return 0;

		lua_pushboolean(L, (int)input->IsMouseButtonPressed((MouseKeyCode)key));

		return 1;
	}

	int LuaFunctions::lua_GetNormalizedMouseCoords2D(lua_State* L)
	{
		Scene* scene = currentEntity.GetParentScene();

		Entity cameraEntity = scene->GetPrimaryCameraEntity();

		if (!cameraEntity.IsNull())
		{
			Vec2 to_return;

			uint32_t mouseX = scene->GetMouseX();
			uint32_t mouseY = scene->GetMouseY();

			// Mouse is outside viewport
			if (mouseX == -1 || mouseY == -1)
				return 0;

			Component::CameraComp& camera = cameraEntity.GetComponent<Component::CameraComp>();

			uint32_t frameBufferWidth = camera.camera->GetFrameBufferWidth();
			uint32_t frameBufferHeight = camera.camera->GetFrameBufferHeight();

			// Invert the Y coordinate:
			mouseY = frameBufferHeight - mouseY;

			Vec3 scale = camera.camera->GetScale();
			Vec3 position = camera.camera->GetPosition();

			float aspectRatio = (float)frameBufferWidth / (float)frameBufferHeight;
			
			to_return.x = (((((float)mouseX / (float)frameBufferWidth ) * 2.0f) - 1.0f) * scale.x * aspectRatio) + position.x;
			to_return.y = (((((float)mouseY / (float)frameBufferHeight) * 2.0f) - 1.0f) * scale.y) + position.y;

			lua_pushnumber(L, to_return.x);
			lua_pushnumber(L, to_return.y);

			return 2;
		}
		else {
			XEN_ENGINE_LOG_ERROR("Primary Camera Entity not found!");
			return 0;
		}
	}

	int LuaFunctions::lua_GetCurrentTransform(lua_State* L)
	{ 
		Component::Transform& transform = currentEntity.GetComponent<Component::Transform>();

		lua_createtable(L, 0, 3);
		{
			lua_pushstring(L, "position");
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

			lua_pushstring(L, "rotation");
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

			lua_pushstring(L, "scale");
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

		lua_pushstring(L, "position");
		lua_gettable(L, -2);
		
		{
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
		}

		// TODO: Complete the implemetation for rotation and scale
		return 0;
	}

	int LuaFunctions::lua_ApplyForce2D(lua_State* L)
	{
		if (currentEntity.HasAnyComponent<Component::RigidBody2D>())
		{
			float pointX = lua_tonumber(L, 1);
			float pointY = lua_tonumber(L, 2);
			float forceX = lua_tonumber(L, 3);
			float forceY = lua_tonumber(L, 4);

			Component::RigidBody2D& rb = currentEntity.GetComponent<Component::RigidBody2D>();
			PhysicsBody2D* physicsBody = rb.runtimePhysicsBody;

			Physics2D::ApplyForce(physicsBody, { pointX, pointY }, { forceX, forceY });
		}

		return 0;
	}

	int LuaFunctions::lua_ApplyForceToCentre2D(lua_State* L)
	{
		if (currentEntity.HasAnyComponent<Component::RigidBody2D>())
		{
			Component::RigidBody2D& rb = currentEntity.GetComponent<Component::RigidBody2D>();
			PhysicsBody2D* physicsBody = rb.runtimePhysicsBody;

			float x = lua_tonumber(L, 1);
			float y = lua_tonumber(L, 2);

			Physics2D::ApplyForceToCenter(physicsBody, { x, y });
		}

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
		char k = toupper(key);

		switch (k)
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
			return false;
		}
	}

	bool LuaFunctions::IsKeyPressed(std::string& key)
	{
		char* k = (char*)key.c_str();

		for (int i = 0; i < key.size(); i++)
			k[i] = std::toupper(key.at(i));

			 if (strcmp(key.c_str(), "TAB")		== 0)		return input->IsKeyPressed(KeyCode::KEY_TAB);
		else if (strcmp(key.c_str(), "LSHIFT")	== 0)		return input->IsKeyPressed(KeyCode::KEY_LEFT_SHIFT);
		else if (strcmp(key.c_str(), "LCTRL")	== 0)		return input->IsKeyPressed(KeyCode::KEY_LEFT_CONTROL);
		else if (strcmp(key.c_str(), "LALT")	== 0)		return input->IsKeyPressed(KeyCode::KEY_LEFT_ALT);
		else if (strcmp(key.c_str(), "RALT")	== 0)		return input->IsKeyPressed(KeyCode::KEY_RIGHT_ALT);
		else if (strcmp(key.c_str(), "RCTRL")	== 0)		return input->IsKeyPressed(KeyCode::KEY_RIGHT_CONTROL);
		else if (strcmp(key.c_str(), "RSHIFT")	== 0)		return input->IsKeyPressed(KeyCode::KEY_RIGHT_SHIFT);
		else if (strcmp(key.c_str(), "ENTER")	== 0)		return input->IsKeyPressed(KeyCode::KEY_ENTER);
		else if (strcmp(key.c_str(), "UP")		== 0)		return input->IsKeyPressed(KeyCode::KEY_UP);
		else if (strcmp(key.c_str(), "DOWN")	== 0)		return input->IsKeyPressed(KeyCode::KEY_DOWN);
		else if (strcmp(key.c_str(), "LEFT")	== 0)		return input->IsKeyPressed(KeyCode::KEY_LEFT);
		else if (strcmp(key.c_str(), "RIGHT")	== 0)		return input->IsKeyPressed(KeyCode::KEY_RIGHT);
		else	XEN_APP_LOG_ERROR("Lua: {0} is not a key!", key);

		return false;
	}

}