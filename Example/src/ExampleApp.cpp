#include <DawnStar.hpp>
#define DS_PLATFORM_WINDOWS
#include <DawnStar/Core/EntryPoint.hpp>

#include "BasicExample/ExampleLayer.hpp"
#include "UIExample/ExampleLayer.hpp"
#include "Example3D/ExampleLayer.hpp"

class Sandbox: public DawnStar::Application
{

public:
	Sandbox()
			: Application("Test", 1080, 600)
	{
		// PushLayer(new BasicExample::ExampleLayer());
		 PushLayer(new UIExample::ExampleLayer());
		//PushLayer(new Example3D::ExampleLayer());
	}
	~Sandbox()
	{

	}
};

DawnStar::Application* DawnStar::CreateApplication()
{
    return new Sandbox();
};