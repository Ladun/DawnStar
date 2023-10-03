#include <DawnStar/dspch.hpp>
#include <DawnStar/Game/World.hpp>

#include <DawnStar/Core/Application.hpp>

#include <DawnStar/Renderer/RenderCommand.hpp>
#include <DawnStar/Renderer/Renderer2D.hpp>
// #include <DawnStar/Renderer/Renderer3D.hpp>
#include <DawnStar/Scene/Components.hpp>
#include <DawnStar/Scene/Entity.hpp>

namespace DawnStar
{
    World::World()
        : m_WorldSetting({
            {0.1f, 0.1f, 0.1f, 0.1f}
        })
    {
        CreateScene("Main", true);
    }

    World::~World()
    {
    }

    Ref<Scene> World::CreateScene(std::string sceneName, 
                                  bool activeImmediately)
    {
		DS_PROFILE_SCOPE()

        Ref<Scene> scene = CreateRef<Scene>();

        // Set viewport size
        scene->OnViewportResize(DawnStar::Application::Get().GetWindow().GetWidth(),
								DawnStar::Application::Get().GetWindow().GetHeight());

        // Create assential entities;
        { // Camera
            Entity mainCam = scene->CreateEntity("Main Camera");
            mainCam.AddComponent<CameraComponent>();        
        }
        
        m_SceneMap.emplace(sceneName, scene);

        if(activeImmediately)
        {
            ActiveScene(scene);
        }

        return scene;
    }

    Ref<Scene> World::FindScene(std::string sceneName)
    {
		DS_PROFILE_SCOPE()

        const auto& it = m_SceneMap.find(sceneName);
        if(it != m_SceneMap.end())
            return it->second;

        return nullptr;        
    }

    void World::ActiveScene(std::string sceneName)
    {
        auto scene = FindScene(sceneName);
        DS_CORE_ASSERT(scene, "Active a wrong scene");

        ActiveScene(scene);
    }

    void World::ActiveScene(Ref<Scene> scene)
    {
        // TODO: deactivate scene (if need)
        m_ActiveScene = scene;
    }

    void World::OnUpdate(Timestep ts)
    {
        DawnStar::Renderer2D::ResetStats();
        DawnStar::RenderCommand::SetClearColor(m_WorldSetting.m_BackgroundColor);
        DawnStar::RenderCommand::Clear();

        m_ActiveScene->OnUpdate(ts);
    }

} // namespace DawnStar