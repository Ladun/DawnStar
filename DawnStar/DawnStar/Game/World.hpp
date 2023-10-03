#pragma once

#include <DawnStar/Core/Core.hpp>
#include <DawnStar/Scene/Scene.hpp>

#include <glm/vec4.hpp>

namespace DawnStar
{
    struct WorldSetting
    {
        glm::vec4 m_BackgroundColor;
    };

    class World
    {
    public:
        World();
        ~World();

        Ref<Scene> CreateScene(std::string sceneName, bool activeImmediately=true);
        Ref<Scene> FindScene(std::string sceneName);
        void ActiveScene(std::string sceneName);
        Ref<Scene> GetActiveScene() const { return m_ActiveScene; }
        
        void OnUpdate(Timestep ts);

        WorldSetting& GetWorldSetting() { return m_WorldSetting; }
    
    private:
        void ActiveScene(Ref<Scene> scene);

    private:
        Ref<Scene> m_ActiveScene;
        std::unordered_map<std::string, Ref<Scene>> m_SceneMap;

        WorldSetting m_WorldSetting;
    };
} // namespace DawnStar
