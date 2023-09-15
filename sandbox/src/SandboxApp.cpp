#include <DawnStar/DawnStar.hpp>
#define DS_PLATFORM_WINDOWS
#include <DawnStar/Core/EntryPoint.hpp>

#include "Editor.hpp"

class Sandbox: public DawnStar::Application
{

public:
	Sandbox()
			: Application("Test", 800, 600)
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new TestLayer());
	}
	~Sandbox()
	{

	}
};

DawnStar::Application* DawnStar::CreateApplication()
{
    return new Sandbox();
};