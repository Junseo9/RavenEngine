#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "Input.hpp"

namespace Raven
{
	struct WindowDesc
	{
		std::string Title = "RavenEngine";
		std::uint32_t Width = 1280;
		std::uint32_t Height = 720;
	};

	class Window
	{
	public:

		virtual ~Window() = default;

		virtual void PollEvents() = 0;
		virtual bool ShouldClose() const = 0;

		virtual std::uint32_t GetWidth() const = 0;
		virtual std::uint32_t GetHeight() const = 0;
		virtual std::vector<const char*> 
			GetRequiredVulkanInstanceExtensions() const = 0;
		virtual const InputState& GetInputState() const = 0;

		virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) const = 0;

		static std::unique_ptr<Window> 
			Create(const WindowDesc& desc = WindowDesc());

	private:

	};
}
