#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include "../Window.hpp"

namespace Raven
{
	class Win32Window : public Window
	{
	public:
		explicit Win32Window(const WindowDesc& desc);
		~Win32Window() override;

		void PollEvents() override;
		bool ShouldClose() const override;
		std::uint32_t GetWidth() const override;
		std::uint32_t GetHeight() const override;
		std::vector<const char*> GetRequiredVulkanInstanceExtensions() const override;
		VkSurfaceKHR CreateVulkanSurface(VkInstance instance) const override;
		bool IsKeyDown(Key key) const override;

	private:
		static LRESULT CALLBACK WindowProc(
			HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
		HWND m_Handle = nullptr;
		std::uint32_t m_Width = 0;
		std::uint32_t m_Height = 0;
		bool m_ShouldClose = false;
		bool m_EscapeDown = false;
	};
}
