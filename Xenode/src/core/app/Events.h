#pragma once

#include <Core.h>
#include "KeyCodes.h"

namespace Xen {

	enum class EventType
	{

		// Window Events:
		WindowMoveEvent, WindowResizeEvent, WindowCloseEvent, WindowFocusEvent, WindowMinimizeEvent, WindowMaximizeEvent,

		// Keyboard Events:
		KeyPressEvent, KeyReleaseEvent, CharEnterEvent,

		// Mouse Events:
		MouseEnterEvent, MouseMoveEvent, MouseButtonPressEvent, MouseButtonReleaseEvent, MouseScrollEvent,

		// Monitor Events:
		MonitorConnectEvent, MonitorDisconnectEvent
	};

	class XEN_API Event
	{
	public:
		bool handled = 0;

		virtual ~Event() {}
		
		virtual EventType GetEventName() const = 0;
		virtual std::string ToString() const = 0;
	};


	//----------------------Window Events:---------------------------------------------------------
	//---------------------------------------------------------------------------------------------


	// Window Moved Event:---------------------------------------
	class XEN_API WindowMoveEvent : public Event
	{
	private:
		uint32_t m_XPos, m_YPos;

	public:
		WindowMoveEvent(uint32_t xpos, uint32_t ypos) : m_XPos(xpos), m_YPos(ypos) {}

		virtual ~WindowMoveEvent() {}

		uint32_t GetXPos() const { return m_XPos; }
		uint32_t GetYPos() const { return m_YPos; }

		EventType GetEventName() const override { return EventType::WindowMoveEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMoved: X_Pos: " << m_XPos << ", Y_Pos: " << m_YPos;
			return ss.str();
		}
	};


	// Window Resize Event:-------------------------------------
	class XEN_API WindowResizeEvent : public Event
	{
	private:
		uint32_t m_Width, m_Height;

	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

		virtual ~WindowResizeEvent() {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		EventType GetEventName() const override { return EventType::WindowResizeEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResize: Width " << m_Width << ", Height: " << m_Height;
			return ss.str();
		}
	};


	// Window Close Event:-------------------------------------
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
		
		EventType GetEventName() const override { return EventType::WindowCloseEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowClosed: " << m_ExitCode;
			return ss.str();
		}
	};


	// Window Focus Event:-------------------------------------
	class XEN_API WindowFocusEvent : public Event
	{
	private:
		bool m_Focused;

	public:
		WindowFocusEvent(bool focused) : m_Focused(focused) {}
		virtual ~WindowFocusEvent() {}

		bool IsFocused() const { return m_Focused; }

		EventType GetEventName() const override { return EventType::WindowFocusEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowFocus: " << m_Focused;
			return ss.str();
		}
	};


	// Window Minimise Event:-------------------------------------
	class XEN_API WindowMinimizeEvent : public Event
	{
	private:
		bool m_Minimised;

	public:
		WindowMinimizeEvent(bool minimised) : m_Minimised(minimised) {}
		virtual ~WindowMinimizeEvent() {}

		bool IsMinimised() const { return m_Minimised; }

		EventType GetEventName() const override { return EventType::WindowMinimizeEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMinimise: " << m_Minimised;
			return ss.str();
		}
	};


	// Window Maximise Event:-------------------------------------
	class XEN_API WindowMaximizeEvent : public Event
	{
	private:
		bool m_Maximised;

	public:
		WindowMaximizeEvent(bool maximised) : m_Maximised(maximised) {}
		virtual ~WindowMaximizeEvent() {}

		bool IsMaximised() const { return m_Maximised; }

		EventType GetEventName() const override { return EventType::WindowMaximizeEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMaximise: " << m_Maximised;
			return ss.str();
		}
	};

	//----------------------Key Events:------------------------------------------------------------
	//---------------------------------------------------------------------------------------------

	// Key Press Event:----------------------------
	class KeyPressEvent : public Event
	{
	private:
		KeyCode m_Code;
		bool m_IsRepeat;

	public:
		KeyPressEvent(KeyCode code) : m_Code(code) { m_IsRepeat = 0; }
		KeyPressEvent(KeyCode code, bool repeat) : m_Code(code), m_IsRepeat(repeat) {}

		virtual ~KeyPressEvent() {}

		KeyCode GetKey() const { return m_Code; }
		bool IsRepeat() { return m_IsRepeat; }

		EventType GetEventName() const override { return EventType::KeyPressEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressed: " << m_Code << ", Repeat: " << m_IsRepeat;
			return ss.str();
		}
	};

	// Key Release Event:----------------------------
	class KeyReleaseEvent : public Event
	{
	private:
		KeyCode m_Code;

	public:
		KeyReleaseEvent(KeyCode code) : m_Code(code) {}
		virtual ~KeyReleaseEvent() {}

		KeyCode GetKey() const { return m_Code; }

		EventType GetEventName() const override { return EventType::KeyReleaseEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleased: " << m_Code;
			return ss.str();
		}
	};

	class CharEnterEvent : public Event
	{
	private:
		uint32_t m_CharCode;
	public:
		CharEnterEvent(uint32_t code) : m_CharCode(code) {}
		virtual ~CharEnterEvent() {}

		uint32_t GetChar() const { return m_CharCode; }

		EventType GetEventName() const override { return EventType::CharEnterEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "CharEnter: " << m_CharCode;
			return ss.str();
		}
	};

	//----------------------Mouse Events:------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	// Mouse Enter Event:-----------------------------
	class MouseEnterEvent : public Event
	{
	private:
		bool m_IsInWindow;

	public:
		MouseEnterEvent(bool entered) : m_IsInWindow(entered) {}
		virtual ~MouseEnterEvent() {}

		bool IsInWindow() { return m_IsInWindow; }

		EventType GetEventName() const override { return EventType::MouseEnterEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseEntered: " << m_IsInWindow;
			return ss.str();
		}
	};

	// Mouse Move Event:------------------------------
	class MouseMoveEvent : public Event
	{
	private:
		uint16_t m_X, m_Y;

	public:
		MouseMoveEvent(uint16_t x, uint16_t y) : m_X(x), m_Y(y) {}
		virtual ~MouseMoveEvent() {}

		uint16_t GetX() const { return m_X; }
		uint16_t GetY() const { return m_Y; }

		EventType GetEventName() const override { return EventType::MouseMoveEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMoved: X:" << m_X << ", Y:" << m_Y;
			return ss.str();
		}
	};

	// Mouse Button Press Event:------------------------------
	class MouseButtonPressEvent : public Event
	{
	private:
		MouseKeyCode m_Code;

	public:
		MouseButtonPressEvent(MouseKeyCode code) : m_Code(code) {}
		virtual ~MouseButtonPressEvent() {}

		MouseKeyCode GetMouseKeyCode() { return m_Code; }

		EventType GetEventName() const override { return EventType::MouseButtonPressEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressed: " << m_Code;
			return ss.str();
		}
	};

	// Mouse Button Release Event:------------------------------
	class MouseButtonReleaseEvent: public Event
	{
	private:
		MouseKeyCode m_Code;

	public:
		MouseButtonReleaseEvent(MouseKeyCode code) : m_Code(code) {}
		virtual ~MouseButtonReleaseEvent() {}

		MouseKeyCode GetMouseKeyCode() { return m_Code; }

		EventType GetEventName() const override { return EventType::MouseButtonReleaseEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleased: " << m_Code;
			return ss.str();
		}
	};

	// Mouse Scroll Event:------------------------------
	class MouseScrollEvent : public Event
	{
	private:
		float m_XOffset, m_YOffset;

	public:
		MouseScrollEvent(float xoff, float yoff) : m_XOffset(xoff), m_YOffset(yoff) {}
		virtual ~MouseScrollEvent() {}

		float GetXOffset() { return m_XOffset; }
		float GetYOffset() { return m_YOffset; }

		EventType GetEventName() const override { return EventType::MouseScrollEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScroll: X:" << m_XOffset << " ,Y:" << m_YOffset;
			return ss.str();
		}
	};

	//----------------------Monitor Events:------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	// Monitor Connect Event:--------------------------
	class MonitorConnectEvent : public Event
	{
	private:
		uint8_t m_MonitorCount;

	public:
		MonitorConnectEvent(uint8_t monitor_count) : m_MonitorCount(monitor_count) {}
		virtual ~MonitorConnectEvent() {}

		uint8_t GetMonitorCount() { return m_MonitorCount; }

		EventType GetEventName() const override { return EventType::MonitorConnectEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MonitorConnect: monitor_count: " << m_MonitorCount;
			return ss.str();
		}
	};

	// Monitor Disconnect Event:--------------------------
	class MonitorDisconnectEvent : public Event
	{
	private:
		uint8_t m_MonitorCount;

	public:
		MonitorDisconnectEvent(uint8_t monitor_count) : m_MonitorCount(monitor_count) {}
		virtual ~MonitorDisconnectEvent() {}

		uint8_t GetMonitorCount() { return m_MonitorCount; }

		EventType GetEventName() const override { return EventType::MonitorDisconnectEvent; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MonitorDisconnectEvent: monitor_count: " << m_MonitorCount;
			return ss.str();
		}
	};
}