#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.h>
#include "Win32Window.hpp"
#include <stdexcept>
#include <string>

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

	bool Win32Window::IsKeyDown(Key key) const
	{
		return key == Key::Escape && m_EscapeDown;
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
			case WM_KEYDOWN:
				if (wParam == VK_ESCAPE)
					self->m_EscapeDown = true;
				break;
			case WM_KEYUP:
				if (wParam == VK_ESCAPE)
					self->m_EscapeDown = false;
				break;
			case WM_KILLFOCUS:
				self->m_EscapeDown = false;
				break;
			}
		}

		return DefWindowProcW(handle, message, wParam, lParam);
	}

	std::unique_ptr<Window> Window::Create(const WindowDesc& desc)
	{
		return std::make_unique<Win32Window>(desc);
	}
}
