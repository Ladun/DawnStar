#pragma once

namespace DawnStar
{
	class StringUtils
	{
	public:
		struct StringHash
		{
			using is_transparent = void;
			size_t operator()(const char* txt) const
			{
				return std::hash<std::string_view>{}(txt);
			}

			size_t operator()(std::string_view txt) const
			{
				return std::hash<std::string_view>{}(txt);
			}

			size_t operator()(const std::string& txt) const
			{
				return std::hash<std::string>{}(txt);
			}
		};
#define UM_StringTransparentEquality StringUtils::StringHash, std::equal_to<>

		static std::string_view GetExtension(const std::string_view& filepath)
		{
			DS_PROFILE_SCOPE()

			const auto lastDot = filepath.find_last_of('.');
			return filepath.substr(lastDot + 1, filepath.size() - lastDot);
		}

		static std::string_view GetName(const std::string_view& filepath)
		{
			DS_PROFILE_SCOPE()

			auto lastSlash = filepath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			const auto lastDot = filepath.rfind('.');
			const auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
			return filepath.substr(lastSlash, count);
		}

		static std::string_view GetNameWithExtension(const std::string_view& filepath)
		{
			DS_PROFILE_SCOPE()

			auto lastSlash = filepath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			return filepath.substr(lastSlash, filepath.size());
		}

		static void ReplaceString(std::string& subject, const std::string_view& search, const std::string_view& replace)
		{
			size_t pos = 0;
			const size_t replaceLength = replace.length();
			const size_t searchLength = search.length();

			while ((pos = subject.find(search, pos)) != std::string::npos)
			{
				subject.replace(pos, searchLength, replace);
				pos += replaceLength;
			}
		}
		
		static const char* FromChar8T(const char8_t* c)
		{
			return reinterpret_cast<const char*>(c);
		}
	};
}