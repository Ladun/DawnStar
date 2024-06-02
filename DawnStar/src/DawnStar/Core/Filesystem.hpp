#pragma once

#include <DawnStar/Core/Buffer.hpp>


namespace DawnStar
{
	class Filesystem
	{
	public:
		static bool IsPartOfDirectoryTree(const std::filesystem::path& filePath, const std::filesystem::path& rootPath);
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
		static std::string ReadFileText(const std::filesystem::path& filepath);
		static void WriteFileText(const std::filesystem::path& filepath, const std::string& buffer);
	};
}