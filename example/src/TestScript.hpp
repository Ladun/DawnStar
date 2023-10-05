#pragma once

#include <DawnStar/DawnStar.hpp>

class TestScript : public DawnStar::ScriptableEntity
{
protected:
    virtual void OnCreate() override;
    virtual void OnDestroy() override;
    virtual void OnUpdate(DawnStar::Timestep ts) override;
};