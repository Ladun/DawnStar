#include <DawnStar/dspch.hpp>
#include "TestScript.hpp"

namespace BasicExample
{
    void TestScript::OnCreate()
    {
        auto& transform = GetComponent<DawnStar::TransformComponent>();
        transform.Translation = {0.0f, 0.0f, 0.0f};
    }

    void TestScript::OnDestroy()
    {
    }

    void TestScript::OnUpdate(DawnStar::Timestep ts)
    {
        auto& transform = GetComponent<DawnStar::TransformComponent>();
        transform.Rotation.z += ts * 1;
    }
}