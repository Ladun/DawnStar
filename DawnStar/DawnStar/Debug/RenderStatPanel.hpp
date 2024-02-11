#pragma once

namespace DawnStar
{
    class RenderStatPanel
    {
    public:
        RenderStatPanel();
        ~RenderStatPanel() = default;

        void OnImGuiRender();

    private:
        float _fpsValues[50];
        std::vector<float> _frameTimes;
    };    
} // namespace DawnStar
