#pragma once

#include <DawnStar/DawnStar.hpp>


struct ImRect;

namespace DawnStar
{

    class ObjectListPanel
    {
    public:
        ObjectListPanel();
        ~ObjectListPanel() = default;

        void OnImGuiRender();

        void SetContext(Ref<Scene>& context);

    private:
        const ImRect DrawEntityNode(Entity entity);

        void DrawComponents(Entity entity);

    private:
        Ref<Scene> m_Context;
        Entity m_SelectionEntity;
    };    
}