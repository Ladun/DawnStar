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
		float m_FpsValues[50];
		std::vector<float> m_FrameTimes;
    };    
} // namespace DawnStar
