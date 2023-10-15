#pragma once

#include <DawnStar/DawnStar.hpp>

namespace BasicExample
{
    class TestSystem : public DawnStar::SystemBase
    {
    protected:
        virtual void OnUpdate(DawnStar::Timestep ts, entt::registry& registry) override;
    };
}