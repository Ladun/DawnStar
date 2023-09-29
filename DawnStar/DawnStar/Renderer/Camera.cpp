#include <DawnStar/dspch.hpp>
#include <DawnStar/Renderer/Camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace DawnStar
{
	Camera::Camera()
	{
		DS_PROFILE_SCOPE()
		
		RecalculateProjection();
	}    

	void Camera::SetPerspective(float verticalFov, float nearClip, float farClip)
	{
		DS_PROFILE_SCOPE()
		
		m_ProjectionType = ProjectionType::Perspective;

		
		m_PerspectiveFOV = verticalFov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		RecalculateProjection();
	}

	void Camera::SetOrthographic(float size, float nearClip, float farClip)
	{
		DS_PROFILE_SCOPE()
		
		m_ProjectionType = ProjectionType::Orthographic;
		
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;

		RecalculateProjection();
	}

	void Camera::SetViewportSize(uint32_t width, uint32_t height)
	{
		DS_PROFILE_SCOPE()
		
		m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
		RecalculateProjection();
	}

	void Camera::RecalculateProjection()
	{
		DS_PROFILE_SCOPE()
		
		if(m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			const float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			const float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			const float orthoBottom = -m_OrthographicSize * 0.5f;
			const float orthoTop = m_OrthographicSize * 0.5f;

			m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
		}
	}
	
} // namespace DawnStar
