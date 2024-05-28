#include <dspch.hpp>
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
		
		_projectionType = ProjectionType::Perspective;

		
		_perspectiveFOV = verticalFov;
		_perspectiveNear = nearClip;
		_perspectiveFar = farClip;

		RecalculateProjection();
	}

	void Camera::SetOrthographic(float size, float nearClip, float farClip)
	{
		DS_PROFILE_SCOPE()
		
		_projectionType = ProjectionType::Orthographic;
		
		_orthographicSize = size;
		_orthographicNear = nearClip;
		_orthographicFar = farClip;

		RecalculateProjection();
	}

	void Camera::SetViewportSize(uint32_t width, uint32_t height)
	{
		DS_PROFILE_SCOPE()
		
		_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		RecalculateProjection();
	}

	void Camera::RecalculateProjection()
	{
		DS_PROFILE_SCOPE()
		
		if(_projectionType == ProjectionType::Perspective)
		{
			_projection = glm::perspective(_perspectiveFOV, _aspectRatio, _perspectiveNear, _perspectiveFar);
		}
		else
		{
			const float orthoLeft = -_orthographicSize * _aspectRatio * 0.5f;
			const float orthoRight = _orthographicSize * _aspectRatio * 0.5f;
			const float orthoBottom = -_orthographicSize * 0.5f;
			const float orthoTop = _orthographicSize * 0.5f;

			_projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, _orthographicNear, _orthographicFar);
		}
	}
	
} // namespace DawnStar
