#include "Application.hpp"

#include <iostream>
#include <chrono>
#include <thread>

#include "Log.hpp"
#include "Assert.hpp"
#include "../Platform/Window.hpp"
#include "../Renderer/Vulkan/VulkanInstance.hpp"
#include "../Renderer/Vulkan/VulkanSurface.hpp"
#include "../Renderer/Vulkan/VulkanDebugMessenger.hpp"

namespace Raven
{
	void Application::Run()
	{
		auto window = Window::Create();

		const auto requiredExtensions = 
			window->GetRequiredVulkanInstanceExtensions();
		
		std::cout << "Required Vulkan instance extensions:\n";
		for (const char* name : requiredExtensions)
			std::cout << "  " << name << '\n';

		VulkanInstance instance(requiredExtensions);

		#ifdef RAVEN_ENABLE_VALIDATION
		VulkanDebugMessenger debugMessenger(instance);
		#endif // RAVEN_ENABLE_VALIDATION

		VulkanSurface surface(instance, *window);
		Log(LogLevel::Info, "Vulkan instance and surface created");

		while (!window->ShouldClose())
		{
			window->PollEvents();
			if (window->IsKeyDown(Key::Escape))
				Log(LogLevel::Warning, "Escape Pressed");
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}
}
