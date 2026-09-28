#pragma once

#include "Log.hpp"
#include <cstdlib>

#ifndef NDEBUG
#define RAVEN_ASSERT(condition)                                      \
	do                                                               \
	{                                                                \
		if (!(condition))                                            \
		{                                                            \
			::Raven::Log(::Raven::LogLevel::Error,                   \
				"Assertion failed: " #condition);                    \
			std::abort();                                            \
		}                                                            \
	} while (false)
#else
#define RAVEN_ASSERT(condition) ((void)0)
#endif //NDEBUG
