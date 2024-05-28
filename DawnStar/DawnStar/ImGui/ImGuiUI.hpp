#pragma once

#include <glm/gtc/type_ptr.hpp>

namespace DawnStar
{
    class ImGuiUI    
    {
    public:
        static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
    
        static void DrawVec2Control(const std::string& label, glm::vec2& values, 
                                    const glm::vec2 minValue={0.0f, 0.0f}, const glm::vec2 maxValue={0.0f, 0.0f}, 
                                    const float speed=0.1f, float resetValue=0.0f, float columnWidth=100.0f);
        static void DrawFloatControl(const std::string& label, float* value, const float minValue=0.0f, const float maxValue=0.0f, 
                                     const float speed=0.1f, float resetValue=0.0f, float columnWidth=100.0f);
    
    };
} // namespace DawnStar::UI
