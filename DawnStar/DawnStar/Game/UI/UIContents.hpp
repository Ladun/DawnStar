#pragma once

#include <DawnStar/Scene/Scene.hpp>
#include <DawnStar/Scene/Entity.hpp>

namespace DawnStar::UI
{
    
    Entity CreateInputText(Ref<Scene> scene, std::string name="Input Text");
} // namespace DawnStar
