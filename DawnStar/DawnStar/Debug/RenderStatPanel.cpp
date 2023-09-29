#include <DawnStar/dspch.hpp>
#include <DawnStar/Debug/RenderStatPanel.hpp>

#include <DawnStar/Renderer/Renderer2D.hpp>
// #include <DawnStar/Renderer/Renderer3D.hpp>

#include <imgui.h>

namespace DawnStar
{
    RenderStatPanel::RenderStatPanel():
        m_FpsValues{}
    {

    }

    void RenderStatPanel::OnImGuiRender()
    {
        DS_PROFILE_SCOPE()

        float avg = 0.0f;

        const size_t size = m_FrameTimes.size();
        if(size >= 50)
            m_FrameTimes.erase(m_FrameTimes.begin());

        m_FrameTimes.emplace_back(ImGui::GetIO().Framerate);
        for (uint32_t i = 0; i < size; i++)
        {
            const float frameTime = m_FrameTimes[i];
            m_FpsValues[i] = frameTime;
            avg += frameTime;
        }
        
		avg /= static_cast<float>(size);
        
	    ImGui::Begin("Render Stat");

        {
            const auto stats = Renderer2D::GetStats();
            ImGui::Text("2D");

            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::SameLine();
            ImGui::PushItemWidth(-1);
            ImGui::Text("Quads: %d", stats.QuadCount);
            ImGui::SameLine();
            ImGui::PushItemWidth(-1);
            ImGui::Text("Tris: %d", stats.GetTotalTriangleCount());

            ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
            ImGui::SameLine();
            ImGui::PushItemWidth(-1);
            ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
        }

        ImGui::Separator();

        ImGui::PlotLines("##FPS", m_FpsValues, static_cast<int>(size));
        ImGui::Text("FPS: %lf", static_cast<double>(avg));
        const double fps = (1.0 / static_cast<double>(avg)) * 1000.0;
        ImGui::Text("Frame time (ms): %lf", fps);

        ImGui::End();
    }
} // namespace DawnStar
