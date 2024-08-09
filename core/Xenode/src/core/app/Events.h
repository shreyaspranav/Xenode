#pragma once

#include <Core.h>
#include "input/KeyboardInput.h"
#include "input/MouseInput.h"

namespace Xen 
{

	// Private macros: (Not meant to be used in other files) --------------------------------------------------------------------------

	// Implements the Event Type getter functions.
#define _DEFINE_EVENT(type) static EventType GetStaticType() { return type; }\
							EventType GetEventType() const override { return GetStaticType(); }

#ifndef XEN_PRODUCTION
	
// To be used with _TO_STRING_IMPL(event, ...)
#define _TO_STRING_MEMBER(member) ss << #member << ": " << member << ", ";

// Macro to define the overridden ToString method
#define _TO_STRING_IMPL(event, ...) \
    std::string ToString() const override \
	{ \
        std::stringstream ss; \
        ss << #event << ": "; \
		__VA_ARGS__ \
        std::string result = ss.str(); \
        if (!result.empty() && result[result.size() - 2] == ',') \
		{ \
            result.pop_back(); \
            result.pop_back(); \
        } \
        return result; \
    }

#else
#define _TO_STRING_MEMBER(member)
#define _TO_STRING_IMPL(event, ...)
#endif
	// ---------------------------------------------------------------------------------------------------------------------------

	// The Type of the Event, If you need to implement a event, 
	// insert here and also implement the event class 
	enum class EventType
	{
		// Window Events:
		WindowMoveEvent, WindowResizeEvent, WindowCloseEvent, WindowFocusEvent, WindowMinimizeEvent, WindowMaximizeEvent,

		// Keyboard Events:
		KeyboardEvent, CharEnterEvent,

		// Mouse Events:
		MouseEnterEvent, MouseMoveEvent, MouseButtonEvent, MouseScrollEvent,

		// Monitor Events:
		MonitorConnectEvent, MonitorDisconnectEvent
	};

	// The base class of an Event.
	class XEN_API Event
	{
	public:
		bool handled = 0;

		virtual ~Event() {}
		
		virtual EventType GetEventType() const = 0;
		virtual std::string ToString() const = 0;
	};

	//---------------------- Window Events:-------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------------------

	class XEN_API WindowMoveEvent : public Event
	{
	private:
		uint32_t m_XPos, m_YPos;

	public:
		WindowMoveEvent(uint32_t xpos, uint32_t ypos) : m_XPos(xpos), m_YPos(ypos) {}

		virtual ~WindowMoveEvent() {}

		uint32_t GetXPos() const { return m_XPos; }
		uint32_t GetYPos() const { return m_YPos; }

		_DEFINE_EVENT(EventType::WindowMoveEvent);
		_TO_STRING_IMPL(WindowMoveEvent, _TO_STRING_MEMBER(m_XPos) _TO_STRING_MEMBER(m_YPos));
	};

	class XEN_API WindowResizeEvent : public Event
	{
	private:
		uint32_t m_Width, m_Height;

	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

		virtual ~WindowResizeEvent() {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		_DEFINE_EVENT(EventType::WindowResizeEvent);
		_TO_STRING_IMPL(WindowResizeEvent, _TO_STRING_MEMBER(m_Width) _TO_STRING_MEMBER(m_Height));
	};

	class XEN_API WindowCloseEvent : public Event
	{
	private:
		int m_ExitCode;

	public:
		static const int USER_EXIT = 1;
		static const int ENGINE_EXIT = 0;
		static const int ENGINE_ERROR_EXIT = -1;

		WindowCloseEvent() { m_ExitCode = 0; }
		WindowCloseEvent(int exitCode) : m_ExitCode(exitCode) {}

		virtual ~WindowCloseEvent() {}

		int GetExitCode() const { return m_ExitCode; }
		
		_DEFINE_EVENT(EventType::WindowCloseEvent);
		_TO_STRING_IMPL(WindowCloseEvent, _TO_STRING_MEMBER(m_ExitCode));
	};

	class XEN_API WindowFocusEvent : public Event
	{
	private:
		bool m_Focused;

	public:
		WindowFocusEvent(bool focused) : m_Focused(focused) {}
		virtual ~WindowFocusEvent() {}

		bool IsFocused() const { return m_Focused; }

		_DEFINE_EVENT(EventType::WindowFocusEvent);
		_TO_STRING_IMPL(WindowFocusEvent, _TO_STRING_MEMBER(m_Focused));
	};

	class XEN_API WindowMinimizeEvent : public Event
	{
	private:
		bool m_Minimised;

	public:
		WindowMinimizeEvent(bool minimised) : m_Minimised(minimised) {}
		virtual ~WindowMinimizeEvent() {}

		bool IsMinimised() const { return m_Minimised; }

		_DEFINE_EVENT(EventType::WindowMinimizeEvent);
		_TO_STRING_IMPL(WindowMinimizeEvent, _TO_STRING_MEMBER(m_Minimised));
	};

	class XEN_API WindowMaximizeEvent : public Event
	{
	private:
		bool m_Maximised;

	public:
		WindowMaximizeEvent(bool maximised) : m_Maximised(maximised) {}
		virtual ~WindowMaximizeEvent() {}

		bool IsMaximised() const { return m_Maximised; }

		_DEFINE_EVENT(EventType::WindowMaximizeEvent);
		_TO_STRING_IMPL(WindowMaximizeEvent, _TO_STRING_MEMBER(m_Maximised));
	};

	//---------------------- Key Events:----------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------

	class KeyboardEvent : public Event
	{
	private:
		KeyboardKeyCode m_Code;
		KeyAction m_Action;

	public:
		KeyboardEvent(KeyboardKeyCode code, KeyAction action = KeyAction::Press) : m_Code(code), m_Action(action) {}

		virtual ~KeyboardEvent() {}

		KeyboardKeyCode GetKey() const { return m_Code; }
		KeyAction GetAction() const { return m_Action; }

		_DEFINE_EVENT(EventType::KeyboardEvent);
		_TO_STRING_IMPL(KeyboardEvent, _TO_STRING_MEMBER(m_Code) _TO_STRING_MEMBER(m_Action));
	};

	class CharEnterEvent : public Event
	{
	private:
		uint32_t m_CharCode;
	public:
		CharEnterEvent(uint32_t code) : m_CharCode(code) {}
		virtual ~CharEnterEvent() {}

		uint32_t GetChar() const { return m_CharCode; }

		_DEFINE_EVENT(EventType::CharEnterEvent);
		_TO_STRING_IMPL(CharEnterEvent, _TO_STRING_MEMBER(m_CharCode));
	};

	//---------------------- Mouse Events:----------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------

	class MouseEnterEvent : public Event
	{
	private:
		bool m_IsInWindow;

	public:
		MouseEnterEvent(bool entered) : m_IsInWindow(entered) {}
		virtual ~MouseEnterEvent() {}

		bool IsInWindow() { return m_IsInWindow; }

		_DEFINE_EVENT(EventType::MouseEnterEvent);
		_TO_STRING_IMPL(MouseEnterEvent, _TO_STRING_MEMBER(m_IsInWindow));
	};

	class MouseMoveEvent : public Event
	{
	private:
		uint16_t m_X, m_Y;

	public:
		MouseMoveEvent(uint16_t x, uint16_t y) : m_X(x), m_Y(y) {}
		virtual ~MouseMoveEvent() {}

		uint16_t GetX() const { return m_X; }
		uint16_t GetY() const { return m_Y; }

		_DEFINE_EVENT(EventType::MouseMoveEvent);
		_TO_STRING_IMPL(MouseMoveEvent, _TO_STRING_MEMBER(m_X) _TO_STRING_MEMBER(m_Y));
	};

	class MouseButtonEvent : public Event
	{
	private:
		MouseButtonCode m_Code;
		MouseButtonAction m_Action;

	public:
		MouseButtonEvent(MouseButtonCode code, MouseButtonAction action = MouseButtonAction::Press) 
			: m_Code(code), m_Action(action) {}
		virtual ~MouseButtonEvent() {}

		MouseButtonCode GetMouseButtonCode() { return m_Code; }
		MouseButtonAction GetMouseButtonAction() { return m_Action; }

		_DEFINE_EVENT(EventType::MouseButtonEvent);
		_TO_STRING_IMPL(MouseButtonEvent, _TO_STRING_MEMBER(m_Code) _TO_STRING_MEMBER(m_Action));
	};

	class MouseScrollEvent : public Event
	{
	private:
		float m_XOffset, m_YOffset;

	public:
		MouseScrollEvent(float xoff, float yoff) : m_XOffset(xoff), m_YOffset(yoff) {}
		virtual ~MouseScrollEvent() {}

		float GetXOffset() { return m_XOffset; }
		float GetYOffset() { return m_YOffset; }

		_DEFINE_EVENT(EventType::MouseScrollEvent);
		_TO_STRING_IMPL(MouseScrollEvent, _TO_STRING_MEMBER(m_XOffset) _TO_STRING_MEMBER(m_YOffset));
	};

	//---------------------- Monitor Events:-----------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------------------------------------

	class MonitorConnectEvent : public Event
	{
	private:
		uint8_t m_MonitorCount;

	public:
		MonitorConnectEvent(uint8_t monitorCount) : m_MonitorCount(monitorCount) {}
		virtual ~MonitorConnectEvent() {}

		uint8_t GetMonitorCount() { return m_MonitorCount; }

		_DEFINE_EVENT(EventType::MonitorConnectEvent);
		_TO_STRING_IMPL(MonitorConnectEvent, _TO_STRING_MEMBER(m_MonitorCount));
	};

	class MonitorDisconnectEvent : public Event
	{
	private:
		uint8_t m_MonitorCount;

	public:
		MonitorDisconnectEvent(uint8_t monitorCount) : m_MonitorCount(monitorCount) {}
		virtual ~MonitorDisconnectEvent() {}

		uint8_t GetMonitorCount() { return m_MonitorCount; }

		_DEFINE_EVENT(EventType::MonitorDisconnectEvent);
		_TO_STRING_IMPL(MonitorDisconnectEvent, _TO_STRING_MEMBER(m_MonitorCount));
	};
}