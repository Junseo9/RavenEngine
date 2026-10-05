#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.h>
#include "Win32Window.hpp"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <windowsx.h>

namespace Raven
{
	namespace
	{
		constexpr wchar_t ClassName[] = L"RavenEngineWindow";

		std::wstring ToWide(const std::string& text)
		{
			const int count = MultiByteToWideChar(
				CP_UTF8, MB_ERR_INVALID_CHARS, text.c_str(), -1, nullptr, 0);

			if (count == 0)
				throw std::runtime_error("Invalid UTF-8 window title");

			std::wstring result(static_cast<std::size_t>(count), L'\0');
			if (MultiByteToWideChar(
				CP_UTF8, MB_ERR_INVALID_CHARS, text.c_str(), -1,
				result.data(), count) == 0)
				throw std::runtime_error("Could not convert window title");

			return result;
		}

		Key TranslateKey(WPARAM virtualKey, LPARAM lParam)
		{
			if (virtualKey == VK_SHIFT)
			{
				const UINT scanCode = LOBYTE(HIWORD(lParam));
				virtualKey = MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX);
			}
			else if (virtualKey == VK_CONTROL || virtualKey == VK_MENU)
			{
				// Scan-code lookup can produce language keys on Korean layouts.
				const bool extended = (HIWORD(lParam) & KF_EXTENDED) != 0;
				if (virtualKey == VK_CONTROL)
					virtualKey = extended ? VK_RCONTROL : VK_LCONTROL;
				else
					virtualKey = extended ? VK_RMENU : VK_LMENU;
			}

			if (virtualKey >= 'A' && virtualKey <= 'Z')
			{
				return static_cast<Key>(
					static_cast<std::uint16_t>(Key::A) +
					static_cast<std::uint16_t>(virtualKey - 'A'));
			}

			if (virtualKey >= '1' && virtualKey <= '9')
			{
				return static_cast<Key>(
					static_cast<std::uint16_t>(Key::Digit1) +
					static_cast<std::uint16_t>(virtualKey - '1'));
			}

			switch (virtualKey)
			{
			case VK_LSHIFT:   return Key::LeftShift;
			case VK_RSHIFT:   return Key::RightShift;
			case VK_LCONTROL: return Key::LeftControl;
			case VK_RCONTROL: return Key::RightControl;
			case VK_LMENU:    return Key::LeftAlt;
			case VK_RMENU:    return Key::RightAlt;
			case VK_LWIN:     return Key::LeftSuper;
			case VK_RWIN:     return Key::RightSuper;
			case '0':       return Key::Digit0;
			case VK_ESCAPE: return Key::Escape;
			case VK_RETURN: return Key::Enter;
			case VK_BACK:   return Key::Backspace;
			case VK_TAB:    return Key::Tab;
			case VK_SPACE:  return Key::Space;
			case VK_INSERT: return Key::Insert;
			case VK_HOME:   return Key::Home;
			case VK_PRIOR:  return Key::PageUp;
			case VK_DELETE: return Key::Delete;
			case VK_END:    return Key::End;
			case VK_NEXT:   return Key::PageDown;
			case VK_LEFT:   return Key::Left;
			case VK_RIGHT:  return Key::Right;
			case VK_UP:     return Key::Up;
			case VK_DOWN:   return Key::Down;
			default:        return Key::Unknown;
			}
		}

		void UpdateButton(ButtonState& state, bool down)
		{
			if (down)
			{
				if (!state.Down)
					state.Pressed = true;
			}
			else if (state.Down)
			{
				state.Released = true;
			}

			state.Down = down;
		}

		void UpdateMouseButton(
			HWND handle,
			InputState& input,
			MouseButton button,
			bool down)
		{
			const auto index = static_cast<std::size_t>(button);
			if (index >= input.MouseButtons.size())
				return;

			UpdateButton(input.MouseButtons[index], down);

			if (down)
			{
				SetCapture(handle);
				return;
			}

			for (const ButtonState& state : input.MouseButtons)
			{
				if (state.Down)
					return;
			}

			if (GetCapture() == handle)
				ReleaseCapture();
		}
	}

	Win32Window::Win32Window(const WindowDesc& desc)
		: m_Width(desc.Width), m_Height(desc.Height)
	{
		const HINSTANCE instance = GetModuleHandleW(nullptr);

		WNDCLASSW windowClass{};
		windowClass.lpfnWndProc = &Win32Window::WindowProc;
		windowClass.hInstance = instance;
		windowClass.lpszClassName = ClassName;
		windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		windowClass.hbrBackground = GetSysColorBrush(COLOR_WINDOW);

		if (!RegisterClassW(&windowClass) &&
			GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
			throw std::runtime_error("Could not register window class");

		RECT bounds{
			0, 0,
			static_cast<LONG>(desc.Width),
			static_cast<LONG>(desc.Height)
		};
		if (!AdjustWindowRect(&bounds, WS_OVERLAPPEDWINDOW, FALSE))
			throw std::runtime_error("Could not calculate window size");

		const std::wstring title = ToWide(desc.Title);
		m_Handle = CreateWindowExW(
			0, ClassName, title.c_str(), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			bounds.right - bounds.left,
			bounds.bottom - bounds.top,
			nullptr, nullptr, instance, this);

		if (!m_Handle)
			throw std::runtime_error("Could not create window");

		ShowWindow(m_Handle, SW_SHOW);
	}

	Win32Window::~Win32Window()
	{
		if (m_Handle)
			DestroyWindow(m_Handle);
	}

	void Win32Window::PollEvents()
	{
		m_Input.ClearTransientState();

		MSG message{};
		while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				m_ShouldClose = true;
				continue;
			}

			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}

	bool Win32Window::ShouldClose() const { return m_ShouldClose; }
	std::uint32_t Win32Window::GetWidth() const { return m_Width; }
	std::uint32_t Win32Window::GetHeight() const { return m_Height; }

	std::vector<const char*> Win32Window::GetRequiredVulkanInstanceExtensions() const
	{
		return {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
		};
	}

	VkSurfaceKHR Win32Window::CreateVulkanSurface(VkInstance instance) const
	{
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hinstance = GetModuleHandleW(nullptr);
		createInfo.hwnd = m_Handle;

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		const VkResult result =
			vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
		if (result != VK_SUCCESS)
			throw std::runtime_error(
				"vkCreateWin32SurfaceKHR failed: " + std::to_string(result));

		return surface;
	}

	const InputState& Win32Window::GetInputState() const
	{
		return m_Input;
	}

	LRESULT CALLBACK Win32Window::WindowProc(
		HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_NCCREATE)
		{
			auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
			SetWindowLongPtrW(
				handle, GWLP_USERDATA,
				reinterpret_cast<LONG_PTR>(create->lpCreateParams));
		}

		auto* self = reinterpret_cast<Win32Window*>(
			GetWindowLongPtrW(handle, GWLP_USERDATA));

		if (self)
		{
			switch (message)
			{
			case WM_CLOSE:
			case WM_DESTROY:
				self->m_ShouldClose = true;
				return 0;
			case WM_SIZE:
			{
				RECT client{};
				if (GetClientRect(handle, &client))
				{
					self->m_Width = static_cast<std::uint32_t>(client.right);
					self->m_Height = static_cast<std::uint32_t>(client.bottom);
				}
				return 0;
			}
			case WM_MOUSEMOVE:
			{
				const std::int32_t x = GET_X_LPARAM(lParam);
				const std::int32_t y = GET_Y_LPARAM(lParam);

				if (self->m_Input.HasCursorPosition)
				{
					self->m_Input.CursorDeltaX += x - self->m_Input.CursorX;
					self->m_Input.CursorDeltaY += y - self->m_Input.CursorY;
				}
				else
				{
					self->m_Input.HasCursorPosition = true;
				}

				self->m_Input.CursorX = x;
				self->m_Input.CursorY = y;
				return 0;
			}
			case WM_LBUTTONDOWN:
				UpdateMouseButton(
					handle, self->m_Input, MouseButton::Left, true);
				return 0;
			case WM_LBUTTONUP:
				UpdateMouseButton(
					handle, self->m_Input, MouseButton::Left, false);
				return 0;
			case WM_RBUTTONDOWN:
				UpdateMouseButton(
					handle, self->m_Input, MouseButton::Right, true);
				return 0;
			case WM_RBUTTONUP:
				UpdateMouseButton(
					handle, self->m_Input, MouseButton::Right, false);
				return 0;
			case WM_MBUTTONDOWN:
				UpdateMouseButton(
					handle, self->m_Input, MouseButton::Middle, true);
				return 0;
			case WM_MBUTTONUP:
				UpdateMouseButton(
					handle, self->m_Input, MouseButton::Middle, false);
				return 0;
			case WM_XBUTTONDOWN:
			{
				const MouseButton button =
					GET_XBUTTON_WPARAM(wParam) == XBUTTON1
					? MouseButton::X1 : MouseButton::X2;
				UpdateMouseButton(handle, self->m_Input, button, true);
				return TRUE;
			}
			case WM_XBUTTONUP:
			{
				const MouseButton button =
					GET_XBUTTON_WPARAM(wParam) == XBUTTON1
					? MouseButton::X1 : MouseButton::X2;
				UpdateMouseButton(handle, self->m_Input, button, false);
				return TRUE;
			}
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			{
				const Key key = TranslateKey(wParam, lParam);
				if (key == Key::Unknown)
					break;

				const bool down =
					message == WM_KEYDOWN || message == WM_SYSKEYDOWN;

				self->m_Input.SetKeyDown(key, down);

				if (message == WM_KEYDOWN || message == WM_KEYUP)
					return 0;

				break;
			}
			case WM_KILLFOCUS:
				for (ButtonState& state : self->m_Input.Keys)
					UpdateButton(state, false);
				for (ButtonState& state : self->m_Input.MouseButtons)
					UpdateButton(state, false);
				if (GetCapture() == handle)
					ReleaseCapture();
				self->m_Input.HasCursorPosition = false;
				return 0;
			case WM_CAPTURECHANGED:
				for (ButtonState& state : self->m_Input.MouseButtons)
					UpdateButton(state, false);
				return 0;
			}
		}

		return DefWindowProcW(handle, message, wParam, lParam);
	}

	std::unique_ptr<Window> Window::Create(const WindowDesc& desc)
	{
		return std::make_unique<Win32Window>(desc);
	}
}
