#pragma once

namespace DawnStar
{
    
	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: _time(time)
		{

		}

		operator float() const { return _time; }

		float GetSeconds() const { return _time; }
		float GetMilliseconds() const { return _time * 1000.0f; }
	private:
		float _time;
	};
} // namespace DawnStar
