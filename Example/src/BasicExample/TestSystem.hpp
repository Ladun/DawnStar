#pragma once

#include <DawnStar.hpp>

using namespace DawnStar;

namespace BasicExample
{
    class TestSystem : public DawnStar::SystemBase
    {
    public:
        TestSystem(DawnStar::Ref<DawnStar::Scene> scene): DawnStar::SystemBase(scene) {}
    protected:
        virtual void OnUpdate(DawnStar::Timestep ts, entt::registry& registry) override;
    };
}