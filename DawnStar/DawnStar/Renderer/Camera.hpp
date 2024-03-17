#pragma once

#include <glm/glm.hpp>

namespace DawnStar
{
    class Camera
    {
	public:
		enum class ProjectionType {Perspective = 0, Orthographic = 1};
    public:
        Camera();
		explicit Camera(const glm::mat4& projection)
			: _projection(projection) {}
		~Camera() = default;

		void SetPerspective(float verticalFov, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);
		
		void SetViewportSize(uint32_t width, uint32_t height);

		[[nodiscard]] float GetPerspectiveVerticalFOV() const { return _perspectiveFOV; }
		void SetPerspectiveVerticalFOV(float verticalFov) { _perspectiveFOV = verticalFov; RecalculateProjection(); }
		[[nodiscard]] float GetPerspectiveNearClip() const { return _perspectiveNear; }
		void SetPerspectiveNearClip(float nearClip) { _perspectiveNear = nearClip; RecalculateProjection(); }
		[[nodiscard]] float GetPerspectiveFarClip() const { return _perspectiveFar; }
		void SetPerspectiveFarClip(float farClip) { _perspectiveFar = farClip; RecalculateProjection(); }
		
		[[nodiscard]] float GetOrthographicSize() const { return _orthographicSize; }
		void SetOrthographicSize(float size) { _orthographicSize = size; RecalculateProjection(); }
		[[nodiscard]] float GetOrthographicNearClip() const { return _orthographicNear; }
		void SetOrthographicNearClip(float nearClip) { _orthographicNear = nearClip; RecalculateProjection(); }
		[[nodiscard]] float GetOrthographicFarClip() const { return _orthographicFar; }
		void SetOrthographicFarClip(float farClip) { _orthographicFar = farClip; RecalculateProjection(); }
		
		[[nodiscard]] ProjectionType GetProjectionType() const { return _projectionType; }
		void SetProjectionType(ProjectionType type) { _projectionType = type; RecalculateProjection(); }		
		
		[[nodiscard]] const glm::mat4& GetProjection() const { return _projection; }

	private:
		void RecalculateProjection();

	private:
		friend class ObjectListPanel;

		ProjectionType _projectionType = ProjectionType::Perspective;

		glm::mat4 _projection = glm::mat4(1.0f);
		
		float _perspectiveFOV = glm::radians(45.0f);
		float _perspectiveNear = 0.01f, _perspectiveFar = 1000.0f;
		
		float _orthographicSize = 10.0f;
		float _orthographicNear = -1.0f, _orthographicFar = 1.0f;

		float _aspectRatio = 0.0f;
    };
} // namespace DawnStar
