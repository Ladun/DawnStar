#pragma once

#include <glm/gtc/type_ptr.hpp>

namespace DawnStar
{
    class ImGuiUI    
    {
    public:
        static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
    
    };
} // namespace DawnStar::UI
