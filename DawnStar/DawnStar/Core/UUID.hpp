#pragma once

// #include <DawnStar/Core/Core.hpp>

namespace DawnStar
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);

		operator uint64_t() const { return _UUID; }
	private:
		uint64_t _UUID;
	};
}// namespace DawnStar

namespace std
{
	template<>
	struct hash<DawnStar::UUID>
	{
		std::size_t operator()(const DawnStar::UUID& uuid) const noexcept
		{
			return hash<uint64_t>()(uuid);
		}
	};
}