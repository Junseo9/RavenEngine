// RavenEngine.cpp : Defines the entry point for the application.
//

#include "RavenEngine.h"
#include "Core/Application.hpp"
#include <exception>
#include "Core/Log.hpp"

int main()
{
	try
	{
		Raven::Application app;
		app.Run();

		return 0;
	}
	catch (const std::exception& error)
	{
		Raven::Log(Raven::LogLevel::Error, error.what());
		return 1;
	}
}
