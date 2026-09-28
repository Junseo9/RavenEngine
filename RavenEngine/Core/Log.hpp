#pragma once

#include <source_location>
#include <string_view>

namespace Raven
{
	enum class LogLevel
	{
		Info, Warning, Error,
	};

	void Log(
		LogLevel level,
		std::string_view message,
		std::source_location location = std::source_location::current());
}
